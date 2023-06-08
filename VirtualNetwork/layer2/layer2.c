#include <stdio.h>
#include "layer2.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> /*for inet_ntop & inet_pton*/

extern void l2_switch_recv_frame(interface_t *interface, char *pkt, unsigned int pkt_size);
extern void promote_pkt_to_layer3(
    node_t *node,
    interface_t *interface,
    char *pkt,
    unsigned int pkt_size,
    int L3_protocol_type
);


void interface_set_l2_mode(node_t *node, interface_t *interface, char *l2_mode_option) {
    intf_l2_mode_t intf_l2_mode;

    if(strncmp(l2_mode_option, "access", strlen("access")) == 0) {
        intf_l2_mode = ACCESS;    
    }
    else if(strncmp(l2_mode_option, "trunk", strlen("trunk")) ==0) {
        intf_l2_mode = TRUNK;
    }
    else{
        printf("Invalid mode %s\n", l2_mode_option);
        assert(0);
    }
    /*Case 1 : if interface is working in L3 mode, i.e. IP address is configured.
     * then disable ip address, and set interface in L2 mode*/
    if(IS_INTF_L3_MODE(interface)){
        interface->intf_nw_props.is_ipadd_config = false;
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }
    /*Case 2 : if interface is working neither in L2 mode or L3 mode, then
     * apply L2 config*/
    if(IF_L2_MODE(interface) == L2_MODE_UNKNOWN) {
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }
    /*case 3 : if interface is operating in same mode, and user config same mode
     * again, then do nothing*/
    if(IF_L2_MODE(interface) == intf_l2_mode){
        return;
    }
    /*case 4 : if interface is operating in access mode, and user config trunk mode,
     * then overwrite*/
    if(IF_L2_MODE(interface) == ACCESS && intf_l2_mode == TRUNK) {
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }
    /* case 5 : if interface is operating in trunk mode, and user config access mode,
     * then overwrite, remove all vlans from interface, user must enable vlan again 
     * on interface*/
    if(IF_L2_MODE(interface) == TRUNK && intf_l2_mode == ACCESS) {
        IF_L2_MODE(interface) = intf_l2_mode;
        for (int i=0 ; i < MAX_VLAN_MEMBERSHIP; i++){
            interface->intf_nw_props.vlans[i] = 0;
        }
    }
}

// used during topology creation
void node_set_intf_l2_mode(node_t *node, char *intf_name, intf_l2_mode_t intf_l2_mode) {
    interface_t *interface = get_node_if_by_name(node, intf_name);
    assert(interface);
    interface_set_l2_mode(node, interface, intf_l2_mode_str(intf_l2_mode));
}

static void promote_pkt_to_layer2(node_t *node, interface_t *iif, ethernet_hdr_t *ethernet_hdr, uint32_t pkt_size){
    switch(ethernet_hdr->type){
        case ARP_MSG:
            {
                /*Can be ARP Broadcast or ARP reply*/
                arp_hdr_t *arp_hdr = (arp_hdr_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr));
                switch(arp_hdr->op_code){
                    case ARP_BROAD_REQ:
                        process_arp_broadcast_request(node, iif, ethernet_hdr);
                        break;
                    case ARP_REPLY:
                        process_arp_reply_msg(node, iif, ethernet_hdr);
                        break;
                    default:
                        break;
                }
            }
            break;
        case ETH_IP:
            promote_pkt_to_layer3(
                node,
                iif,
                GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr),
                pkt_size - GET_ETH_HDR_SIZE_EXCL_PAYLOAD(ethernet_hdr),
                ethernet_hdr->type
            );
            break;
        default:
            ;
    }
}


// entry point of the stack
void layer2_frame_recv(node_t* node, interface_t* interface, char* pkt, unsigned int pkt_size) {
    unsigned int vlan_id_to_tag = 0;
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt;
    
    if (!l2_frame_recv_qualify_on_interface(interface, ethernet_hdr, &vlan_id_to_tag)){
        printf("L2 Frame Rejected on node %s\n", node->node_name);
        return;
    }
    printf("L2 Frame Accepted on node %s\n", node->node_name);

    /*Handle Reception of a L2 Frame on L3 Interface*/
    if (IS_INTF_L3_MODE(interface)) {
       promote_pkt_to_layer2(node, interface, ethernet_hdr, pkt_size);
    } else if (IF_L2_MODE(interface) == ACCESS || IF_L2_MODE(interface) == TRUNK) {
        unsigned int new_pkt_size = 0;
        if(vlan_id_to_tag) {
            pkt = (char *)tag_pkt_with_vlan_id(
                (ethernet_hdr_t *)pkt,
                pkt_size, vlan_id_to_tag,
                &new_pkt_size
            );
            assert(new_pkt_size != pkt_size);
            l2_switch_recv_frame(interface, pkt, new_pkt_size);
            return;
        }
        l2_switch_recv_frame(interface, pkt, pkt_size);
    } else {
        return; /*Do nothing, drop the packet*/
    }
}


// LAYER3 CONNECTION
static void l2_forward_ip_packet(
    node_t *node,
    unsigned int next_hop_ip,
    char *outgoing_intf,
    ethernet_hdr_t *pkt, 
    unsigned int pkt_size) {

    interface_t *oif = NULL;
    char next_hop_ip_str[16];
    arp_entry_t * arp_entry = NULL;
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt;
    unsigned int ethernet_payload_size = pkt_size - ETH_HDR_SIZE_EXCL_PAYLOAD;

    next_hop_ip = htonl(next_hop_ip);
    inet_ntop(AF_INET, &next_hop_ip, next_hop_ip_str, 16);
    
    /*restore again, since htonl reverses the byte order*/
    next_hop_ip = htonl(next_hop_ip);

    if(outgoing_intf) {
        /* Case 1 : Forwarding Case
         * It means, L3 has resolved the nexthop, So its 
         * time to L2 forward the pkt out of this interface*/
        oif = get_node_if_by_name(node, outgoing_intf);
        assert(oif);

        arp_entry = arp_table_lookup(NODE_ARP_TABLE(node), next_hop_ip_str);

        if (!arp_entry){
            printf("Unable to find mac address for %s\nConsider running arp", next_hop_ip_str);
            return;
        }
    } else {
        /* case 2 : Direct host Delivery
        L2 has to forward the frame to machine on local connected subnet */
        oif = node_get_matching_subnet_interface(node, next_hop_ip_str);
        if(!oif){
            printf(
                "%s : Error : Local matching subnet for IP : %s could not be found\n",
                node->node_name,
                next_hop_ip_str
            );
            return;
        }
        arp_entry = arp_table_lookup(NODE_ARP_TABLE(node), next_hop_ip_str);
    }

    memcpy(ethernet_hdr->dst_mac.mac, arp_entry->mac_addr.mac, sizeof(mac_add_t));
    memcpy(ethernet_hdr->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
    SET_COMMON_ETH_FCS(ethernet_hdr, ethernet_payload_size, 0);
    send_pkt_out((char *)ethernet_hdr, pkt_size, oif);
}

static void layer2_pkt_receieve_from_top(
    node_t *node, 
    unsigned int next_hop_ip,
    char *outgoing_intf,
    char *pkt,
    unsigned int pkt_size,
    int protocol_number) {

    assert(pkt_size < sizeof(((ethernet_hdr_t *)0)->payload));

    if(protocol_number == ETH_IP) {
        ethernet_hdr_t *empty_ethernet_hdr = ALLOC_ETH_HDR_WITH_PAYLOAD(pkt, pkt_size); 
        empty_ethernet_hdr->type = ETH_IP;
        l2_forward_ip_packet(
            node,
            next_hop_ip, 
            outgoing_intf,
            empty_ethernet_hdr,
            pkt_size + ETH_HDR_SIZE_EXCL_PAYLOAD
        );
    }
}

void demote_pkt_to_layer2(
    node_t *node, /*Currenot node*/ 
    unsigned int next_hop_ip,  /*If pkt is forwarded to next router, then this is Nexthop IP address (gateway) provided by L3 layer. L2 need to resolve ARP for this IP address*/
    char *outgoing_intf,       /*The oif obtained from L3 lookup if L3 has decided to forward the pkt. If NULL, then L2 will find the appropriate interface*/
    char *pkt,                  /*higher level payload*/
    unsigned int pkt_size,
    int protocol_number) {      /*Higher Layer need to tell L2 what value need to be feed in eth_hdr->type field*/

    layer2_pkt_receieve_from_top(
        node,
        next_hop_ip,
        outgoing_intf,
        pkt,
        pkt_size,
        protocol_number
    );
}











// VLAN STUFF

ethernet_hdr_t* tag_pkt_with_vlan_id(ethernet_hdr_t *ethernet_hdr, unsigned int total_pkt_size, int vlan_id, unsigned int* new_pkt_size) {
    unsigned int payload_size  = 0 ;
    *new_pkt_size = 0;

    /*If the pkt is already tagged, replace it*/
    vlan_8021q_hdr_t *vlan_8021q_hdr = is_pkt_vlan_tagged(ethernet_hdr);
    if(vlan_8021q_hdr){
        payload_size = total_pkt_size - VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD;
        vlan_8021q_hdr->tci_vid = (short)vlan_id;
        /*Update checksum, however not used*/
        SET_COMMON_ETH_FCS(ethernet_hdr, payload_size, 0);
        *new_pkt_size = total_pkt_size;
        return ethernet_hdr;
    }

    /*If the pkt is not already tagged, tag it*/
    ethernet_hdr_t ethernet_hdr_old;
    memcpy(
        (char *)&ethernet_hdr_old,
        (char *)ethernet_hdr, 
        ETH_HDR_SIZE_EXCL_PAYLOAD - sizeof(ethernet_hdr_old.FCS)
    );
    payload_size = total_pkt_size - ETH_HDR_SIZE_EXCL_PAYLOAD; 
    vlan_ethernet_hdr_t *vlan_ethernet_hdr = (vlan_ethernet_hdr_t *)((char *)ethernet_hdr - sizeof(vlan_8021q_hdr_t));
    memset(
        (char *)vlan_ethernet_hdr,
        0, 
        VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD - sizeof(vlan_ethernet_hdr->FCS)
    );
    memcpy(vlan_ethernet_hdr->dst_mac.mac, ethernet_hdr_old.dst_mac.mac, sizeof(mac_add_t));
    memcpy(vlan_ethernet_hdr->src_mac.mac, ethernet_hdr_old.src_mac.mac, sizeof(mac_add_t));

    /*Come to 802.1Q vlan hdr*/
    vlan_ethernet_hdr->vlan_8021q_hdr.tpid = VLAN_8021Q_PROTO;
    vlan_ethernet_hdr->vlan_8021q_hdr.tci_pcp = 0;
    vlan_ethernet_hdr->vlan_8021q_hdr.tci_dei = 0;
    vlan_ethernet_hdr->vlan_8021q_hdr.tci_vid = (short)vlan_id;

    /*Type field*/
    vlan_ethernet_hdr->type = ethernet_hdr_old.type;

    /*Update checksum, however not used*/
    SET_COMMON_ETH_FCS((ethernet_hdr_t *)vlan_ethernet_hdr, payload_size, 0 );
    *new_pkt_size = total_pkt_size  + sizeof(vlan_8021q_hdr_t);
    return (ethernet_hdr_t *)vlan_ethernet_hdr;
}

/* Return new packet size if pkt is untagged with the existing
 * vlan 801.1q hdr*/
ethernet_hdr_t* untag_pkt_with_vlan_id(ethernet_hdr_t *ethernet_hdr, unsigned int total_pkt_size, unsigned int *new_pkt_size) {
    *new_pkt_size = 0;

    vlan_8021q_hdr_t *vlan_8021q_hdr = is_pkt_vlan_tagged(ethernet_hdr);
    /*Not tagged already, do nothing*/    
    if(!vlan_8021q_hdr){
        *new_pkt_size = total_pkt_size;
        return ethernet_hdr;
    }

    vlan_ethernet_hdr_t vlan_ethernet_hdr_old;
    memcpy((char *)&vlan_ethernet_hdr_old, (char *)ethernet_hdr, 
                VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD - sizeof(vlan_ethernet_hdr_old.FCS));
    ethernet_hdr = (ethernet_hdr_t *)((char *)ethernet_hdr + sizeof(vlan_8021q_hdr_t));
    memcpy(ethernet_hdr->dst_mac.mac, vlan_ethernet_hdr_old.dst_mac.mac, sizeof(mac_add_t));
    memcpy(ethernet_hdr->src_mac.mac, vlan_ethernet_hdr_old.src_mac.mac, sizeof(mac_add_t));
    ethernet_hdr->type = vlan_ethernet_hdr_old.type;
    
    /*No need to copy data*/
    unsigned int payload_size = total_pkt_size - VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD;
    /*Update checksum, however not used*/
    SET_COMMON_ETH_FCS(ethernet_hdr, payload_size, 0);
    
    *new_pkt_size = total_pkt_size - sizeof(vlan_8021q_hdr_t);
    return ethernet_hdr;
}

void interface_set_vlan(node_t *node, interface_t *interface, unsigned int vlan_id) {
    /* Case 1 : Cant set vlans on interface configured with ip address*/
    if(IS_INTF_L3_MODE(interface)){
        printf("Error : Interface %s : L3 mode enabled\n", interface->if_name);
        return;
    }
    /*Case 2 : Cant set vlan on interface not operating in L2 mode*/
    if(IF_L2_MODE(interface) != ACCESS && IF_L2_MODE(interface) != TRUNK){
        printf("Error : Interface %s : L2 mode not Enabled\n", interface->if_name);
        return;
    }
    /*case 3 : Can set only one vlan on interface operating in ACCESS mode*/
    if(interface->intf_nw_props.intf_l2_mode == ACCESS){
        interface->intf_nw_props.vlans[0] = vlan_id;
    }
    /*case 4 : Add vlan membership on interface operating in TRUNK mode*/
    if(interface->intf_nw_props.intf_l2_mode == TRUNK){
        unsigned int* vlan = NULL;
        for(int i = 0 ; i < MAX_VLAN_MEMBERSHIP; i++) {
            if(!vlan && interface->intf_nw_props.vlans[i] == 0) {
                vlan = &interface->intf_nw_props.vlans[i];
            } else if(interface->intf_nw_props.vlans[i] == vlan_id){
                return;
            }
        }
        if(vlan){
            *vlan = vlan_id;
            return;
        }
        printf("Error : Interface %s : Max Vlan membership limit reached", interface->if_name);
    }
}

void node_set_intf_vlan_membsership(node_t *node, char *intf_name, unsigned int vlan_id) {
    interface_t *interface = get_node_if_by_name(node, intf_name);
    assert(interface);
    interface_set_vlan(node, interface, vlan_id);
}

















// ARP STUFF

void init_arp_table(arp_table_t **arp_table) {
    *arp_table = calloc(1, sizeof(arp_table_t));
    init_glthread(&((*arp_table)->arp_entries));
}

// get arp entry from an ip
arp_entry_t* arp_table_lookup(arp_table_t *arp_table, char *ip_addr) {

    glthread_t *curr;
    arp_entry_t *arp_entry;
    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){
    
        arp_entry = arp_glue_to_arp_entry(curr);
        if(strncmp(arp_entry->ip_addr.ip_addr, ip_addr, 16) == 0){
            return arp_entry;
        }
    } ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr);
    return NULL;
}

void delete_arp_entry(arp_entry_t *arp_entry) {
    remove_glthread(&arp_entry->arp_glue);
    free(arp_entry);
}

void delete_arp_table_entry(arp_table_t *arp_table, char *ip_addr) {
    arp_entry_t *arp_entry = arp_table_lookup(arp_table, ip_addr);
    if(!arp_entry)
        return;
    delete_arp_entry(arp_entry);
}


bool arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry) {
    arp_entry_t *arp_entry_old = arp_table_lookup(arp_table, arp_entry->ip_addr.ip_addr);

    /* Case 0 : if ARP table do not exist already, then add it
     * and return true*/
    if(!arp_entry_old){
        glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);
        return true;
    }
    
    /*Case 1 : If existing and new ARP entries are full and equal, then
     * do nothing*/
    if(arp_entry_old && memcmp(arp_entry_old, arp_entry, sizeof(arp_entry_t))==0) {
            // IS_ARP_ENTRIES_EQUAL(arp_entry_old, arp_entry)){

        return false;
    }

    /*Case 2 : If there already exists full ARP table entry, then replace it*/
    if(arp_entry_old) { // && !arp_entry_sane(arp_entry_old)){
        delete_arp_entry(arp_entry_old);
        init_glthread(&arp_entry->arp_glue);
        glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);
        return true;
    }
    return false;
}


void arp_table_update_from_arp_reply(arp_table_t *arp_table, arp_hdr_t *arp_hdr, interface_t *iif) {
    unsigned int src_ip = 0;
    arp_entry_t *arp_entry = calloc(1, sizeof(arp_entry_t));
    src_ip = htonl(arp_hdr->src_ip);
    inet_ntop(AF_INET, &src_ip, arp_entry->ip_addr.ip_addr, 16);
    arp_entry->ip_addr.ip_addr[15] = '\0';
    memcpy(arp_entry->mac_addr.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));
    strncpy(arp_entry->oif_name, iif->if_name, IF_NAME_SIZE);

    bool rc = arp_table_entry_add(arp_table, arp_entry);
    if(rc == false){
        free(arp_entry);
    }
}

void dump_arp_table(arp_table_t *arp_table)  {

    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){

        arp_entry = arp_glue_to_arp_entry(curr);
        printf("IP : %s, MAC : %u:%u:%u:%u:%u:%u, OIF = %s\n", 
            arp_entry->ip_addr.ip_addr, 
            arp_entry->mac_addr.mac[0], 
            arp_entry->mac_addr.mac[1], 
            arp_entry->mac_addr.mac[2], 
            arp_entry->mac_addr.mac[3], 
            arp_entry->mac_addr.mac[4], 
            arp_entry->mac_addr.mac[5], 
            arp_entry->oif_name);
            // arp_entry_sane(arp_entry) ? "TRUE" : "FALSE");
    } ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr);
}

void send_arp_broadcast_request(node_t *node, interface_t *oif, char *ip_addr){
    /*Take memory which can accomodate Ethernet hdr + ARP hdr*/
    unsigned int payload_size = sizeof(arp_hdr_t);
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)calloc(1, ETH_HDR_SIZE_EXCL_PAYLOAD + payload_size);

    if(!oif){
        oif = node_get_matching_subnet_interface(node, ip_addr);
        if(!oif){
            printf("Error : %s : No eligible subnet for ARP resolution for Ip-address : %s",
                    node->node_name, ip_addr);
            return;
        }
        if(strncmp(IF_IP(oif), ip_addr, 16) == 0){
            printf("Error : %s : Attemp to resolve ARP for local Ip-address : %s",
                    node->node_name, ip_addr);
            return;
        }
    }
    /*STEP 1 : Prepare ethernet hdr*/
    layer2_fill_with_broadcast_mac(ethernet_hdr->dst_mac.mac);
    memcpy(ethernet_hdr->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
    ethernet_hdr->type = ARP_MSG;

    /*Step 2 : Prepare ARP Broadcast Request Msg out of oif*/
    arp_hdr_t *arp_hdr = (arp_hdr_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr));
    arp_hdr->hw_type = 1;
    arp_hdr->proto_type = 0x0800;
    arp_hdr->hw_addr_len = sizeof(mac_add_t);
    arp_hdr->proto_addr_len = 4;

    arp_hdr->op_code = ARP_BROAD_REQ;

    memcpy(arp_hdr->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));

    inet_pton(AF_INET, IF_IP(oif), &arp_hdr->src_ip);
    arp_hdr->src_ip = htonl(arp_hdr->src_ip);

    memset(arp_hdr->dst_mac.mac, 0,  sizeof(mac_add_t));

    inet_pton(AF_INET, ip_addr, &arp_hdr->dst_ip);
    arp_hdr->dst_ip = htonl(arp_hdr->dst_ip);

    SET_COMMON_ETH_FCS(ethernet_hdr, sizeof(arp_hdr_t), 0); /*Not used*/

    /*STEP 3 : Now dispatch the ARP Broadcast Request Packet out of interface*/
    send_pkt_out((char *)ethernet_hdr, ETH_HDR_SIZE_EXCL_PAYLOAD + payload_size, oif);
    free(ethernet_hdr);
}

static void send_arp_reply_msg(ethernet_hdr_t *ethernet_hdr_in, interface_t *oif) {

    arp_hdr_t *arp_hdr_in = (arp_hdr_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr_in));

    ethernet_hdr_t *ethernet_hdr_reply = (ethernet_hdr_t *)calloc(1, MAX_PACKET_BUFFER_SIZE);

    memcpy(ethernet_hdr_reply->dst_mac.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    memcpy(ethernet_hdr_reply->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
    
    ethernet_hdr_reply->type = ARP_MSG;
    
    arp_hdr_t *arp_hdr_reply = (arp_hdr_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr_reply));
    
    arp_hdr_reply->hw_type = 1;
    arp_hdr_reply->proto_type = 0x0800;
    arp_hdr_reply->hw_addr_len = sizeof(mac_add_t);
    arp_hdr_reply->proto_addr_len = 4;
    
    arp_hdr_reply->op_code = ARP_REPLY;
    memcpy(arp_hdr_reply->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));

    inet_pton(AF_INET, IF_IP(oif), &arp_hdr_reply->src_ip);
    arp_hdr_reply->src_ip =  htonl(arp_hdr_reply->src_ip);

    memcpy(arp_hdr_reply->dst_mac.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    arp_hdr_reply->dst_ip = arp_hdr_in->src_ip;
  
    SET_COMMON_ETH_FCS(ethernet_hdr_reply, sizeof(arp_hdr_t), 0); /*Not used*/

    unsigned int total_pkt_size = ETH_HDR_SIZE_EXCL_PAYLOAD + sizeof(arp_hdr_t);

    char *shifted_pkt_buffer = pkt_buffer_shift_right((char *)ethernet_hdr_reply, 
                               total_pkt_size, MAX_PACKET_BUFFER_SIZE);

    send_pkt_out(shifted_pkt_buffer, total_pkt_size, oif);

    free(ethernet_hdr_reply);  
}

static void process_arp_reply_msg(node_t *node, interface_t *iif,
                        ethernet_hdr_t *ethernet_hdr){

    printf("%s : ARP reply msg recvd on interface %s of node %s\n",
             __FUNCTION__, iif->if_name , iif->att_node->node_name);

    arp_table_update_from_arp_reply( NODE_ARP_TABLE(node), 
                    (arp_hdr_t *)GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr), iif);    
}


static void process_arp_broadcast_request(node_t *node, interface_t *iif, 
                              ethernet_hdr_t *ethernet_hdr) {

   printf("%s : ARP Broadcast msg recvd on interface %s of node %s\n", 
                __FUNCTION__, iif->if_name , iif->att_node->node_name); 

   /* ARP broadcast request msg has passed MAC Address check*/
   /* Now, this node need to reply to this ARP Broadcast req
    * msg if Dst ip address in ARP req msg matches iif's ip address*/

    char ip_addr[16];
    arp_hdr_t *arp_hdr = (arp_hdr_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr));

    unsigned int arp_dst_ip = htonl(arp_hdr->dst_ip);

    inet_ntop(AF_INET, &arp_dst_ip, ip_addr, 16);
    ip_addr[15] = '\0';
    
    if(strncmp(IF_IP(iif), ip_addr, 16)){
        
        printf("%s : ARP Broadcast req msg dropped, Dst IP address %s did not match with interface ip : %s\n", 
                node->node_name, ip_addr , IF_IP(iif));
        return;
    }

   send_arp_reply_msg(ethernet_hdr, iif);
}
#include <stdio.h>
#include "layer2.h"
#include <stdlib.h>
#include "../gluethread/glthread.h"
/*L2 Switch Owns Mac Table*/

typedef struct mac_table_{
    glthread_t mac_entries;
} mac_table_t;

typedef struct mac_table_entry_{
    mac_add_t mac;
    char oif_name[IF_NAME_SIZE];
    glthread_t mac_entry_glue;
} mac_table_entry_t;
GLTHREAD_TO_STRUCT(mac_entry_glue_to_mac_entry, mac_table_entry_t, mac_entry_glue);



#define IS_MAC_TABLE_ENTRY_EQUAL(mac_entry_1, mac_entry_2)   \
    (strncmp(mac_entry_1->mac.mac, mac_entry_2->mac.mac, sizeof(mac_add_t)) == 0 && \
            strncmp(mac_entry_1->oif_name, mac_entry_2->oif_name, IF_NAME_SIZE) == 0)


void init_mac_table(mac_table_t **mac_table){
    *mac_table = calloc(1, sizeof(mac_table_t));
    init_glthread(&((*mac_table)->mac_entries));
}

mac_table_entry_t* mac_table_lookup(mac_table_t *mac_table, char *mac) {
    glthread_t *curr;
    mac_table_entry_t *mac_table_entry;
    ITERATE_GLTHREAD_BEGIN(&mac_table->mac_entries, curr){
        mac_table_entry = mac_entry_glue_to_mac_entry(curr);
        if(strncmp(mac_table_entry->mac.mac, mac, sizeof(mac_add_t)) == 0){
            return mac_table_entry;
        }
    } ITERATE_GLTHREAD_END(&mac_table->mac_entries, curr);
    return NULL;
}

void delete_mac_table_entry(mac_table_t *mac_table, char *mac) {
    mac_table_entry_t *mac_table_entry = mac_table_lookup(mac_table, mac);
    if(!mac_table_entry)
        return;
    remove_glthread(&mac_table_entry->mac_entry_glue);
    free(mac_table_entry);
}

bool mac_table_entry_add(mac_table_t *mac_table, mac_table_entry_t *mac_table_entry) {
    mac_table_entry_t *mac_table_entry_old = mac_table_lookup(
        mac_table,
        mac_table_entry->mac.mac
    );
    if(mac_table_entry_old && IS_MAC_TABLE_ENTRY_EQUAL(mac_table_entry_old, mac_table_entry)){
        return false;
    }
    if(mac_table_entry_old){
        delete_mac_table_entry(mac_table, mac_table_entry_old->mac.mac);
    }
    init_glthread(&mac_table_entry->mac_entry_glue);
    glthread_add_next(&mac_table->mac_entries, &mac_table_entry->mac_entry_glue);
    return true;
}

void dump_mac_table(mac_table_t *mac_table) {
    glthread_t *curr;
    mac_table_entry_t *mac_table_entry;
    ITERATE_GLTHREAD_BEGIN(&mac_table->mac_entries, curr){
        mac_table_entry = mac_entry_glue_to_mac_entry(curr);
        printf("\tMAC : %u:%u:%u:%u:%u:%u   | Intf : %s\n", 
            mac_table_entry->mac.mac[0], 
            mac_table_entry->mac.mac[1],
            mac_table_entry->mac.mac[2],
            mac_table_entry->mac.mac[3], 
            mac_table_entry->mac.mac[4],
            mac_table_entry->mac.mac[5],
            mac_table_entry->oif_name);
    } ITERATE_GLTHREAD_END(&mac_table->mac_entries, curr);
}

void l2_switch_perform_mac_learning(node_t* node, char* src_mac, char* if_name) {
    mac_table_entry_t* mac_table_entry = (mac_table_entry_t*) malloc(sizeof(mac_table_entry_t));
    memcpy(mac_table_entry->mac.mac, src_mac, sizeof(mac_add_t));
    strncpy(mac_table_entry->oif_name, if_name, IF_NAME_SIZE);
    mac_table_entry->oif_name[IF_NAME_SIZE - 1] = '\0';
    bool rc = mac_table_entry_add(node->node_nw_prop.mac_table, mac_table_entry);
    if(!rc) {
        free(mac_table_entry);
    }
}


static void l2_switch_send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *oif){
    /*L2 switch must not even try to send the pkt out of interface 
      operating in L3 mode*/
    assert(!IS_INTF_L3_MODE(oif));
    send_pkt_out(pkt, pkt_size, oif);
}

static void l2_switch_flood_pkt_out(node_t *node, interface_t *exempted_intf, char *pkt, unsigned int pkt_size){
    interface_t *oif = NULL;
    char *pkt_copy = NULL;
    char *temp_pkt = calloc(1, MAX_PACKET_BUFFER_SIZE);
    pkt_copy = temp_pkt + MAX_PACKET_BUFFER_SIZE - pkt_size;
    for(int i=0 ; i < MAX_INTF_PER_NODE; i++){
        oif = node->intf[i];
        if(!oif)
            break;
        if(oif == exempted_intf || IS_INTF_L3_MODE(oif))
            continue;
        memcpy(pkt_copy, pkt, pkt_size);
        l2_switch_send_pkt_out(pkt_copy, pkt_size, oif);
    }
    free(temp_pkt);
}

static void l2_switch_forward_frame(node_t *node, interface_t *recv_intf, ethernet_hdr_t *ethernet_hdr, unsigned int pkt_size){
    /*If dst mac is broadcast mac, then flood the frame*/
    if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac.mac)){
        l2_switch_flood_pkt_out(node, recv_intf, (char *)ethernet_hdr, pkt_size);
        return;
    }

    /*Check the mac table to forward the frame*/
    mac_table_entry_t *mac_table_entry = 
        mac_table_lookup(NODE_MAC_TABLE(node), ethernet_hdr->dst_mac.mac);
    if(!mac_table_entry){
        printf("flood packet\n");
        l2_switch_flood_pkt_out(node, recv_intf, (char *)ethernet_hdr, pkt_size);
        return;
    }
    printf("use mac table to send packet\n");
    char *oif_name = mac_table_entry->oif_name;
    interface_t *oif = get_node_if_by_name(node, oif_name);
    if(!oif){
        return;
    }
    l2_switch_send_pkt_out((char *)ethernet_hdr, pkt_size, oif);
}


void l2_switch_recv_frame(interface_t *interface, char *pkt, unsigned int pkt_size){
    node_t *node = interface->att_node;
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt;
    char *dst_mac = (char *)ethernet_hdr->dst_mac.mac;
    char *src_mac = (char *)ethernet_hdr->src_mac.mac;

    l2_switch_perform_mac_learning(node, src_mac, interface->if_name);
    l2_switch_forward_frame(node, interface, ethernet_hdr, pkt_size);
}

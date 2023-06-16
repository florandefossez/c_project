# include "ether.h"

extern int tapdev;
extern const u_int8_t MYMAC[6];
extern const char MYIP[16];

void print_layer2(eth_hdr_t* eth_hdr) {
    char type_name[10];
    u_int16_t type = ntohs(eth_hdr->ethertype);
    switch (type) {
    case IPv4:
        strcpy(type_name, "IPv4");
        break;
    
    case IPv6:
        strcpy(type_name, "IPv6");
        break;
    
    case ARP:
        strcpy(type_name, "ARP");
        break;
    
    default:
        sprintf(type_name, "0x%x", type);
        break;
    }

    printf(
        "source mac: %x:%x:%x:%x:%x:%x | dest: %x:%x:%x:%x:%x:%x | ether type: %s\n",
        eth_hdr->smac[0],
        eth_hdr->smac[1],
        eth_hdr->smac[2],
        eth_hdr->smac[3],
        eth_hdr->smac[4],
        eth_hdr->smac[5],

        eth_hdr->dmac[0],
        eth_hdr->dmac[1],
        eth_hdr->dmac[2],
        eth_hdr->dmac[3],
        eth_hdr->dmac[4],
        eth_hdr->dmac[5],

        type_name
    );
}

void parse_eth(char* packet, ssize_t packet_size) {
    eth_hdr_t *eth_hdr = (eth_hdr_t*) packet;
    print_layer2(eth_hdr);
    // u_int16_t type = ntohs(eth_hdr->ethertype);
    static const u_int8_t BROADCAST[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (memcmp(eth_hdr->dmac, BROADCAST, 6) != 0) {
        printf("Incorrect destination MAC - Drop the packet\n");
        return;
    }

    switch (ntohs(eth_hdr->ethertype)) {
    // case IPv4:

    //     break;
    
    case ARP:
        print_arp((arp_hdr_t *) eth_hdr->payload);
        arp_handler((arp_hdr_t *) eth_hdr->payload);
        break;
    
    default:
        printf("protocol not supported yet - Drop the packet\n");
        break;
    }
}

void arp_handler(arp_hdr_t* arp_hdr) {

    eth_hdr_t* reply = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
    memcpy(reply->smac, MYMAC, 6);
    memcpy(reply->dmac, arp_hdr->smac, 6);
    reply->ethertype = htons(ARP);

    arp_hdr_t* arp_reply = (arp_hdr_t*) reply->payload;
    memcpy(arp_reply->dmac, arp_hdr->smac, 6);
    arp_reply->dip = arp_hdr->sip;
    memcpy(arp_reply->smac, MYMAC, 6);
    inet_pton(AF_INET, MYIP, &arp_reply->sip);
    arp_reply->hw_type = htons(1);
    arp_reply->hwsize = 6;
    arp_reply->prosize = 4;
    arp_reply->op_code = htons(2);
    arp_reply->proto_type = htons(IPv4);

    printf("ARP reply: ");
    print_layer2(reply);
    print_arp(arp_reply);
    
    write(tapdev, reply, sizeof(eth_hdr_t) - sizeof(((eth_hdr_t*)0)->payload) + sizeof(arp_hdr_t));

    free(reply);
    }

void print_arp(arp_hdr_t* arp_hdr) {
    u_int16_t type = ntohs(arp_hdr->proto_type);
    if (type != IPv4) {
        printf("ARP 0x%x protocol is not supported - Drop the packet\n", type);
        return;
    }
    char psip[16];
    char pdip[16];
    inet_ntop(AF_INET, &arp_hdr->sip, psip, 16);
    inet_ntop(AF_INET, &arp_hdr->dip, pdip, 16);
    printf(
        "\ttype: 0x%x | hwsize: %d | prosize: %d | op: %d | smac: %x:%x:%x:%x:%x:%x | sip: %s | dmac: %x:%x:%x:%x:%x:%x | dip: %s\n",
        type,
        arp_hdr->hwsize,
        arp_hdr->prosize,
        ntohs(arp_hdr->op_code),
        arp_hdr->smac[0],
        arp_hdr->smac[1],
        arp_hdr->smac[2],
        arp_hdr->smac[3],
        arp_hdr->smac[4],
        arp_hdr->smac[5],
        psip,
        arp_hdr->dmac[0],
        arp_hdr->dmac[1],
        arp_hdr->dmac[2],
        arp_hdr->dmac[3],
        arp_hdr->dmac[4],
        arp_hdr->dmac[5],
        pdip     
    );
}

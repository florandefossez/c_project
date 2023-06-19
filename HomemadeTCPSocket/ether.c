#include "ether.h"
#include "ip4.h"

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
        "[ETH] source mac: %x:%x:%x:%x:%x:%x | dest: %x:%x:%x:%x:%x:%x | ether type: %s\n",
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
    eth_hdr_t* reply_eth_hdr;
    print_layer2(eth_hdr);
    static const u_int8_t BROADCAST[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (memcmp(eth_hdr->dmac, BROADCAST, 6) != 0 && memcmp(eth_hdr->dmac, MYMAC, 6) != 0) {
        printf("\tIncorrect destination MAC - Drop the packet\n");
        return;
    }

    switch (ntohs(eth_hdr->ethertype)) {
    case IPv4:
        print_ip4((ip4_hdr_t *) (packet + sizeof(eth_hdr_t)));
        reply_eth_hdr = ip4_handler((ip4_hdr_t *) (packet + sizeof(eth_hdr_t)));
        break;
    
    case ARP:
        print_arp((arp_hdr_t *) (packet + sizeof(eth_hdr_t)));
        arp_handler((arp_hdr_t *) (packet + sizeof(eth_hdr_t)));
        break;
    
    default:
        printf("\tProtocol not supported - Drop the packet\n");
        break;
    }
    if (reply_eth_hdr) {
        reply_eth_hdr->ethertype = eth_hdr->ethertype;
        memcpy(reply_eth_hdr->smac, MYMAC, 6);
        memcpy(reply_eth_hdr->dmac, eth_hdr->smac, 6);
        
        printf("SEND %lu bytes\n", sizeof(eth_hdr_t) + ntohs(((ip4_hdr_t *) (((char*) reply_eth_hdr) + sizeof(eth_hdr_t)))->len));
        write(tapdev, (char *) reply_eth_hdr, sizeof(eth_hdr_t) + ntohs(((ip4_hdr_t *) (((char*) reply_eth_hdr) + sizeof(eth_hdr_t)))->len));
        
    }
}


void arp_handler(arp_hdr_t* arp_hdr) {
    u_int16_t type = ntohs(arp_hdr->proto_type);
    if (type != IPv4) {
        printf("\tARP 0x%x protocol is not supported - Drop the packet\n", type);
        return;
    }
    char* reply_packet = (char *) malloc(MAX_PACKET_SIZE);
    eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) reply_packet;
    memcpy(reply_eth_hdr->smac, MYMAC, 6);
    memcpy(reply_eth_hdr->dmac, arp_hdr->smac, 6);
    reply_eth_hdr->ethertype = htons(ARP);

    arp_hdr_t* reply_arp = (arp_hdr_t*) (reply_packet + sizeof(eth_hdr_t));
    memcpy(reply_arp->dmac, arp_hdr->smac, 6);
    reply_arp->dip = arp_hdr->sip;
    memcpy(reply_arp->smac, MYMAC, 6);
    inet_pton(AF_INET, MYIP, &reply_arp->sip);
    reply_arp->hw_type = htons(1);
    reply_arp->hwsize = 6;
    reply_arp->prosize = 4;
    reply_arp->op_code = htons(2);
    reply_arp->proto_type = htons(IPv4);

    printf("[ARP reply]\n");
    printf("\t");
    print_layer2(reply_eth_hdr);
    printf("\t");
    print_arp(reply_arp);
    
    printf("SEND %lu bytes\n", sizeof(eth_hdr_t) + sizeof(arp_hdr_t));
    write(tapdev, reply_packet, sizeof(eth_hdr_t) + sizeof(arp_hdr_t));

    free(reply_packet);
    }

void print_arp(arp_hdr_t* arp_hdr) {
    u_int16_t type = ntohs(arp_hdr->proto_type);
    char psip[16];
    char pdip[16];
    inet_ntop(AF_INET, &arp_hdr->sip, psip, 16);
    inet_ntop(AF_INET, &arp_hdr->dip, pdip, 16);
    printf(
        "[ARP] type: 0x%x | hwsize: %d | prosize: %d | op: %d | smac: %x:%x:%x:%x:%x:%x | sip: %s | dmac: %x:%x:%x:%x:%x:%x | dip: %s\n",
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

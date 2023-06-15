# include "ether.h"

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
        return;
    }

void print_arp(arp_hdr_t* arp_hdr) {
    u_int16_t type = ntohs(arp_hdr->proto_type);
    if (type != IPv4) {
        printf("ARP 0x%x protocol is not supported - Drop the packet\n", type);
        return;
    }
    char psip[16];
    inet_ntop(AF_INET, &arp_hdr->sip, psip, 16);
    printf(
        "\ttype: 0x%x | hwsize: %d | prosize: %d | op: %d | smac: %x:%x:%x:%x:%x:%x | sip: %s\n",
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
        psip        
    );
}

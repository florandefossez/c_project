#include "ip4.h"

extern int tapdev;
extern const u_int8_t MYMAC[6];
extern const char MYIP[16];


void ip4_handler(ip4_hdr_t* ip4_hdr) {
    u_int32_t myip;
    inet_pton(AF_INET, MYIP, &myip);
    if (memcmp(&ip4_hdr->dip, &myip, 4) != 0) {
        printf("Invalid destination IP address - Drop the packet\n");
        return;
    }
    switch (ip4_hdr->proto) {
    case TCP:
        printf("Not supported yet\n");
        break;
    
    case UDP:
        printf("Not supported yet\n");
        break;
    
    case ICMP:
        printf("Not supported yet\n");
        break;
    
    default:
        printf("Protocol supported\n");
        break;
    }
}



void print_ip4(ip4_hdr_t* ip4_hdr) {
    char psip[16];
    char pdip[16];
    inet_ntop(AF_INET, &ip4_hdr->sip, psip, 16);
    inet_ntop(AF_INET, &ip4_hdr->dip, pdip, 16);
    u_int16_t flag_frag_offset = ntohs(ip4_hdr->flags_frag_offset);

    char proto[5];
    switch (ip4_hdr->proto) {
    case UDP:
        strcpy(proto, "UDP");
        break;
    case TCP:
        strcpy(proto, "TCP");
        break;
    case ICMP:
        strcpy(proto, "ICMP");
        break;
    default:
        sprintf(proto, "0x%x", ip4_hdr->proto);
        break;
    }

    printf(
        "\tv: %u | ihl: %u | tos: %x | len: %u | frag id: %u | flags: %u%u%u | offset: %u\n\tttl: %u | proto: %s | csum: %X | sip: %s | dip: %s\n",
        ip4_hdr->version_ihl >> 4,
        ip4_hdr->version_ihl & 0x0F,
        ip4_hdr->tos,
        ntohs(ip4_hdr->len),

        ntohs(ip4_hdr->frag_id),
        (flag_frag_offset >> 15) & 0x01,
        (flag_frag_offset >> 14) & 0x01,
        (flag_frag_offset >> 13) & 0x01,
        flag_frag_offset & 0x1FFF,

        ip4_hdr->ttl,
        proto,
        ntohs(ip4_hdr->csum),

        psip,
        pdip
    );
}


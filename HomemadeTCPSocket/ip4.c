#include "ip4.h"
#include "icmp.h"
#include "utils.h"
#include "tcp.h"

extern int tapdev;
extern const u_int8_t MYMAC[6];
extern const char MYIP[16];



eth_hdr_t* ip4_handler(ip4_hdr_t* ip4_hdr) {
    eth_hdr_t* reply_eth_hdr;
    u_int32_t myip;
    ip4_pseudo_hdr_t pseudo_hdr;
    reply_eth_hdr = NULL;

    inet_pton(AF_INET, MYIP, &myip);
    if (memcmp(&ip4_hdr->dip, &myip, 4) != 0) {
        printf("Invalid destination IP address - Drop the packet\n");
        return NULL;
    }
    switch (ip4_hdr->proto) {
    case TCP:
        // ip4_pseudo_hdr_t* pseudo_hdr = (ip4_pseudo_hdr_t *) calloc(1, sizeof(ip4_pseudo_hdr_t));
        pseudo_hdr.sip = ip4_hdr->sip;
        pseudo_hdr.dip = ip4_hdr->dip;
        pseudo_hdr.proto = ip4_hdr->proto;
        pseudo_hdr.tcp_len = htons(ntohs(ip4_hdr->len) - 4*(ip4_hdr->version_ihl & 0x0F));
        pseudo_hdr.fixed = 0;
        print_tcp(
            (tcp_hdr_t *) (((char *) ip4_hdr) + 4*(ip4_hdr->version_ihl & 0x0F)),
            &pseudo_hdr
        );
        break;
    
    case UDP:
        printf("Not supported yet\n");
        break;
    
    case ICMP:
        print_icmp(
            (icmp_hdr_t *) (((char *) ip4_hdr) + 4*(ip4_hdr->version_ihl & 0x0F)),
            htons(ip4_hdr->len) - 4*(ip4_hdr->version_ihl & 0x0F)
        );
        reply_eth_hdr = icmp_handler(
            (icmp_hdr_t *) (((char *) ip4_hdr) + 4*(ip4_hdr->version_ihl & 0x0F)),
            htons(ip4_hdr->len) - 4*(ip4_hdr->version_ihl & 0x0F),
            ip4_hdr->sip
        );
        break;
    
    default:
        printf("Protocol not supported\n");
        break;
    }

    if (reply_eth_hdr) {
        ip4_hdr_t* reply_ip4_hdr = (ip4_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t));
        reply_ip4_hdr->version_ihl = 0x45;
        reply_ip4_hdr->tos = 0;
        reply_ip4_hdr->frag_id = 0;
        reply_ip4_hdr->flags_frag_offset = 0x0040;
        reply_ip4_hdr->ttl = 64;
        reply_ip4_hdr->proto = ip4_hdr->proto;
        reply_ip4_hdr->csum = 0;
        reply_ip4_hdr->sip = myip;
        reply_ip4_hdr->dip = ip4_hdr->sip;
        reply_ip4_hdr->csum = checksum((char *) reply_ip4_hdr, 4*(ip4_hdr->version_ihl & 0x0F));
        return reply_eth_hdr;
    }
    return NULL;
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
        "[IPv4] v: %u | ihl: %u | tos: 0x%04X | len: %u | frag id: %u | flags: %u%u%u | offset: %u\n\tttl: %u | proto: %s | csum: 0x%04X | sip: %s | dip: %s\n",
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

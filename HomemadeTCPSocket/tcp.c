#include "tcp.h"
#include "utils.h"


void print_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr) {
    printf(
        "[TCP] sport: %u | dport: %u | seq: %u | ack: %u | hlen: %u | flags: [%x] | win_size: %u | csum: 0x%04X\n",
        ntohs(tcp_hdr->sport),
        ntohs(tcp_hdr->dport),
        ntohl(tcp_hdr->seq),
        ntohl(tcp_hdr->ack),
        tcp_hdr->hlen_reserved >> 4,
        tcp_hdr->flags,
        ntohs(tcp_hdr->window_size),
        tcp_hdr->csum
    );

    tcp_hdr->csum = 0;

    uint16_t* ptr = (uint16_t *) ip4_pseudo_hdr;
    uint32_t sum = 0;
    for(int i=0; i<6; i++) {
        sum += * ptr++;
    }
    int count = ntohs(ip4_pseudo_hdr->tcp_len);
    ptr = (uint16_t *) tcp_hdr;
    while( count > 1 ) {
        sum += * ptr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += *(uint8_t *) ptr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    sum = ~sum;
    
    printf("comp: %x\n\n", (u_int16_t) sum);
}


eth_hdr_t* handle_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr) {
    
}

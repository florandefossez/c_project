#include "tcp.h"
#include "utils.h"
#include "ip4.h"

sockets_t* sockets = NULL;


void print_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr) {
    char flags[17] = "\0";
    unsigned int i = 0;
    if ((tcp_hdr->flags >> 4) & 0x1) {
        strcpy(flags + i, "ACK ");
        i += 4;
    }
    if ((tcp_hdr->flags >> 2) & 0x1) {
        strcpy(flags + i, "RST ");
        i += 4;
    }
    if ((tcp_hdr->flags >> 1) & 0x1) {
        strcpy(flags + i, "SYN ");
        i += 4;
    }
    if (tcp_hdr->flags & 0x1) {
        strcpy(flags + i, "FIN ");
        i += 4;
    }
    if (i>0)
        flags[i-1] = '\0';

    printf(
        "[TCP] sport: %u | dport: %u | seq: %u | ack: %u | hlen: %u | flags: [%s] | win_size: %u | csum: 0x%04X\n",
        ntohs(tcp_hdr->sport),
        ntohs(tcp_hdr->dport),
        ntohl(tcp_hdr->seq),
        ntohl(tcp_hdr->ack),
        tcp_hdr->hlen_reserved >> 4,
        flags,
        ntohs(tcp_hdr->window_size),
        tcp_hdr->csum
    );

    tcp_hdr->csum = 0;
    
}


eth_hdr_t* handle_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr) {
    ip4_pseudo_hdr_t reply_pseudo_hdr;
    if (ntohs(tcp_hdr->dport) != 1234) {
        printf("TCP wrong port - Drop the packet\n");
        return NULL;
    }
    sockets_t* s;
    s = sockets;
    while (s) {
        if (sockets->remote_ip == ip4_pseudo_hdr->sip && sockets->remote_port == ntohs(tcp_hdr->sport) ) {
            printf("TCP packet is part of an active socket\n");
            break;
        }
        s = s->next;
    }
    if (s) {
        switch (s->state) {
            case SYN_RCVD:
                if (ntohl(tcp_hdr->ack) != s->seq + 1) {
                    printf("Error in ack number ! - Drop the packet\n");
                    break;
                }
                s->state = ESTABLISHED;
                s->seq += 1;
                break;
            
            case ESTABLISHED:
                if (s->seq != ntohl(tcp_hdr->ack)) {
                    printf("Error in ack number ! - Drop the packet.\n");
                    break;
                }
                char* payload = (char*) tcp_hdr + 4*(tcp_hdr->hlen_reserved >> 4);
                int payload_size = ntohs(ip4_pseudo_hdr->tcp_len) - 4*(tcp_hdr->hlen_reserved >> 4);
                if (payload_size == 0) {
                    printf("Empty TCP ack packet\n");
                    return NULL;
                }
                eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
                tcp_hdr_t* reply_tcp_hdr = (tcp_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t) + sizeof(ip4_hdr_t));
                payload[payload_size] = '\0';
                printf("MESSAGE: %s\n", payload);
                s->ack += payload_size;

                reply_tcp_hdr->sport = htons(1234);
                reply_tcp_hdr->dport = tcp_hdr->sport;
                reply_tcp_hdr->seq = htonl(s->seq);
                s->seq += payload_size;
                reply_tcp_hdr->ack = htonl(s->ack);
                reply_tcp_hdr->hlen_reserved = 0x50;
                reply_tcp_hdr->urg_ptr = 0;
                reply_tcp_hdr->window_size = 0xF0FA; //64240
                reply_tcp_hdr->flags = 0x10; // ACK
                reply_tcp_hdr->csum = 0;
                memcpy(((char*) reply_tcp_hdr) + 20, payload, payload_size);

                reply_pseudo_hdr.dip = ip4_pseudo_hdr->sip;
                reply_pseudo_hdr.proto = TCP;
                reply_pseudo_hdr.sip = ip4_pseudo_hdr->dip;
                reply_pseudo_hdr.tcp_len = htons(20 + payload_size);
                reply_pseudo_hdr.fixed = 0;

                reply_tcp_hdr->csum = tcp_ckecksum(reply_tcp_hdr, &reply_pseudo_hdr);

                ip4_hdr_t* reply_ip4_hdr = (ip4_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t));
                reply_ip4_hdr->len = htons(sizeof(ip4_hdr_t) + sizeof(tcp_hdr_t) + payload_size);

                return reply_eth_hdr;
            
            default:
                break;
        }
        return NULL;
    
    } else {

        if (!(tcp_hdr->flags & 0x2)) {
            printf("Not a SYN - Drop the packet\n");
            return NULL;
        }

        sockets_t * new_socket = (sockets_t *) calloc(1, sizeof(sockets_t));
        new_socket->remote_port = ntohs(tcp_hdr->sport);
        new_socket->remote_ip = ip4_pseudo_hdr->sip;
        new_socket->ack = ntohl(tcp_hdr->seq) + 1;
        new_socket->seq = 11042000;
        new_socket->state = SYN_RCVD;
        new_socket->next = NULL;

        s = sockets;
        if (!s) {
            sockets = new_socket;
        } else {
            while (s->next) {
                s = s->next;
            }
            s->next = new_socket;
        }
        
        
        

        eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
        tcp_hdr_t* reply_tcp_hdr = (tcp_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t) + sizeof(ip4_hdr_t));
        reply_tcp_hdr->sport = htons(1234);
        reply_tcp_hdr->dport = tcp_hdr->sport;
        reply_tcp_hdr->seq = htonl(new_socket->seq);
        reply_tcp_hdr->ack = htonl(new_socket->ack);
        reply_tcp_hdr->hlen_reserved = 0x50;
        reply_tcp_hdr->urg_ptr = 0;
        reply_tcp_hdr->window_size = 0xF0FA; //64240
        reply_tcp_hdr->flags = 0x12; // SYN ACK
        reply_tcp_hdr->csum = 0;

        reply_pseudo_hdr.dip = ip4_pseudo_hdr->sip;
        reply_pseudo_hdr.proto = TCP;
        reply_pseudo_hdr.sip = ip4_pseudo_hdr->dip;
        reply_pseudo_hdr.tcp_len = htons(20);
        reply_pseudo_hdr.fixed = 0;

        reply_tcp_hdr->csum = tcp_ckecksum(reply_tcp_hdr, &reply_pseudo_hdr);

        ip4_hdr_t* reply_ip4_hdr = (ip4_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t));
        reply_ip4_hdr->len = htons(sizeof(ip4_hdr_t) + sizeof(tcp_hdr_t));

        return reply_eth_hdr;
    }
    
}


u_int16_t tcp_ckecksum(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr) {
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
    
    return ~sum;
}
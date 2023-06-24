#include "tcp.h"
#include "utils.h"
#include "ip4.h"

extern const char MYIP[16];


socket_t* sockets[MAX_TCP_CONNECTIONS] = {0};


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
    socket_t* s = NULL;
    int socket_index;
    for (int i=0; i<MAX_TCP_CONNECTIONS; i++) {
        if (sockets[i] && sockets[i]->remote_ip == ip4_pseudo_hdr->sip && sockets[i]->remote_port == ntohs(tcp_hdr->sport) && sockets[i]->state != CLOSED) {
            s = sockets[i];
            socket_index = i;
            printf("TCP packet is part of the existing socket %d\n", i);
            break;
        }
    }

    if (s) {
        switch (s->state) {
            case SYN_RCVD:
                if (ntohl(tcp_hdr->ack) != s->seq + 1) {
                    printf("Error in ack number ! - Drop the packet\n");
                    break;
                }
                // ESTABLISHED
                s->state = ESTABLISHED;
                s->seq += 1;
                break;
            
            case ESTABLISHED:
                // ERROR
                if (s->seq != ntohl(tcp_hdr->ack)) {
                    printf("Error in ack number ! - Drop the packet.\n");
                    break;
                }

                // FIN
                if (tcp_hdr->flags & 0x1) {
                    printf("Client wants to close connection.\n");
                    break;
                }

                
                char* payload = (char*) tcp_hdr + 4*(tcp_hdr->hlen_reserved >> 4);
                int payload_size = ntohs(ip4_pseudo_hdr->tcp_len) - 4*(tcp_hdr->hlen_reserved >> 4);
                if (payload_size == 0) {
                    printf("Empty TCP ack packet\n");
                    return NULL;
                }

                eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
                s->ack += payload_size;
                payload[payload_size] = '\0';
                printf("MESSAGE: %s\n", payload);

                if (strncmp(payload, "exit", 4) == 0) {
                    printf("Reply with FIN\n");
                    fill_reply(s, reply_eth_hdr, &reply_pseudo_hdr, 0x11, NULL, 0);
                    s->state = FIN_WAIT;
                    s->seq += 1;
                } else {
                    fill_reply(s, reply_eth_hdr, &reply_pseudo_hdr, 0x10, payload, payload_size);
                }

                return reply_eth_hdr;
                break;
            
            case FIN_WAIT:
                if (tcp_hdr->flags & 0x1) {
                    s->ack += 1;
                    eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
                    // reply the last ACK
                    fill_reply(s, reply_eth_hdr, &reply_pseudo_hdr, 0x10, NULL, 0);
                    s->state = CLOSED;
                    free(s);
                    sockets[socket_index] = NULL;

                    return reply_eth_hdr;
                    break;
                }

            
            default:
                break;
        }
        return NULL;
    
    } else {

        if (!(tcp_hdr->flags & 0x2)) {
            printf("Not a SYN - Drop the packet\n");
            return NULL;
        }

        socket_t *new_socket = (socket_t *) calloc(1, sizeof(socket_t));
        new_socket->remote_port = ntohs(tcp_hdr->sport);
        new_socket->remote_ip = ip4_pseudo_hdr->sip;
        new_socket->ack = ntohl(tcp_hdr->seq) + 1;
        new_socket->seq = 11042000;
        new_socket->state = SYN_RCVD;

        socket_index = -1;
        for (int i=0; i<MAX_TCP_CONNECTIONS; i++) {
            if (sockets[i] == NULL) {
                socket_index = i;
                break;
            }
        }
        if (socket_index == -1) {
            printf("There is no available slot for the new incomming connection - Drop the packet\n");
            free(new_socket);
            return NULL;
        }
        sockets[socket_index] = new_socket;
        
        // retrun a SYN ACK
        eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);
        fill_reply(new_socket, reply_eth_hdr, &reply_pseudo_hdr, 0x12, NULL, 0);

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

void fill_reply(socket_t* s, eth_hdr_t* reply_eth_hdr, ip4_pseudo_hdr_t* reply_pseudo_hdr, u_int8_t flags, char* payload, size_t payload_size) {
    tcp_hdr_t* reply_tcp_hdr = (tcp_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t) + sizeof(ip4_hdr_t));
    ip4_hdr_t* reply_ip4_hdr = (ip4_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t));
    
    reply_tcp_hdr->sport = htons(1234);
    reply_tcp_hdr->dport = htons(s->remote_port);
    reply_tcp_hdr->seq = htonl(s->seq);
    reply_tcp_hdr->ack = htonl(s->ack);
    reply_tcp_hdr->hlen_reserved = 0x50;
    reply_tcp_hdr->urg_ptr = 0;
    reply_tcp_hdr->window_size = 0xF0FA; //64240
    reply_tcp_hdr->flags = flags;
    reply_tcp_hdr->csum = 0;

    reply_pseudo_hdr->dip = s->remote_ip;
    reply_pseudo_hdr->proto = TCP;
    inet_pton(AF_INET, MYIP, &reply_pseudo_hdr->sip);
    reply_pseudo_hdr->fixed = 0;
    if (payload) {
        reply_pseudo_hdr->tcp_len = htons(20 + payload_size);
        reply_ip4_hdr->len = htons(sizeof(ip4_hdr_t) + sizeof(tcp_hdr_t) + payload_size);
        memcpy(((char*) reply_tcp_hdr) + 20, payload, payload_size);
        s->seq += payload_size;
    } else {
        reply_pseudo_hdr->tcp_len = htons(20);
        reply_ip4_hdr->len = htons(sizeof(ip4_hdr_t) + sizeof(tcp_hdr_t));
    }

    reply_tcp_hdr->csum = tcp_ckecksum(reply_tcp_hdr, reply_pseudo_hdr);
}
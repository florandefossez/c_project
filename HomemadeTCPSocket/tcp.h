#ifndef tcp
#define tcp

#include "ether.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

// TCP states
typedef enum TCP_state_ {
    SYN_RCVD = 0,
    ESTABLISHED = 1,
    CLOSE_WAIT = 2,
    LAST_ACK = 3,
    CLOSED = 4
} TCP_state_t;

#pragma pack (push,1)
typedef struct tcp_hdr_ {
    uint16_t sport;
    uint16_t dport;
    uint32_t seq;
    uint32_t ack;
    uint8_t hlen_reserved;
    uint8_t flags;
    u_int16_t window_size;
    uint16_t csum;
    uint16_t urg_ptr;
} tcp_hdr_t;

typedef struct ip4_pseudo_hdr_ {
    u_int32_t sip;
    u_int32_t dip;
    u_int8_t fixed;
    u_int8_t proto;
    u_int16_t tcp_len;
} ip4_pseudo_hdr_t;
#pragma pack(pop)

typedef struct sockets_ {
    u_int16_t remote_port;
    u_int32_t remote_ip;
    TCP_state_t state;
    u_int16_t window_size;
    u_int32_t seq;
    u_int32_t ack;
    struct sockets_* next;
} sockets_t;

void print_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr);
eth_hdr_t* handle_tcp(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr);
u_int16_t tcp_ckecksum(tcp_hdr_t* tcp_hdr, ip4_pseudo_hdr_t* ip4_pseudo_hdr);

#endif
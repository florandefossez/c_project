#ifndef icmp
#define icmp

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "ether.h"


// ICMP types
#define ECHO_REQUEST    8
#define ECHO_REPLY      0


#pragma pack (push,1)
typedef struct icmp_hdr_ {
    uint8_t type;
    uint8_t code;
    uint16_t csum;
} icmp_hdr_t;
#pragma pack(pop)

void print_icmp(icmp_hdr_t* icmp_hdr, unsigned int size);
eth_hdr_t* icmp_handler(icmp_hdr_t* icmp_hdr, unsigned int size, u_int32_t sip);
eth_hdr_t* echo_reply(icmp_hdr_t* icmp_hdr, unsigned int size, u_int32_t sip);

#endif
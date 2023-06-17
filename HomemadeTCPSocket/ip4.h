#ifndef ipv4
#define ipv4

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

// transport protocols
#define TCP     6
#define UDP     17     
#define ICMP    1


#pragma pack (push,1)
typedef struct ip4_hdr_ {
    uint8_t version_ihl;    // always 4 AND ip4 hdr length in 32-bit word
    uint8_t tos;            // type of service
    uint16_t len;           // length hdr+payload in byte
    uint16_t frag_id;       // fragment id
    uint16_t flags_frag_offset;     // flags: NotUsed | DF | MF  AND  frag_offset
    uint8_t ttl;          // time to leave
    uint8_t proto;        // transport proto
    uint16_t csum;        // checksum
    uint32_t sip;         // source addr
    uint32_t dip;         // destination addr
} ip4_hdr_t;
#pragma pack(pop)

void print_ip4(ip4_hdr_t* ip4_hdr);
void ip4_handler(ip4_hdr_t* ip4_hdr);

#endif
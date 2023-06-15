# ifndef ether
# define ether

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// ethertype
#define ARP     0x0806
#define IPv4    0x0800
#define IPv6    0x86DD

#pragma pack (push,1)
typedef struct eth_hdr {
    u_int8_t dmac[6];
    u_int8_t smac[6];
    u_int16_t ethertype;
    char payload[500];
} eth_hdr_t;

typedef struct arp_hdr_ {
    u_int16_t hw_type;          /*1 for ethernet cable*/
    u_int16_t proto_type;       /*same as ethertype*/
    u_int8_t hwsize;            /*6 for MAC*/
    u_int8_t prosize;           /*4 for IPV4*/
    u_int16_t op_code;          /*request or reply*/
    u_int8_t smac[6];
    u_int32_t sip;
    u_int8_t dmac[6];
    u_int32_t dip;
} arp_hdr_t;
#pragma pack(pop)

void parse_eth(char* packet, ssize_t packet_size);
void arp_handler(arp_hdr_t* arp_hdr);
void print_arp(arp_hdr_t* arp_hdr);

# endif
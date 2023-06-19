#include "icmp.h"
#include "ip4.h"
#include "utils.h"

void print_icmp(icmp_hdr_t* icmp_hdr, unsigned int size) {
    char type[15];
    switch (icmp_hdr->type) {
    case ECHO_REPLY:
        strcpy(type, "ECHO_REPLY");
        break;
    case ECHO_REQUEST:
        strcpy(type, "ECHO_REQUEST");
        break;
    default:
        sprintf(type, "%u", icmp_hdr->type);
        break;
    }
    printf(
        "[ICMP] type: %s | code: %u | csum: 0x%04X | len: %u\n",
        type,
        icmp_hdr->code,
        ntohs(icmp_hdr->csum),
        size
    );
}


eth_hdr_t* icmp_handler(icmp_hdr_t* icmp_hdr, unsigned int size, u_int32_t sip) {
    switch (icmp_hdr->type) {
    case ECHO_REPLY:
        printf("[ICMP ECHO] Echo received !\n");
        break;
    case ECHO_REQUEST:
        return echo_reply(icmp_hdr, size, sip);
        break;
    default:
        printf("ICMP type not supported - Drop the packet\n");
        break;
    }
    return NULL;
}

eth_hdr_t* echo_reply(icmp_hdr_t* icmp_hdr, unsigned int size, u_int32_t sip) {
    printf(
        "[ICMP ECHO REQUEST] id: %u | seq: %u\n",
        ntohs(*((u_int16_t *) (((char *) icmp_hdr) + 4))),
        ntohs(*((u_int16_t *) (((char *) icmp_hdr) + 6)))
    );
    eth_hdr_t* reply_eth_hdr = (eth_hdr_t *) calloc(1, MAX_PACKET_SIZE);

    icmp_hdr_t* reply_icmp_hdr = (icmp_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t) + sizeof(ip4_hdr_t));
    reply_icmp_hdr->code = ECHO_REPLY;
    reply_icmp_hdr->type = 0;
    reply_icmp_hdr->csum = 0;
    char* reply_payload = ((char *) reply_icmp_hdr) + sizeof(icmp_hdr_t);
    char* src_payload = ((char *) icmp_hdr) + sizeof(icmp_hdr_t);
    memcpy(reply_payload, src_payload, size - sizeof(icmp_hdr_t));
    reply_icmp_hdr->csum = checksum((char *) reply_icmp_hdr, size);

    ip4_hdr_t* reply_ip4_hdr = (ip4_hdr_t *) (((char *) reply_eth_hdr) + sizeof(eth_hdr_t));
    reply_ip4_hdr->len = htons(sizeof(ip4_hdr_t) + size);

    return reply_eth_hdr;
}

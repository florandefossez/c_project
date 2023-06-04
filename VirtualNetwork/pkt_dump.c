#include "layer2/layer2.h"
#include <arpa/inet.h>
#include "tcpconst.h"

void packet_dump(ethernet_hdr_t *ethernet_hdr, unsigned int pkt_size)
{
    printf("----Ethernet Hdr------\n");
    printf("\t Src Mac : %u:%u:%u:%u:%u:%u\n",
           ethernet_hdr->src_mac.mac[0], ethernet_hdr->src_mac.mac[1],
           ethernet_hdr->src_mac.mac[2], ethernet_hdr->src_mac.mac[3],
           ethernet_hdr->src_mac.mac[4], ethernet_hdr->src_mac.mac[5]);
    printf("\t Dst Mac : %u:%u:%u:%u:%u:%u\n",
           ethernet_hdr->dst_mac.mac[0], ethernet_hdr->dst_mac.mac[1],
           ethernet_hdr->dst_mac.mac[2], ethernet_hdr->dst_mac.mac[3],
           ethernet_hdr->dst_mac.mac[4], ethernet_hdr->dst_mac.mac[5]);

    printf("\t Eth type : %d\n", ethernet_hdr->type);

    switch (ethernet_hdr->type) {

    case ARP_MSG:
    {
        arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);

        printf("\t--------ARP Hdr------");

        printf("\t\t Src Mac : %u:%u:%u:%u:%u:%u\n",
               ethernet_hdr->src_mac.mac[0], ethernet_hdr->src_mac.mac[1],
               ethernet_hdr->src_mac.mac[2], ethernet_hdr->src_mac.mac[3],
               ethernet_hdr->src_mac.mac[4], ethernet_hdr->src_mac.mac[5]);
        printf("\t\t Dst Mac : %u:%u:%u:%u:%u:%u\n",
               ethernet_hdr->dst_mac.mac[0], ethernet_hdr->dst_mac.mac[1],
               ethernet_hdr->dst_mac.mac[2], ethernet_hdr->dst_mac.mac[3],
               ethernet_hdr->dst_mac.mac[4], ethernet_hdr->dst_mac.mac[5]);

        // char ip_addr[16];
        // ip_addr_n_to_p(arp_hdr->src_ip, ip_addr);
        // printf("\t\t Src Ip: %s\n", ip_addr);

        // ip_addr_n_to_p(arp_hdr->dst_ip, ip_addr);
        // printf("\t\t Dest Ip: %s\n", ip_addr);
        break;
    }

    default:
    {
        unsigned int offset = (char *)&(((ethernet_hdr_t *)0)->payload);
        printf("Payload offset: %d, Payload size: %d\n", offset, pkt_size - ETH_HDR_SIZE_EXCL_PAYLOAD);
        break;
    }
    }
}
#ifndef net
#define net

#include <string.h>
#include <stdbool.h>
#include "utils.h"

typedef struct graph_ graph_t;
typedef struct interface_ interface_t;
typedef struct node_ node_t;
typedef struct arp_table_ arp_table_t;


#pragma pack (push,1)
typedef struct ip_addr_ {
    unsigned char ip_addr[16];
} ip_add_t;

typedef struct mac_add_ {
    unsigned char mac[6];
} mac_add_t;
#pragma pack(pop)

typedef struct node_nw_prop_{
    bool is_lb_addr_config;
    ip_add_t lb_addr;

    arp_table_t* arp_table;
} node_nw_prop_t;

extern void init_arp_table(arp_table_t **arp_table);

static inline void init_node_nw_prop(node_nw_prop_t* node_nw_prop) {
    node_nw_prop->is_lb_addr_config = false;
    memset(node_nw_prop->lb_addr.ip_addr, 0, 16);
    init_arp_table(&(node_nw_prop->arp_table));
}






typedef struct intf_nw_props_ {

    mac_add_t mac_add;
    bool is_ipadd_config; 
    ip_add_t ip_add;
    char mask;
} intf_nw_props_t;

static inline void
init_intf_nw_prop(intf_nw_props_t *intf_nw_props) {


    memset(intf_nw_props->mac_add.mac , 0 , 
        sizeof(intf_nw_props->mac_add.mac));

    intf_nw_props->is_ipadd_config = false;
    memset(intf_nw_props->ip_add.ip_addr, 0, 16);
    intf_nw_props->mask = 0;

}

#define IF_MAC(intf_ptr)    ((intf_ptr)->intf_nw_props.mac_add.mac)
#define IF_IP(intf_ptr)     ((intf_ptr)->intf_nw_props.ip_add.ip_addr)

#define NODE_LO_ADDR(node_ptr) (node_ptr->node_nw_prop.lb_addr.ip_addr)

void interface_assign_mac_address(interface_t *interface);

bool node_set_loopback_address(node_t* node, char* ipaddr);
bool node_set_intf_ip_address(node_t* node, char* intf_name, char* ip_addr, char mask);
bool node_unset_intf_ip_address(node_t* node, char* intf_name);

void dump_nw_graph(graph_t *graph);
void dump_node_nw_props(node_t *node);
void dump_intf_props(interface_t *interface);

interface_t* node_get_matching_subnet_interface(node_t *node, char *ip_addr);


unsigned int convert_ip_from_str_to_int(char *ip_addr);
char* pkt_buffer_shift_right(char *pkt, unsigned int pkt_size, 
                       unsigned int total_buffer_size);

#define IS_INTF_L3_MODE(intf_ptr) intf_ptr->intf_nw_props.is_ipadd_config
#define NODE_ARP_TABLE(node_ptr)    (node_ptr->node_nw_prop.arp_table)

interface_t* node_get_matching_subnet_interface(node_t *node, char *ip_addr);

#endif


#include <stdio.h>
#include "graph.h"
#include "layer3.h"
#include <sys/socket.h>
#include <memory.h>
#include <stdlib.h>
#include "tcpconst.h"
#include "comm.h"
#include <arpa/inet.h> /*for inet_ntop & inet_pton*/


// ROUTING TABLE FUNCTIONS

void init_rt_table(rt_table_t **rt_table) {
    *rt_table = calloc(1, sizeof(rt_table_t));
    init_glthread(&((*rt_table)->route_list));
}

l3_route_t* rt_table_lookup(rt_table_t *rt_table, char *ip_addr, char mask) {
    glthread_t *curr;
    l3_route_t *l3_route;
    ITERATE_GLTHREAD_BEGIN(&rt_table->route_list, curr) {
        l3_route = rt_glue_to_l3_route(curr);
        if(strncmp(l3_route->dest, ip_addr, 16) == 0 && l3_route->mask == mask){
            return l3_route;
        }
    } ITERATE_GLTHREAD_END(&rt_table->route_list, curr);
    return NULL;
}

/*Look up L3 routing table using longest prefix match*/
l3_route_t* l3rib_lookup_lpm(rt_table_t *rt_table, unsigned int dest_ip) {
    l3_route_t* l3_route = NULL;
    l3_route_t* lpm_l3_route = NULL;
    // l3_route_t* default_l3_rt = NULL;
    glthread_t *curr = NULL;
    char subnet[16];
    char dest_ip_str[16];
    char longest_mask = 0;
   
    dest_ip = htonl(dest_ip); 
    inet_ntop(AF_INET, &dest_ip, dest_ip_str, 16);
    dest_ip_str[15] = '\0';
    ITERATE_GLTHREAD_BEGIN(&rt_table->route_list, curr){
        l3_route = rt_glue_to_l3_route(curr);
        memset(subnet, 0, 16);
        apply_mask(dest_ip_str, l3_route->mask, subnet);
        // if(strncmp("0.0.0.0", l3_route->dest, 16) == 0 && l3_route->mask == 0) {
        //     default_l3_rt = l3_route;
        // } else 
        if (
            strncmp(subnet, l3_route->dest, strlen(subnet)) == 0 &&
            l3_route->mask > longest_mask
        ) {
            longest_mask = l3_route->mask;
            lpm_l3_route = l3_route;
        }
    } ITERATE_GLTHREAD_END(&rt_table->route_list, curr);
    return lpm_l3_route;
}

void clear_rt_table(rt_table_t *rt_table) {
    glthread_t *curr;
    l3_route_t *l3_route;
    ITERATE_GLTHREAD_BEGIN(&rt_table->route_list, curr){
        l3_route = rt_glue_to_l3_route(curr);
        remove_glthread(curr);
        free(l3_route);
    } ITERATE_GLTHREAD_END(&rt_table->route_list, curr);
}

void delete_rt_table_entry(rt_table_t *rt_table, char *ip_addr, char mask) {
    char dst_str_with_mask[16];
    apply_mask(ip_addr, mask, dst_str_with_mask); 
    l3_route_t *l3_route = rt_table_lookup(rt_table, dst_str_with_mask, mask);
    if(!l3_route)
        return;
    remove_glthread(&l3_route->rt_glue);
    free(l3_route);
}

static bool _rt_table_entry_add(rt_table_t *rt_table, l3_route_t *l3_route) {
    l3_route_t *l3_route_old = rt_table_lookup(rt_table, l3_route->dest, l3_route->mask);
    if (l3_route_old && IS_L3_ROUTES_EQUAL(l3_route_old, l3_route)) {
        return false;
    }
    if (l3_route_old) {
        delete_rt_table_entry(rt_table, l3_route_old->dest, l3_route_old->mask);
    }
    init_glthread(&l3_route->rt_glue);
    glthread_add_next(&rt_table->route_list, &l3_route->rt_glue);
    return true;
}

void rt_table_add_route(rt_table_t *rt_table, char *dst, char mask, char *gw, char *oif) {
    unsigned int dst_int;
    char dst_str_with_mask[16];
    apply_mask(dst, mask, dst_str_with_mask); 
    inet_pton(AF_INET, dst_str_with_mask, &dst_int);

    /*Trying to add duplicate route ?*/
    l3_route_t *l3_route = l3rib_lookup_lpm(rt_table, dst_int);
    assert(!l3_route);

    l3_route = calloc(1, sizeof(l3_route_t));
    strncpy(l3_route->dest, dst_str_with_mask, 16);
    l3_route->dest[15] = '\0';
    l3_route->mask = mask;
    l3_route->is_direct = !gw && !oif;

    if(gw && oif){
        strncpy(l3_route->gw_ip, gw, 16);
        l3_route->gw_ip[15] = '\0';
        strncpy(l3_route->oif, oif, IF_NAME_SIZE);
        l3_route->oif[IF_NAME_SIZE - 1] = '\0';
    }

    if(!_rt_table_entry_add(rt_table, l3_route)){
        printf("Error : Route %s/%d Installation Failed\n", 
        dst_str_with_mask, mask);
        free(l3_route);   
    }
}


void rt_table_add_direct_route(rt_table_t *rt_table, char *dst, char mask) {
    rt_table_add_route(rt_table, dst, mask, 0, 0);
}
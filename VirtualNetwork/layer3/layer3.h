#ifndef layer3
#define layer3

#include "../gluethread/glthread.h"

typedef struct rt_table_{
    glthread_t route_list;    
} rt_table_t;

typedef struct l3_route_{
    char dest[16];
    char mask;
    bool is_direct;  /*if set to True, then gw_ip and oif has no meaning*/
    char gw_ip[16];
    char oif[IF_NAME_SIZE];
    glthread_t rt_glue;
} l3_route_t;
GLTHREAD_TO_STRUCT(rt_glue_to_l3_route, l3_route_t, rt_glue);

#define IS_L3_ROUTES_EQUAL(rt1, rt2)              \
    ((strncmp(rt1->dest, rt2->dest, 16) == 0) &&  \
    (rt1->mask == rt2->mask) &&                   \
    (rt1->is_direct == rt2->is_direct) &&         \
    (strncmp(rt1->gw_ip, rt2->gw_ip, 16) == 0) && \
    strncmp(rt1->oif, rt2->oif, IF_NAME_SIZE) == 0)


#endif /* layer3 */

void init_rt_table(rt_table_t **rt_table);
void clear_rt_table(rt_table_t *rt_table);
void delete_rt_table_entry(rt_table_t *rt_table, char *ip_addr, char mask);
void rt_table_add_route(rt_table_t *rt_table, char *dst, char mask, char *gw, char *oif);
void rt_table_add_direct_route(rt_table_t *rt_table, char *dst, char mask);
void dump_rt_table(rt_table_t *rt_table);
l3_route_t* l3rib_lookup_lpm(rt_table_t *rt_table, unsigned int dest_ip);
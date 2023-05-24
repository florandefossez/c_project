#include "gluethread/glthread.h"
#include <stdlib.h>
#include "net.h"
#include <assert.h>


#define IF_NAME_SIZE 16
#define NODE_NAME_SIZE 16
#define MAX_INTF_PER_NODE 10


typedef struct node_ node_t;
typedef struct link_ link_t;


typedef struct interface_ {
    char if_name[IF_NAME_SIZE];
    struct node_* att_node;
    struct link_* link;
    intf_nw_props_t intf_nw_props;
} interface_t;

struct link_ {
    interface_t intf1;
    interface_t intf2;
    unsigned int cost;
};

struct node_ {
    char node_name[NODE_NAME_SIZE];
    interface_t* intf[MAX_INTF_PER_NODE];
    glthread_t graph_glue;
    node_nw_prop_t node_nw_prop;
};


typedef struct graph_ {
    char topology_name[32];
    glthread_t node_list;
} graph_t;

static inline node_t* get_nbr_node(interface_t* interface) {
    if (interface == &interface->link->intf1) {
        return interface->link->intf2.att_node;
    } else {
        return interface->link->intf1.att_node;
    }   
}

static inline int get_node_intf_available_slot(node_t *node) {
    int index = 0;
    while (node->intf[index] && index<MAX_INTF_PER_NODE) {
        index++;
    }
    return index == MAX_INTF_PER_NODE ? -1 : index;    
}

GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);

static inline interface_t* get_node_if_by_name(node_t* node, char* if_name) {
    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        if (strcmp(node->intf[i]->if_name, if_name) == 0) 
            return node->intf[i];
    }
    return NULL;
}

static inline node_t* get_node_by_node_name(graph_t* topo, char* node_name) {
    glthread_t *current;
    node_t *node;

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, current) {
        node = graph_glue_to_node(current);
        if (strcmp(node->node_name, node_name) == 0) {
            return node;
        }
    } ITERATE_GLTHREAD_END(&topo->node_list, current);
    return NULL;
}


graph_t* create_new_graph(char* topology_name);
node_t* create_graph_node(graph_t* graph, char* node_name);
void insert_link_between_two_nodes(
    node_t* node1,
    node_t* node2,
    char* from_if_name,
    char* to_if_name,
    unsigned int cost);
void dump_graph(graph_t *graph);
void dump_node(node_t *node);
void dump_interface(interface_t *interface);


#include "graph.h"
#include "CommandParser/libcli.h"
#include <stdio.h>
#include <unistd.h> // for sleep function

extern graph_t *linear_node_topo();
extern graph_t *build_simple_l2_switch_topo();
extern graph_t *build_dualswitch_topo();

extern void nw_init_cli();
graph_t *topo = NULL;

extern int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

int main(int argc, char **argv) {

    nw_init_cli();
    topo = build_dualswitch_topo();

    sleep(2);


    start_shell();
    return 0;
}
#include "graph.h"
#include "CommandParser/libcli.h"
#include <stdio.h>
#include <unistd.h> // for sleep function

extern graph_t *build_first_topo();
extern void nw_init_cli();
graph_t *topo = NULL;

extern int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

int main(int argc, char **argv) {

    nw_init_cli();
    topo = build_first_topo();

    sleep(2);

    node_t *node = get_node_by_node_name(topo, "R0_re");    interface_t *oif = get_node_if_by_name(node, "eth0/0");

    char msg[] = "Hello world\0";
    send_pkt_out(msg, strlen(msg), oif);

    start_shell();
    return 0;
}
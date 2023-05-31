#include "graph.h"
#include "CommandParser/libcli.h"
#include <stdio.h>
#include <unistd.h> // for sleep function

extern graph_t *linear_node_topo();
extern void nw_init_cli();
graph_t *topo = NULL;

extern int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

int main(int argc, char **argv) {

    nw_init_cli();
    topo = linear_node_topo();

    sleep(2);

    node_t *node = get_node_by_node_name(topo, "R1");    interface_t *oif = get_node_if_by_name(node, "eth0/1");

    char msg[] = "Hello world\0";
    send_pkt_out(msg, strlen(msg), oif);

    start_shell();
    return 0;
}
#ifndef comm
#define comm

#define MAX_PACKET_BUFFER_SIZE   2048

typedef struct node_ node_t;
// typedef struct graph_ graph_t;
typedef struct interface_ interface_t;

// int get_next_udp_port_number();
void init_udp_socket(node_t* node);

int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);
int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size);

#endif
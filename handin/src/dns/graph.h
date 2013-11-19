#ifndef _GRAPH
#define _GRAPH

#include <arpa/inet.h>
#include "roundrobin.h"

struct neighbor_list;

typedef struct node_list {
  char ip[INET_ADDRSTRLEN + 1];
  int last_seqnum;
  int distance;
  int visited;
  int pq_index;

  struct neighbor_list *neighbors;
  struct node_list *next;
} node_list;

typedef struct neighbor_list {
  struct node_list *node;
  struct neighbor_list *next;
} neighbor_list;

void newLSA(node_list **nodes, 
	    char *senderIP, int seqnum, neighbor_list *neighbors);
node_list *parse_file(char *LSAfile);
node_list *closest_server(node_list *graph, char *source, server_loop *servers);

#endif

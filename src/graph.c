#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "priority_queue.h"

#define MAXNEIGHBORS 100

/* Adds the node to nodes if doesn't exist: */
node_list *find_node(node_list **nodes, char *node) {
  while (1) {
    if (*nodes == NULL) {
      (*nodes) = malloc(sizeof(node_list));
      memset((*nodes)->ip, 0, INET_ADDRSTRLEN);
      strncpy((*nodes)->ip, node, INET_ADDRSTRLEN);
      (*nodes)->next = NULL;
      (*nodes)->neighbors = NULL;
      (*nodes)->last_seqnum = -1;
      (*nodes)->distance = -1;
      return *nodes;
    }
    if (strncmp((*nodes)->ip, node, INET_ADDRSTRLEN) == 0)
      return *nodes;
    nodes = &((*nodes)->next);
  }
}

void remove_neighbor(node_list *node, char *ip) {
  /*
  printf("Remove neighbor!!\n");
    char copy1[INET_ADDRSTRLEN + 1];
    strncpy(copy1, node->neighbors->ip, INET_ADDRSTRLEN);
    copy1[INET_ADDRSTRLEN] = 0;
    printf("first ip = %s\n", copy1);
  if (strncmp(node->neighbors->ip, ip, INET_ADDRSTRLEN) == 0) {
    free(node->neighbors);
    node->neighbors = NULL;
    return;
  }
  node_list *cur = node->neighbors;
  while (cur->next != NULL) {
    char copy[INET_ADDRSTRLEN + 1];
    strncpy(copy, cur->next->ip, INET_ADDRSTRLEN);
    copy[INET_ADDRSTRLEN] = 0;
    printf("cur->next->ip = %s\n", copy);

    if (strncmp(cur->next->ip, ip, INET_ADDRSTRLEN) == 0) {
      free(cur->next);
      cur->next = cur->next->next;
      return;
    }
    }*/
}

void add_neighbor(node_list *node, node_list *neighbor) {
  /*
    char copy[INET_ADDRSTRLEN + 1];
    strncpy(copy, ip, INET_ADDRSTRLEN);
    copy[INET_ADDRSTRLEN] = 0;
    printf("ip = %s\n", copy);*/

  neighbor_list *new = malloc(sizeof(neighbor_list));
  new->node = neighbor;
  new->next = node->neighbors;
  node->neighbors = new;
}

void newLSA(node_list **nodes, 
	    char *senderIP, int seqnum, neighbor_list *neighbors) {
  printf("newLSA...\n");

  node_list *sender = find_node(nodes, senderIP);
  if (sender->last_seqnum >= seqnum)
    return;
  sender->last_seqnum = seqnum;

  /* Remove all neighbors: */
  neighbor_list *cur = sender->neighbors;
  while (cur != NULL) {
    neighbor_list *next = cur->next;
    free(cur);
    cur = next;
  }

  sender->neighbors = neighbors;
}

/* For print_graph: */
int index_for_ip(node_list *nodes, char *ip) {
  int index = 0;
  while (nodes != NULL) {
    char copy[INET_ADDRSTRLEN + 1];
    strncpy(copy, nodes->ip, INET_ADDRSTRLEN);
    copy[INET_ADDRSTRLEN] = 0;
    //printf("nodes->ip = %s, ip = %s\n", copy, ip);

    if (strncmp(nodes->ip, ip, INET_ADDRSTRLEN) == 0)
      return index;
    nodes = nodes->next;
    index++;
  }
  fprintf(stderr, "Couldn't find node %s!!\n", ip);
  return 0;
}

void print_graph(node_list *graph) {
  int len = 0;
  node_list *cur = graph;
  while (cur != NULL) {
    len++;
    cur = cur->next;
  }
  char **adjMat = malloc(len * sizeof(char *));
  int i, j;
  for (i = 0; i < len; i++) {
    adjMat[i] = malloc(len * sizeof(char));
    memset(adjMat[i], 0, len);
  }
  cur = graph;
  while (cur != NULL) {
    printf("Looking at %s\n", cur->ip);
    int cur_index = index_for_ip(graph, cur->ip);
    neighbor_list *cur_neighbor = cur->neighbors;
    while (cur_neighbor != NULL) {
      int neighbor_index = index_for_ip(graph, cur_neighbor->node->ip);
      printf("Setting adjMat[%d][%d]\n", cur_index, neighbor_index);
      adjMat[cur_index][neighbor_index] = 1;
      cur_neighbor = cur_neighbor->next;
    }
    cur = cur->next;
  }

  for (i = 0; i < len; i++) {
    for (j = 0; j < len; j++) {
      printf("%d ", adjMat[i][j]);
    }
    printf("\n");
  }
}

node_list *parse_file(char *LSAfile) {
  node_list *nodes = NULL;
  FILE *f = fopen(LSAfile, "r");
  if (!f) {
    fprintf(stderr, "Couldn't open LSA file! :(\n");
    return NULL;
  }
  char sender[INET_ADDRSTRLEN + 1];

  int seqnum;
  char neighbor_str[(INET_ADDRSTRLEN + 1) * MAXNEIGHBORS];
  printf("Opened, going to read...\n");
  int ret_val;
  while (1) {
    ret_val = fscanf(f, "%s %d %s", sender, &seqnum, neighbor_str);
    printf("got ret_val = %d\n", ret_val);
    if (ret_val <= 0)
      break;
    printf("Read something!\n");

    neighbor_list *neighbors = malloc(sizeof(node_list));
    neighbors->next = NULL;
    char *next = neighbor_str;
    while (1) {
      printf("next = %s\n", next);
      char *comma = strchr(next, ',');
      if (comma == NULL) {
	neighbors->node = find_node(&nodes, next);
	break;
      }
      char *ip = malloc(comma - next + 1);
      strncpy(ip, next, comma - next);
      ip[comma - next] = 0;
      neighbors->node = find_node(&nodes, ip);
      free(ip);

      next = comma + 1;
      neighbor_list *new = malloc(sizeof(neighbor_list));
      new->next = neighbors;
      neighbors = new;
    }

    /*
    int k = 0;

    while (1) {
      ret_val = fscanf(f, ", %[^,]", neighbor);
      printf("ret_val: %d, EOF = %d\n", ret_val, EOF);
      if (ret_val <= 0)
	break;
      k++;
      if (k > 6) {
	printf("Quitting..\n");
	return NULL;
      }
      node_list *new = malloc(sizeof(node_list));
      strncpy(new->ip, neighbor, INET_ADDRSTRLEN);
      new->next = neighbors;
      neighbors = new;
    }
    */

    printf("newLSA!: %s, %d, neighbors\n", sender, seqnum);
    newLSA(&nodes, 
	   sender, seqnum, neighbors);
  }
  fclose(f);
  return nodes;
}

int is_video_server(char *ip) {
  printf("Is %s a video server? %s!\n", ip, ip[0] <= 'Z' ? "Yes" : "No");
  return ip[0] <= 'Z';
}

node_list *closest_server(node_list *graph, char *ip) {
  // Reset all the Dijkstra flags:
  node_list *cur = graph;
  while (cur != NULL) {
    cur->distance = -1;
    cur->visited = 0;
    cur->pq_index = -1;
    cur = cur->next;
  }

  node_list *source = find_node(&graph, ip);
  if (source->neighbors == NULL)
    printf("No neighbors!\n");
  source->distance = 0;
  pq *queue = newPQ();
  insert(source, queue);

  node_list *next = NULL;
  while (!empty(queue)) {
    next = pop(queue);
    if (next->visited)
      continue;
    printf("Visit %s\n", next->ip);
    next->visited = 1;
    if (is_video_server(next->ip))
      break;

    neighbor_list *neighbor = next->neighbors;
    while (neighbor != NULL) {
      node_list *cur = neighbor->node;
      printf("%s is a neighbor\n", cur->ip);
      if (!cur->visited) {
	printf("%s is unvisited...\n", cur->ip);
	if (cur->distance < 0 || 
	    next->distance + 1 < cur->distance) {
	  printf("Updating %s\n", cur->ip);
	  cur->distance = next->distance + 1;
	  if (cur->pq_index >= 1)
	    update(cur, queue);
	  else
	    insert(cur, queue);
	}
      }
      neighbor = neighbor->next;
    }
  }
  return next;
}

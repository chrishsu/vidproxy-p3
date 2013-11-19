/**
 * @file priority_queue.h
 * @author David Wise
 */

#ifndef _PQ
#define _PQ

#include "graph.h"

typedef node_list data;

int priority(data *d);

typedef struct {
  data **array;
  int next;
  int len;
} pq;

pq *newPQ();
int empty(pq *heap);
void insert(data *new, pq *heap);
void update(data *element, pq *heap);
data *pop(pq *heap);

#endif

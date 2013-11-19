#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"

/**
 * The priority of d:
 */
int priority(data *d) {
  return d->distance;
}

/**
 * Swaps the contents of *a and *b
 */
void swap(data **a, data **b) {
  data *tmp = *a;
  *a = *b;
  *b = tmp;
}

/**
 * Standard heap sift up
 */
void sift_up(int index, pq *heap) {
  if (index <= 1)
    return;
  if (priority(heap->array[index]) >= priority(heap->array[index / 2]))
    return;
  heap->array[index]->pq_index = index / 2;
  heap->array[index / 2]->pq_index = index;
  swap(&(heap->array[index]), &(heap->array[index / 2]));
  sift_up(index / 2, heap);
}

/**
 * Standard heap sift down
 */
void sift_down(int index, pq *heap) {
  if (2 * index >= heap->next)
    return;
  int minIndex = 2 * index;
  if (2 * index + 1 < heap->next && 
      priority(heap->array[2 * index + 1]) < priority(heap->array[2 * index]))
    minIndex = 2 * index + 1;
  if (priority(heap->array[index]) <= priority(heap->array[minIndex]))
    return;
  heap->array[index]->pq_index = minIndex;
  heap->array[minIndex]->pq_index = index;
  swap(&(heap->array[index]), &(heap->array[minIndex]));
  sift_down(minIndex, heap);
}

/**
 * Doubles the capacity of the heap
 */
void resize(pq *heap) {
  data **newArray = malloc(sizeof(data *) * (2 * heap->len));
  memcpy(newArray, heap->array, heap->len * sizeof(data *));
  heap->len *= 2;
  free(heap->array);
  heap->array = newArray;
}

/**
 * Initializes and returns a new priority queue
 */
pq *newPQ() {
  pq *heap = malloc(sizeof(pq));
  heap->len = 4;
  heap->next = 1;
  heap->array = malloc(sizeof(data *) * heap->len);
  return heap;
}

/**
 * Returns true iff the heap is empty
 */
int empty(pq *heap) {
  return heap->next == 1;
}

/**
 * Inserts new into heap
 */
void insert(data *new, pq *heap) {
  if (heap->next == heap->len)
    resize(heap);
  heap->array[heap->next] = new;
  heap->array[heap->next]->pq_index = heap->next;
  heap->next++;
  sift_up(heap->next - 1, heap);
}

/**
 * Moves element in heap so that it has the correct
 * position based on its priority
 */
void update(data *element, pq *heap) {
  sift_up(element->pq_index, heap);
  sift_down(element->pq_index, heap);
}

/**
 * Removes and returns the minimum
 */
data *pop(pq *heap) {
  if (heap->len < 1) {
    fprintf(stderr, "Trying to pop empty priority queue!\n");
    return NULL;
  }
  data *min = heap->array[1];
  heap->array[1] = heap->array[heap->next - 1];
  heap->next--;
  sift_down(1, heap);

  return min;
}

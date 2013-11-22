#include "request.h"

/**
 * Initializes a request with the given bit rate, seq num, and frag num.
 *
 * @param[in] br    The bit rate.
 * @param[in] seq   The sequence number.
 * @param[in] frag  The fragment number.
 *
 * @return The request object.
 */
request *request_init(int br, int seq, int frag) {
  request *r = malloc(sizeof(request));
  gettimeofday(r->start, NULL);
  r->bitrate = br;
  r->throughput = 0.0;
  r->seq_num = seq;
  r->frag_num = frag;
  r->chunksize = 0;
  return r;
}

/**
 * Completes the request. Calculates the throughput.
 *
 * @param[out] r         The request.
 * @param[in] chunksize  The size of the chunk.
 */
void request_complete(request *r) {
  if (r == NULL) return;
  gettimeofday(r->end, NULL);
  float diff = (r->end.tv_sec - r->start.tv_sec) +
  (0.000001 * (r->end.tv_usec - r->start.tv_usec));
  if (diff == 0) diff = 1; // Don't divide by zero.
  r->throughput = r->chunksize/diff;
}

/**
 * Frees the request object.
 *
 * @param[out] r  The request.
 */
void request_free(request *r) {
  if (r == NULL) return;
  free(r);
  r = NULL;
}

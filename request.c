#include "request.h"

/**
 * Initializes a request with the given bit rate, chunk name,
 * and IP address.
 *
 * @param[in] br    The bit rate.
 *
 * @return The request object.
 */
request *request_init(int br) {
  request *r = malloc(sizeof(request));
  r->start = time(NULL);
  r->end = 0;
  r->bitrate = br;
  r->chunksize = 0;
  return r;
}

/**
 * Completes the request.
 *
 * @param[out] r         The request.
 * @param[in] chunksize  The size of the chunk.
 */
void request_complete(request *r, int chunksize) {
  if (r == NULL) return;
  r->end = time(NULL);
  r->chunksize = chunksize;
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

#include "request.h"

/**
 * Initializes a request with the given bit rate, chunk name,
 * and IP address.
 *
 * @param[in] br    The bit rate.
 * @param[in] ip    The IP string.
 * @param[in] name  The chunk name.
 *
 * @return The request object.
 */
request *request_init(int br, char *ip, char *name) {
  request *r = malloc(sizeof(request));
  r->start = time(NULL);
  r->end = 0;
  r->bitrate = br;
  memcpy(r->client_ip, ip, IP_LEN);
  r->chunksize = 0;
  memcpy(r->chunkname, name, CHUNKNAME_LEN);
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

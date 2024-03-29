#include "stream.h"

/**
 * Initialize a stream object.
 *
 * @param[in] br  The current bitrate.
 *
 * @return The stream object.
 */
stream *stream_init(int br) {
  stream *s = malloc(sizeof(stream));
  s->cur_bitrate = 0;
  s->throughput = (float)br;
  s->cur_request = NULL;
  return s;
}

/**
 * Add a request to the stream object.
 * Automatically frees any previous request.
 *
 * @param[in/out] s  The stream.
 * @param[in] r      The request.
 */
void stream_add_request(stream *s, request *r) {
  if (s == NULL) return;
  if (s->cur_request != NULL) {
    request_free(s->cur_request);
    s->cur_request = NULL;
  }
  s->cur_request = r;
}

/**
 * Adds a chunksize the request.
 *
 * @param[out] s         The stream.
 * @param[in] chunksize  The size of the chunk.
 */
void stream_request_chunksize(stream *s, int chunksize) {
  if (s == NULL) {
    fprintf(stderr, "stream_request_chunksize s is NULL!\n");
    return;
  }
  if (s->cur_request == NULL) {
    fprintf(stderr, "stream_request_chunksize request is NULL!\n");
    return;
  }
  // printf("Setting chunksize: %d!!!\n", chunksize);
  s->cur_request->chunksize = chunksize;
}

/**
 * Completes the request.
 *
 * @param[out] s         The stream.
 * @param[in] chunksize  The size of the chunk.
 */
void stream_request_complete(stream *s) {
  if (s == NULL) return;
  request_complete(s->cur_request);
}

/**
 * Calculates the throughput.
 *
 * @param[in/out] s  The stream.
 * @param[in] alpha  The alpha value.
 */
void stream_calc_throughput(stream *s, float alpha) {
  if (s == NULL) return;
  if (s->cur_request == NULL) return;
  float t_new = s->cur_request->throughput;
  float t_cur = (alpha * t_new) + ((1 - alpha) * s->throughput);
  s->throughput = t_cur;
}

/**
 * Frees the stream object and any assoicated request object.
 */
void stream_free(stream *s) {
  if (s == NULL) return;
  stream_add_request(s, NULL);
  free(s);
}

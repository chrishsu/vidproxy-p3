#include "stream.h"

/**
 * Initialize a stream object.
 *
 * @return The stream object.
 */
stream *stream_init() {
  stream *s = malloc(sizeof(stream));
  s->cur_bitrate = 0;
  s->throughput = 0;
  s->cur_request = NULL;
  return s;
}

/**
 * Add a request to the stream object.
 * Automatically frees any previous request.
 */
void stream_add_request(stream *s,request *r) {
  if (s == NULL) return;
  if (s->cur_request != NULL) {
    request_free(s->cur_request);
    s->cur_request = NULL;
  }
  s->cur_request = r;
}

/**
 * Frees the stream object and any assoicated request object.
 */
void stream_free(stream *s) {
  if (s == NULL) return;
  stream_add_request(s, NULL);
  free(s);
}
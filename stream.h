
#ifndef VP_STREAM
#define VP_STREAM

#include "request.h"

typedef struct {
  int cur_bitrate;
  int throughput;
  request *cur_request;
} stream;

stream *stream_init();
void stream_add_request(stream *s, request *r);
void stream_free(stream *s);

#endif
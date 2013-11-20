
#ifndef VP_STREAM
#define VP_STREAM

#include "request.h"

typedef struct {
  int cur_bitrate;
  int throughput;
  request *cur_request;
} stream;

stream *stream_init(int br);
void stream_add_request(stream *s, request *r);
void stream_calc_throughput(stream *s, int alpha);
void stream_free(stream *s);

#endif
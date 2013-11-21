
#ifndef VP_STREAM
#define VP_STREAM

#include <stdlib.h>
#include "request.h"

typedef struct {
  int cur_bitrate;
  float throughput;
  request *cur_request;
} stream;

stream *stream_init(int br);
void stream_add_request(stream *s, request *r);
void stream_request_chunksize(stream *s, int chunksize);
void stream_request_complete(stream *s);
void stream_calc_throughput(stream *s, float alpha);
void stream_free(stream *s);

#endif

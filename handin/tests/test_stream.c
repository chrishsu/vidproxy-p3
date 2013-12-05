#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../src/proxy/stream.h"
#include "../src/proxy/request.h"

#define ALPHA .1

int main() {
  int diff;
  float throughput, updated_t;

  printf("testing stream..\n\n");

  stream *s = stream_init(200);

  /* Add first request */
  stream_add_request(s, request_init(s->cur_bitrate, 1, 1));
  sleep(1);
  stream_request_chunksize(s, 15000);
  stream_request_complete(s);
  diff = (int)(s->cur_request->end.tv_sec - s->cur_request->start.tv_sec);
  throughput = 15000/diff;
  updated_t = (ALPHA * throughput) + (1 - ALPHA) * s->throughput;
  printf("request 1..\n");
  printf("diff: %d\n", diff);
  printf("throughput: %f\n", throughput);
  printf("new throughput: %f\n", updated_t);
  stream_calc_throughput(s, ALPHA);
  printf("calc throughput: %f\n\n", s->throughput);

  /* Add another request */
  stream_add_request(s, request_init(s->cur_bitrate, 1, 2));
  sleep(1);
  stream_request_chunksize(s, 15000);
  stream_request_complete(s);
  diff = (int)(s->cur_request->end.tv_sec - s->cur_request->start.tv_sec);
  throughput = 15000/diff;
  updated_t = (ALPHA * throughput) + (1 - ALPHA) * s->throughput;
  printf("request 2..\n");
  printf("diff: %d\n", diff);
  printf("throughput: %f\n", throughput);
  printf("new throughput: %f\n", updated_t);
  stream_calc_throughput(s, ALPHA);
  printf("calc throughput: %f\n\n", s->throughput);

  stream_free(s);

  printf("All tests passed!\n");

  return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../stream.h"
#include "../request.h"

#define ALPHA .1

int main() {
  int diff, throughput, updated_t;

  printf("testing stream..\n\n");

  stream *s = stream_init(200);

  /* Add first request */
  stream_add_request(s, request_init(s->cur_bitrate, "1.1.1.1", "300Seq1-Frag1"));
  sleep(1);
  request_complete(s->cur_request, 15000);

  diff = (int)(s->cur_request->end - s->cur_request->start);
  throughput = s->cur_request->chunksize/diff;
  updated_t = (ALPHA * throughput) + (1 - ALPHA) * s->throughput;
  printf("request 1..\n");
  printf("diff: %d\n", diff);
  printf("throughput: %d\n", throughput);
  printf("new throughput: %d\n", updated_t);
  stream_calc_throughput(s, ALPHA);
  printf("calc throughput: %d\n\n", s->throughput);

  /* Add another request */
  stream_add_request(s, request_init(s->cur_bitrate, "1.1.1.1", "300Seq1-Frag2"));
  sleep(1);
  request_complete(s->cur_request, 15000);

  diff = (int)(s->cur_request->end - s->cur_request->start);
  throughput = s->cur_request->chunksize/diff;
  updated_t = (ALPHA * throughput) + (1 - ALPHA) * s->throughput;
  printf("request 2..\n");
  printf("diff: %d\n", diff);
  printf("throughput: %d\n", throughput);
  printf("new throughput: %d\n", updated_t);
  stream_calc_throughput(s, ALPHA);
  printf("calc throughput: %d\n\n", s->throughput);

  stream_free(s);

  printf("All tests passed!\n");

  return 0;
}

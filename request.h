
#ifndef VP_REQUEST
#define VP_REQUEST

#include <stdlib.h>
#include <time.h>

#define IP_LEN  16
#define CHUNKNAME_LEN 256

typedef struct {
  time_t start;
  time_t end;
  int bitrate;
  float throughput;
  int seq_num;
  int frag_num;
  int chunksize;
} request;

request *request_init(int br, int seq, int frag);
void request_complete(request *r);
void request_free(request *r);

#endif

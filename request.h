
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
  int chunksize;
} request;

request *request_init(int br);
void request_complete(request *r, int chunksize);
void request_free(request *r);

#endif

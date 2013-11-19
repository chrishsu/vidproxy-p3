
#ifndef VP_REQUEST
#define VP_REQUEST

#include <time.h>

#define IP_LEN  16
#define CHUNKNAME_LEN 256

typedef struct {
  time_t start;
  time_t end;
  int throughput;
  int bitrate;
  char[IP_LEN] client_ip;
  int chunksize;
  char[CHUNKNAME_LEN] chunkname;
} request;

request *request_init(int br, char *ip, char *name);
void request_complete(request *r);
void request_free(request *r);

#endif
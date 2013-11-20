
#ifndef VP_REQUEST
#define VP_REQUEST

#include <time.h>

#define IP_LEN  16
#define CHUNKNAME_LEN 256

typedef struct {
  time_t start;
  time_t end;
  int bitrate;
  char client_ip[IP_LEN];
  int chunksize;
  char chunkname[CHUNKNAME_LEN];
} request;

request *request_init(int br, char *ip, char *name);
void request_complete(request *r, int chunksize);
void request_free(request *r);

#endif
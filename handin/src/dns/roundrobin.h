
#ifndef DNS_RR
#define DNS_RR

#include <stdio.h>
#include <stdlib.h>

#define IP_STRING_MAX 16

typedef struct server_loop {
  char ip[IP_STRING_MAX];
  struct server_loop *next;
} server_loop;

void rr_parse_servers(char *filename);
char *rr_next_server();
void rr_free();

#endif

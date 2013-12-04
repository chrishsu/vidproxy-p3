
#ifndef DNS_LOG
#define DNS_LOG

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void log_init(char *filename);
void log_print(char *client_ip, char *query_name, char *response_ip);

#endif

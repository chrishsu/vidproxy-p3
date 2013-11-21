
#ifndef VP_LOG
#define VP_LOG

#include <stdio.h>
#include <stdlib.h>
#include "request.h"

void log_init(char *filename);
void log_print(request *r, float avg_tpt, char *client_ip);

#endif

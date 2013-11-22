
#ifndef VP_LOG
#define VP_LOG

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stream.h"

void log_init(char *filename);
void log_print(stream *s, char *client_ip);

#endif

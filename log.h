
#ifndef VP_LOG
#define VP_LOG

#include "request.h"

void log_init(char *filename);
void log_print(request *r, int avg_tpt);

#endif
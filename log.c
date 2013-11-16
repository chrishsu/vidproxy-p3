#include "log.h"

char *vp_log_file;

/**
 * Sets up the file pointer.
 *
 * @param[in] filename  The file name.
 */
void log_init(char *filename) {
  vp_log_file = filename;
}

/**
 * Prints out the request and details.
 *
 * @param[in] r        The request.
 * @param[in] avg_tpt  The average throughput.
 */
void log_print(request *r, int avg_tpt) {
  if (vp_log_file == NULL || r == NULL) return;
  FILE *fp = fopen(vp_log_file, 'a');
  
  fprintf(fp, "%d %d %d %d %d %s %s",
          time(NULL), (int)(r->end - r->start),
          &(r->throughput), &avg_tpt,
          &(r->bitrate), r->client_ip, r->chunkname);
  
  fclose(fp);
}

#endif
#include "log.h"

char *vp_log_file;

/**
 * Sets up the file pointer.
 *
 * @param[in] filename  The file name.
 */
void log_init(char *filename) {
  vp_log_file = filename;
  FILE *fp = fopen(vp_log_file, "w");
  fclose(fp);
}

/**
 * Prints out the request and details.
 *
 * @param[in] s          The stream object.
 * @param[in] client_ip  The client IP request was from.
 */
void log_print(stream *s, char *client_ip) {
  if (vp_log_file == NULL || s == NULL) return;

  request *r = s->cur_request;
  if (r == NULL) return;

  FILE *fp = fopen(vp_log_file, "a");

  // Current Time, Duration, Throughput
  // Avg Throughput, Bitrate, Client IP
  // Chunkname
  fprintf(fp, "%d %f %f %f %d %s /vod/%dSeq%d-Frag%d\n",
          (int)time(NULL), r->time_diff, r->throughput,
          s->throughput, r->bitrate, client_ip,
          r->bitrate, r->seq_num, r->frag_num);

  fclose(fp);
}

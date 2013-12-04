#include "log.h"

char *dns_log_file;

/**
 * Sets up the file pointer.
 *
 * @param[in] filename  The file name.
 */
void log_init(char *filename) {
  dns_log_file = filename;
  FILE *fp = fopen(dns_log_file, "w");
  fclose(fp);
}

/**
 * Prints out the request and details.
 *
 * @param[in] client_ip    The client IP request was from.
 * @param[in] query_name   The hostname queried.
 * @param[in] response_ip  The IP returned.
 */
void log_print(char *client_ip, char *query_name, char *response_ip) {
  if (dns_log_file == NULL) return;

  FILE *fp = fopen(dns_log_file, "a");

  // Current Time, Client IP, Query Name, Response IP
  fprintf(fp, "%d %s %s %s\n", (int)time(NULL),
          client_ip, query_name, response_ip);

  fclose(fp);
}

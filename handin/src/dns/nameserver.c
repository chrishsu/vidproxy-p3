

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dns_utils.h"
#include "roundrobin.h"

#define MAX_FILENAME 128
#define MAX_UDP_BUF 512

typedef char flag;

flag roundrobin;
char logfile[MAX_FILENAME];
char listen_ip[INET_ADDRSTRLEN];
int listen_port;
char servers_file[MAX_FILENAME];
char lsas_file[MAX_FILENAME];

int read_inputs(int argc, char **argv) {
  int idx = 0;
  roundrobin = 0;
  if (argc == 7) {
    if (strcmp("-r", argv[1]) == 0) {
      roundrobin = 1;
    }
    idx = 1;
  }
  
  strcpy(logfile, argv[1+idx]);
  strcpy(listen_ip, argv[2+idx]);
  sscanf(argv[3+idx], "%d", &listen_port);
  
  strcpy(servers_file, argv[4+idx]);
  strcpy(lsas_file, argv[5+idx]);
  
  return 1;
}

void cleanup() {
  if (roundrobin) rr_free();
}

void send_udp(int sock, char *buf, int len, struct sockaddr_in *dest) {
  int bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr *)dest, sizeof(*(dest)));
  if (bytes_sent < 0) {
    fprintf(stderr, "Error sending..\n");
  }
}

void send_error_udp(int sock, struct sockaddr_in *dest, byte rcode) {
  dns_header *dh = dns_create_header(IS_ERROR, rcode);
  
  char *buf;
  int buflen = dns_make_buf(dh, NULL, NULL, buf);
  
  send_udp(sock, buf, buflen, dest);
  
  free(dh);
  free(buf);
}

void send_valid_udp(int sock, struct sockaddr_in * dest, char *name) {
  char ip[INET_ADDRSTRLEN];
  int server_ip;
  
  if (roundrobin) {
    ip = rr_next_server();
  } else {
    // LSA
  }
  
  inet_pton(AF_INET, ip, &server_ip);
  
  dns_header *dh = dns_create_header(IS_RESPONSE, R_OK);
  dns_answer *da = dns_create_answer(server_ip);
  
  char *buf;
  int buflen = dns_make_buf(dh, NULL, da, buf);
  
  send_udp(sock, buf, buflen, dest);
  
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(dest->sin_addr.s_addr), client_ip, INET_ADDRSTRLEN);
  log_print(client_ip , name, ip);
  
  free(da);
  free(dh);
  free(buf);
}

/**
 * Receives from the socket. If it is a valid header, calls send_udp.
 */
void process_udp(int sock) {
  struct sockaddr_in from;
  socklen_t fromlen;
  char buf[MAX_UDP_BUF];
  
  short bytes_read = recvfrom(sock, buf, MAX_UPD_BUF, 0, (struct sockaddr *) &from, &fromlen);
  
  if (bytes_read <= 0 ||
      bytes_read < (short) sizeof(dns_header)) {
    fprintf(stderr, "Error in process_udp!\n");
    return;
  }
  
  dns_header dh;
  dns_process_header(buf, &dh);
  
  if (ntohs(dh->qdcount) != 1) {
    send_error_udp(sock, &from, R_FORMAT);
    return;
  }
  
  dns_question dq;
  dns_process_query(buf + sizeof(dns_header), &dq);
  if (dns_is_valid(&dh, &dq)) {
    char *name = dns_query_name(dq);
    if (strcmp("video.cs.cmu.edu", name) == 0) {
      send_valid_udp(sock, &from, name);
    } else {
      send_error_udp(sock, &from, R_NAME);
    }
    free(name);
  } else {
    send_error_udp(sock, &from, R_FORMAT);
  }
}

void select_loop() {
  int sock;
  struct sockaddr_in myaddr;
  fd_set readfds;
  
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
    fprintf(stderr, "Could not create socket!\n");
    return;
  }
  
  bzero(&myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  inet_pton(AF_INET, listen_ip, &(myaddr.sin_addr));
  myaddr.sin_port = htons(listen_port);
  
  if (bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
    fprintf(stderr, "Could not bind socket!\n");
    return;
  }
  
  FD_ZERO(&readfds);
  
  while (1) {
    int sel_ret;
    
    FD_SET(sock, &readfds);
    
    sel_ret = select(sock+1, &readfds, NULL, NULL, NULL);
    
    if (sel_ret < 0) {
      printf("Select error: %d\n", errno);
      return;
    }
    if (sel_ret == 0) {
      printf("Select returned 0\n");
      break;
    }
    
    if (FD_ISSET(sock, &readfds)) {
      process_udp(sock);
    }
  }
}

int main(int argc, char **argv) {
  if (!read_inputs(argc, argv)) {
    return EXIT_FAILURE;
  }
  
  if (roundrobin) {
    rr_parse_servers(servers_file);
  } else {
    // LSA
  }
  
  select_loop();
  
  cleanup();
  
  return EXIT_SUCCESS;
}
#include "mydns.h"

#define MAX_UDP_BUF 512

char mydns_ip[INET_ADDRSTRLEN];
unsigned int mydns_port;
char mydns_fake_ip[INET_ADDRSTRLEN];

int init_mydns(const char *dns_ip, unsigned int dns_port, const char *fake_ip) {
  memcpy(mydns_ip, dns_ip, INET_ADDRSTRLEN);
  mydns_port = dns_port;
  memcpy(mydns_fake_ip, fake_ip, INET_ADDRSTRLEN);
  return 0;
}

int resolve_send(int mysock) {
  struct sockaddr_in dnsaddr;
  dnsaddr.sin_family = AF_INET;
  dnsaddr.sin_port = htons(mydns_port);
  inet_pton(AF_INET, mydns_ip, &dnsaddr.sin_addr.s_addr);

  dns_header *dh = dns_create_header(IS_QUERY, R_COK);
  dns_question *dq = dns_create_question();
  int len;
  char *buf = dns_make_buf(dh, dq, NULL, &len);

  int bytes_sent = sendto(mysock, buf, len, 0,
                          (struct sockaddr *)&dnsaddr,
                          sizeof(struct sockaddr_in));

  free(buf);
  free(dq);
  free(dh);

  if (bytes_sent < 0) {
    fprintf(stderr, "Error sending..\n");
    return -1;
  }
  return 0;
}

int resolve_recv(int mysock, struct sockaddr_in *myaddr, char *buf) {
  fd_set myfds;
  FD_ZERO(&myfds);
  FD_SET(mysock, &myfds);

  int sel_ret = select(mysock+1, &myfds, NULL, NULL, NULL);

  if (sel_ret < 0) {
    printf("Select error: \n");
    return -1;
  }
  if (sel_ret == 0) {
    printf("Select returned 0\n");
    return -1;
  }

  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  short bytes_read = recvfrom(mysock, buf, MAX_UDP_BUF, 0, (struct sockaddr *) &from, &fromlen);

  if (bytes_read <= 0 ||
      bytes_read < (short) sizeof(dns_header)) {
    fprintf(stderr, "Error in process_udp!\n");
    return -1;
  }

  return (int)bytes_read;
}

#define DNS_Q_SIZE 22

int resolve(const char *node, const char *service,
            const struct addrinfo *hints, struct addrinfo **res) {
  int mysock;
  struct sockaddr_in myaddr;

  if ((mysock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
    fprintf(stderr, "Could not create socket!\n");
    return -1;
  }

  bzero(&myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  inet_pton(AF_INET, mydns_fake_ip, &(myaddr.sin_addr));
  myaddr.sin_port = 0;

  if (bind(mysock, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
    fprintf(stderr, "Could not bind socket!\n");
    return -1;
  }

  if (resolve_send(mysock) == -1) {
    return -1;
  }

  char buf[MAX_UDP_BUF];
  int buflen;
  if ((buflen = resolve_recv(mysock, &myaddr, buf)) == -1) {
    return -1;
  }

  dns_answer da;
  dns_process_answer(buf + sizeof(dns_header) + DNS_Q_SIZE,
                     buflen - sizeof(dns_header) - DNS_Q_SIZE, &da);

  int port;
  sscanf(service, "%d", &port);

  *res = malloc(sizeof(struct addrinfo));

  (*res)->ai_flags = 0;
  (*res)->ai_family = AF_INET;
  (*res)->ai_socktype = SOCK_STREAM;
  (*res)->ai_protocol = IPPROTO_TCP;
  (*res)->ai_addrlen = sizeof(struct sockaddr);
  struct sockaddr_in *addr = malloc(sizeof(struct sockaddr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = (unsigned long)da.rdata;
  (*res)->ai_addr = (struct sockaddr *)addr;
  (*res)->ai_canonname = NULL;

  return 0;
}

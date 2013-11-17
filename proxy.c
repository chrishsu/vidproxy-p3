/**
 * @file proxy.c
 * @section Description
 * The proxy file
 */
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <netdb.h>

#include "stream.h"
#include "parse_utils.h"

#define MAX_FDS 1023 ///// Is this okay?
#define MAX_REQ_SIZE 8192
#define MAX_FILENAME 128
#define IP_STRING_MAX 25 // Extra large to avoid overflow in case of mistake
#define MANIFEST_MAX 8000
#define CLIENTTYPE 0
#define SERVERTYPE 1

// Arguments:
char logfile[MAX_FILENAME];
float alpha;
int listen_port;
char fakeip[IP_STRING_MAX];
char dnsip[IP_STRING_MAX];
int dnsport;
char wwwip[IP_STRING_MAX];

// The listening port and socket:
int port, sock;
int manifestsock; // The one we use to initially get the manifest
char gotmanifest = 0; // 1 if we got it

// The amount of data left before
// we finish a request
int data_left = 0;

// FD sets:
fd_set readfds, writefds, exceptfds;

// Must be larger because we add a null character
// and we make the requests slightly longer:
char bufs[MAX_FDS + 1][MAX_REQ_SIZE + 25];
int buflens[MAX_FDS + 1];
char types[MAX_FDS + 1]; // client or server, see type defined above
int server_socks[MAX_FDS + 1]; // server socket for the indexed client socket
int client_socks[MAX_FDS + 1]; // client socket for the indexed server socket
char ip[MAX_FDS + 1][INET_ADDRSTRLEN + 1];

char manifest[MANIFEST_MAX];
int manifestlen = 0;

stream *ss = NULL;
bitrate_list *brlist = NULL;

void clean_up() {
  close(sock);

  ///// Close other sockets
}

void signal_handler(int sig) {
  clean_up();
}

int add_client() {
  struct sockaddr_in cli_addr;
  socklen_t cli_size = sizeof(cli_addr);
  int client_sock;
  if ((client_sock = accept(sock, (struct sockaddr *)&cli_addr,
			    &cli_size)) == -1) {
    fprintf(stderr, "Error accepting connection.\n");
    return -1;
  }
  if (client_sock > MAX_FDS) {
    fprintf(stderr, "Socket number too large.\n");
    return -1;
  }

  struct addrinfo fakehints, *fakeinfo;
  memset(&fakehints, 0, sizeof(fakehints));
  fakehints.ai_family = AF_INET;
  fakehints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(fakeip, "0", &fakehints, &fakeinfo)) {
    fprintf(stderr, "Error getting fake server address info!\n");
    close(client_sock);
    return -1;
  }
  struct addrinfo servhints, *servinfo;
  memset(&servhints, 0, sizeof(servhints));
  servhints.ai_family = AF_INET;
  servhints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(wwwip, "8080", &servhints, &servinfo)) {
    fprintf(stderr, "Error getting real server address info!\n");
    freeaddrinfo(fakeinfo);
    close(client_sock);
    return -1;
  }

  // Try the first address returned:
  int serv_sock;
  if ((serv_sock = socket(fakeinfo->ai_family, fakeinfo->ai_socktype, fakeinfo->ai_protocol)) == -1) {
    fprintf(stderr, "Error creating server socket!\n");
    freeaddrinfo(fakeinfo);
    freeaddrinfo(servinfo);
    close(client_sock);
    return -1;
  }

  if (bind(serv_sock, (struct sockaddr *)fakeinfo->ai_addr, fakeinfo->ai_addrlen)) {
    fprintf(stderr, "Error binding server socket: %d!\n", errno);
    close(client_sock);
    close(serv_sock);
    return -1;
  }
  if (connect(serv_sock, (struct sockaddr *)servinfo->ai_addr, servinfo->ai_addrlen)) {
    fprintf(stderr, "Error connecting to server socket!\n");
    freeaddrinfo(fakeinfo);
    freeaddrinfo(servinfo);
    close(client_sock);
    close(serv_sock);
    return -1;
  }

  buflens[client_sock] = 0;
  types[client_sock] = CLIENTTYPE;
  types[serv_sock] = SERVERTYPE;
  server_socks[client_sock] = serv_sock;
  client_socks[serv_sock] = client_sock;
  FD_SET(client_sock, &readfds);
  FD_SET(serv_sock, &readfds);

  freeaddrinfo(fakeinfo);
  freeaddrinfo(servinfo);

  if (!inet_ntop(AF_INET, &cli_addr, ip[client_sock], INET_ADDRSTRLEN)) {
    fprintf(stderr, "Error getting IP address!\n");
    ip[client_sock][0] = 0; // Make it empty
  }
  printf("Added client!\n");
  return 0;
}

int request_manifest() {
  struct addrinfo fakehints, *fakeinfo;
  memset(&fakehints, 0, sizeof(fakehints));
  fakehints.ai_family = AF_INET;
  fakehints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(fakeip, "0", &fakehints, &fakeinfo)) {
    fprintf(stderr, "Error getting manifest fake server address info!\n");
    return -1;
  }
  struct addrinfo servhints, *servinfo;
  memset(&servhints, 0, sizeof(servhints));
  servhints.ai_family = AF_INET;
  servhints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(wwwip, "8080", &servhints, &servinfo)) {
    fprintf(stderr, "Error getting manifest real server address info!\n");
    freeaddrinfo(fakeinfo);
     return -1;
  }

  // Try the first address returned:
  int serv_sock;
  if ((serv_sock = socket(fakeinfo->ai_family, fakeinfo->ai_socktype, fakeinfo->ai_protocol)) == -1) {
    fprintf(stderr, "Error creating manifest server socket!\n");
    freeaddrinfo(fakeinfo);
    freeaddrinfo(servinfo);
    return -1;
  }

  if (bind(serv_sock, (struct sockaddr *)fakeinfo->ai_addr, fakeinfo->ai_addrlen)) {
    fprintf(stderr, "Error binding manifest server socket: %d!\n", errno);
    close(serv_sock);
    return -1;
  }
  if (connect(serv_sock, (struct sockaddr *)servinfo->ai_addr, servinfo->ai_addrlen)) {
    fprintf(stderr, "Error connecting to manifest server socket!\n");
    freeaddrinfo(fakeinfo);
    freeaddrinfo(servinfo);
    close(serv_sock);
    return -1;
  }

  manifestsock = serv_sock;
  types[manifestsock] = SERVERTYPE;
  FD_SET(manifestsock, &readfds);

  freeaddrinfo(fakeinfo);
  freeaddrinfo(servinfo);
  printf("Requested manifest!\n");
  return 0;
}

void process_request(int server) {
  int bit_rate, seq, frag;
  if (parse_uri(bufs[server], buflens[server], &bit_rate, &seq, &frag)) {
    int rate = bitrate_list_select(brlist, ss->throughput);
    replace_uri(bufs[server], buflens[server], rate);
    stream_add_request(ss, request_init(rate, seq, frag));
  }
  if (parse_f4m(bufs[server], buflens[server])) {
    replace_f4m(bufs[server], buflens[server]);
  }
}

void process_data(int client) {
  int len;
  if (parse_headers(bufs[client], buflens[client], &len)) {
    data_left = len;
    stream_request_chunksize(ss, len);
  }
  if (data_left <= 0)
    return; // We're being sloppy, so we don't want to finish twice

  data_left -= buflens[client];
  if (data_left <= 0) { // This data made us go below 0
    stream_request_complete(ss);
    stream_calc_throughput(ss, alpha);
    
    log_print(ss, ip[client]);
  }
}

void remove_client(int client_sock) {
  FD_CLR(client_sock, &readfds);
  FD_CLR(server_socks[client_sock], &readfds);
  close(client_sock);
  close(server_socks[client_sock]);
}

void remove_server(int server_sock) {
  FD_CLR(server_sock, &readfds);
  FD_CLR(client_socks[server_sock], &readfds);
  close(server_sock);
  close(client_socks[server_sock]);
}

int complement_sock(int sock) {
  if (types[sock] == CLIENTTYPE)
    return server_socks[sock];
  return client_socks[sock];
}

int main(int argc, char* argv[])
{
  signal(SIGINT, signal_handler);
  int i;
  for (i = 0; i <= MAX_FDS; i++)
    buflens[i] = 0;

  strcpy(logfile, argv[1]);
  sscanf(argv[2], "%f", &alpha);
  sscanf(argv[3], "%d", &listen_port);
  strcpy(fakeip, argv[4]);
  strcpy(dnsip, argv[5]);
  sscanf(argv[6], "%d", &dnsport);
  if (argc > 7)
    strcpy(wwwip, argv[7]);

  log_init(logfile);

  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Failed creating socket.\n");
    return EXIT_FAILURE;
  }

  struct sockaddr_in listen_addr;
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons(listen_port);
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr))) {
    fprintf(stderr, "Error binding to listening socket!\n");
    close(sock);
    return EXIT_FAILURE;
  }

  if (listen(sock, 5)) {
    fprintf(stderr, "Error listening!\n");
    close(sock);
    return EXIT_FAILURE;
  }
  FD_SET(sock, &readfds);

  // Request the manifest from the server:
  request_manifest();

  printf("Entering select loop!\n");

  fd_set readfdscopy, writefdscopy;
  while (1) {
    // Copy fd sets:
    FD_ZERO(&readfdscopy);
    FD_ZERO(&writefdscopy);
    int i;
    for (i = 0; i <= MAX_FDS; i++) {
      if (i == sock) {
	FD_SET(i, &readfdscopy);
	continue;
      }
      // We'll accept connections, but we won't read or write
      // to anything other than the manifest socket until
      // we get the manifest:
      if (!gotmanifest) {
	FD_SET(manifestsock, &readfdscopy);
	FD_SET(manifestsock, &writefdscopy);
	continue;
      }

      // Decide if we actually want to read/write from this socket:
      if (FD_ISSET(i, &readfds) && 
	  buflens[complement_sock(i)] == 0) {
	FD_SET(i, &readfdscopy);
      }
      if (FD_ISSET(i, &writefds) || 
          buflens[i] > 0) { // Write if there's stuff left to write
	FD_SET(i, &writefdscopy);
      }
    }

    int select_return = select(MAX_FDS, &readfdscopy, &writefdscopy, &exceptfds, NULL);
    if (select_return < 0) {
      printf("Select error: %d\n", errno);
      if (errno == EINTR)
	clean_up();
      return EXIT_FAILURE;
    }
    if (select_return == 0) {
      printf("Select returned 0\n");
      break;
    }

    for (i = 0; i <= MAX_FDS; i++) {
      /* The listening socket is handled separately. */
      if (i == sock)
	continue;

      if (i == manifestsock) {
	int bytes_read = recv(i, manifest + manifestlen, 
			      MANIFEST_MAX - manifestlen, 0);
	if (bytes_read <= 0) {
	  if (bytes_read < 0) {
	    fprintf(stderr, "Error reading from manifest socket %d\n", i);
	  }
	  return EXIT_FAILURE;
	} else { // read something
	  ///// Is this safe?
	  gotmanifest = 1;
	  brlist = parse_xml(manifest, manifestlen);
	  ss = stream_init(bitrate_list_select(brlist, 0.0));
	}
      }

      // Reading:
      if (FD_ISSET(i, &readfdscopy)) {
	int complement_sock;
	if (types[i] == CLIENTTYPE)
	  complement_sock = server_socks[i];
	else
	  complement_sock = client_socks[i];
	int bytes_read = recv(i, bufs[complement_sock], MAX_REQ_SIZE, 0);
	if (bytes_read <= 0) {
	  if (bytes_read < 0) {
	    fprintf(stderr, "Error reading from client socket %d\n", i);
	  }
	  if (types[i] == CLIENTTYPE)
	    remove_client(i);
	  else
	    remove_server(i);
	} else { // read something from i for complement_sock
	  // If we already read other stuff then we shouldn't be here:
	  buflens[complement_sock] = bytes_read;
	  // Set last character to null:
	  bufs[complement_sock][bytes_read] = 0;
	  if (types[i] == CLIENTTYPE) // Sent from the client
	    process_request(complement_sock); // complement is the server
	  else
	    process_data(complement_sock); // complement is the client
	}
      }
    }

    for (i = 0; i <= MAX_FDS; i++) {
      if (i == sock)
	continue;

      // Writing:
      if (FD_ISSET(i, &writefdscopy)) {
	int bytes_sent = send(i, bufs[i], buflens[i], 0);
	if (bytes_sent <= 0) {
	  if (bytes_sent < 0) {
	    fprintf(stderr, "Error sending to %d!\n", i);
	  }
          if (types[i] == CLIENTTYPE)
            remove_client(i);
          else
	    remove_server(i);
	} else { // sent something
	  buflens[i] -= bytes_sent;
	  memcpy(bufs[i], bufs[i] + bytes_sent, buflens[i]);
	}
      }
    }

    if (FD_ISSET(sock, &readfdscopy)) {
      add_client();
    }
  }

  clean_up();

  return EXIT_SUCCESS;
}

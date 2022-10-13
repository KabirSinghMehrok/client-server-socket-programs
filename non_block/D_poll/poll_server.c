#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CONNECTIONS 100
#define MAX_CLIENTS 10;
#define MAX_REQUESTS_CLIENT 20

int requests_read[MAX_CONNECTIONS];
int total_requests = 10 * MAX_REQUESTS_CLIENT;

void error(char* msg)
{
  perror(msg);
  exit(1);
}

uint64_t factorial(int n)
{
  uint64_t fact = 1;
  for (int i = 1; i <= n; ++i) {
    fact *= (uint64_t) i;
  }

  return fact;
}

int create_connection()
{
  int socketfd;
  struct sockaddr_in server_address, client_address;
  bzero((char*) &server_address, sizeof(server_address));

  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);

  int bind_val = bind(socketfd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
  if (bind_val < 0) error("ERROR: While binding");

  int listen_val = listen(socketfd, 10);
  if (listen_val < 0) 
  {
    close(socketfd);
    error("ERROR: While listening");
  }

  return socketfd;
}

int read_write_connection(int socket, FILE* fd)
{
  // returns 1 if socket needs to be removed, else 0

  // recieve int from client
  int server_rec_int = 0;
  // printf("\nReading\n");
  int read_n = read(socket, &server_rec_int, sizeof(server_rec_int));
  if (read_n < 0) error("ERROR: While reading");

  // compute factorial
  uint64_t server_sen_int = factorial(ntohl(server_rec_int));

  // send computed int to client
  // printf("Sending to client\n");
  int write_n = write(socket, &server_sen_int, sizeof(server_sen_int));
  // printf("aa: %d\n", socket);
  if (write_n < 0) error("ERROR: Writing the socket"); 
  
  // close the socket once 20 requests are read
  requests_read[socket]++;
  if (requests_read[socket] > MAX_REQUESTS_CLIENT) return 1;

  fprintf(fd, "received -> %d | fact(received) -> %lu \n", ntohl(server_rec_int), (unsigned long) server_sen_int);
  total_requests--;
  return 0;
}

int main(int argc, char* argv[])
{
  clock_t begin = clock();

  // declare
  FILE* fd;
  int clients_remaining = MAX_CLIENTS;
  struct sockaddr_in client_address;
  int client_addr_len = sizeof(client_address);

  // open
  fd = fopen("poll_out.txt", "w+");
  if (fd == NULL) error("ERROR: While opening file");

  // create socket server fd
  int server_fd = create_connection();
  if (server_fd < 0) error("ERROR: Failed to create server");

  // initialize max_requests
  for (int i=0; i<MAX_CONNECTIONS; i++) requests_read[i] = 0;

  // initializd poll specific structures
  int pollfd_size = MAX_CLIENTS + 1;
  struct pollfd pollfds[pollfd_size];
  pollfds[0].fd = server_fd;
  pollfds[0].events = POLLIN;
  int useClient = 0;

  // initialize pollfds for clients
  for (int i=1; i<pollfd_size; i++) pollfds[i].fd = -1;

  while(total_requests > 0)
  {
    // run poll on pollfds to see which socket is ready
    if (poll(pollfds, useClient + 1, 5000) < 0) error("ERROR: Polling fail");
    
    if (pollfds[0].revents & POLLIN)
    {
      // accept new connection
      int new_socket = accept(server_fd, (struct sockaddr *) &client_address, &client_addr_len);
      if (new_socket < 0) error("ERROR: While accepting new connection");

      // print client info into socket
      char str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &client_address.sin_addr, str, sizeof(str));
      fprintf(fd, "Connected to client addr&port -> %s:%d\n", str, htons(client_address.sin_port));

      // add new connection to pollfds
      useClient++;
      pollfds[useClient].fd = new_socket;
      pollfds[useClient].events = POLLIN;
    }

    // we first check if there is any new connection using server_fd
    // if there is, accept it
    // else, we iterate over pollfds to find if any non-server_fd socket is ready
    for (int socket=1; socket < pollfd_size + 1; socket++)
    {
      if (pollfds[socket].fd > 0 && pollfds[socket].revents & POLLIN)
      {
        // read and write to socket
        int socket_fd = pollfds[socket].fd;
        int remove_socket = read_write_connection(socket_fd, fd);
        if (remove_socket)
        {
          // remove socket from pollfds
          close(socket_fd);
          pollfds[socket].fd = -1;
          clients_remaining--;
        }
      }
    } 

    // printf("Capacity: %d | Clients: %d\n", total_requests, clients_remaining);

  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Time taken: %f\n", time_spent);

  close(server_fd);
  return 0;
}
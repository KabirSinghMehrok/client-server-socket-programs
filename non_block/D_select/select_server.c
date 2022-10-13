#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
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
  fd_set current_sockets, backup_sockets; // declarations for select

  // open
  fd = fopen("select_out.txt", "w+");
  if (fd == NULL) error("ERROR: While opening file");

  // create socket server fd
  int server_fd = create_connection();
  if (server_fd < 0) error("ERROR: Failed to create server");

  // initialize select sets
  FD_ZERO(&current_sockets);
  FD_SET(server_fd, &current_sockets);

  // initialize max_requests
  for (int i=0; i<MAX_CONNECTIONS; i++) requests_read[i] = 0;

  while(total_requests > 0)
  {
    backup_sockets = current_sockets;
    
    // run select on backup_socket to generate list of readable ports in backup_sockets
    if (select(FD_SETSIZE, &backup_sockets, NULL, NULL, NULL) < 0) error("ERROR: Select fail");
    
    // [assumption] we can't iterate over fdset
    // so we iterate from 0 to fd_setsize using i
    // we check if i is in backup_sockets (i.e. readable)
    // if it is, then we read it (ready only one send request)
    for (int socket=0; socket < FD_SETSIZE; socket++)
    {
      if (FD_ISSET(socket, &backup_sockets))
      {
        if (socket == server_fd) // if server socket, generate new connection
        {
          int new_socket = accept(server_fd, (struct sockaddr *) &client_address, &client_addr_len);
          
          // print client info
          char str[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &client_address.sin_addr, str, sizeof(str));
          fprintf(fd, "Connected to client addr&port -> %s:%d\n", str, htons(client_address.sin_port));

          FD_SET(new_socket, &current_sockets);
        }
        else
        {
          int remove_socket = read_write_connection(socket, fd);
          if (remove_socket)
          {
            clients_remaining--;
            // printf("\n\n %d client remaining \n\n", clients_remaining);
            if (close(socket) < 0) 
            {
              printf("ERROR socket no: %d", socket);
              error("ERROR: unable to close the required socket");
            }
            FD_CLR(socket, &current_sockets);
            // printf("Clear successful\n");
          } 
        }
      }
    } 

    // printf("%d is the total request capacity remaining", total_requests);
  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Time taken: %f\n", time_spent);

  close(server_fd);
  return 0;
}
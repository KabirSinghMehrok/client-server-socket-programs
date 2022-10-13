#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080

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

int main(int argc, char* argv[])
{
  clock_t begin = clock();

  // declarations
  FILE* fd;
  int socketfd, newsocketfd, portno;
  struct sockaddr_in server_address, client_address; 
  portno = (int) PORT;
  bzero((char*) &server_address, sizeof(server_address));

  // create a socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);

  // store server address and port no
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portno);

  // bind the socket to port
  int bind_val = bind(socketfd, (struct sockaddr *) &server_address, sizeof(server_address));
  if (bind_val < 0) error("ERROR: While binding");

  // listen for 
  int listen_val = listen(socketfd, 5);
  if (listen_val < 0) error("ERROR: While listening");

  fd = fopen("fork_out.txt", "a");
  if (fd == NULL) error("ERROR: While opening file");

  int client_no = 10;
  while (client_no > 0)
  {
    int client_len = sizeof(client_address);
    newsocketfd = accept(socketfd, (struct sockaddr *) &client_address, &client_len);
    if (newsocketfd < 0)
    {
      fprintf(stderr, "ERROR: While accepting client no : %d", (int) client_no);
      exit(1);
    } 

    if (!fork()) // ignore if the fork is unsuccessful
    {
      // close(socketfd); // close the parent socket in child process, since its work is done
      // while(1)
      // {
        int to_send = 20;
        for (int i=0; i<to_send; i++)
        {
          char str[INET_ADDRSTRLEN];
          inet_ntop(AF_INET,&client_address.sin_addr, str, sizeof(str));

          // recieve int from client
          int server_rec_int = 0;
          int read_n = read(newsocketfd, &server_rec_int, sizeof(server_rec_int));
          if (read_n < 0) error("ERROR: While reading");

          // computer factorial
          uint64_t server_sen_int = factorial(ntohl(server_rec_int));

          // send computed int to client
          int write_n = write(newsocketfd, &server_sen_int, sizeof(server_sen_int));
          if (write_n < 0) error("ERROR: Writing the socket"); 

          fprintf(fd, "client addr&port -> %s:%d | received -> %d | fact(received) -> %lu \n", str, 
          htons(client_address.sin_port), ntohl(server_rec_int), (unsigned long) server_sen_int);
        }
      // }
      
      close(newsocketfd);
      exit(0);
    }
    
    close(newsocketfd);
    client_no--;
  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Time taken: %f\n", time_spent);

  close(socketfd);
  return 0;
}
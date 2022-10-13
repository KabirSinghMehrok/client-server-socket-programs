#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define PORT 8080

void error(char *msg)
{
  perror(msg);
  exit(0);
}

void* process()
{
  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  portno = (int) PORT;
  char buffer[256];
  bzero((char *) &serv_addr, sizeof(serv_addr));
  bzero(buffer,256);
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");

  server = gethostbyname("localhost");
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  // printinf for checking
  // printf("%d %s %s\n", sockfd, g_argv[1], g_argv[2]);

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

  int to_send = 20;
  for (int i=0; i<to_send; i++)
  {
    // send int to client
    int client_sen_int = htonl(i+1);
    int write_n = write(sockfd, &client_sen_int, sizeof(client_sen_int));
    if (write_n < 0) error("ERROR writing to socket");
    
    // receive factorial of int from client
    uint64_t client_rec_int = 0;
    int read_n = read(sockfd, &client_rec_int, sizeof(client_rec_int));
    if (read_n < 0) 
    {
      error("ERROR reading from socket");
      printf("\nThe value is %lu\n", pthread_self());
    }
    printf("Client process: %lu | sent: %d   factorial(sent): %lu\n", pthread_self(), i+1, (unsigned long) client_rec_int);
  }

  close(sockfd);
  return NULL;
}

int main(int argc, char *argv[])
{
  int thread_no = 10;
  pthread_t threads[thread_no];

  for (int i=0; i<thread_no; i++)
  {
    int thread_sep = pthread_create(&threads[i], NULL, process, NULL);
    if (thread_sep < 0) error("ERROR: Creating client thread not working");
  }

  for (int i=0; i<thread_no; i++)
  {
    int thread_join = pthread_join(threads[i], NULL);
    if (thread_join < 0) error("ERROR: Joining client thread not working");
  }
  
  return 0;
}
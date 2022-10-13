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
int total_request_sent;
pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;

void error(char *msg)
{
  perror(msg);
  exit(0);
}

void* process()
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  bzero((char *) &serv_addr, sizeof(serv_addr));
  bzero(buffer,256);
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");

  // printf("1");

  server = gethostbyname("localhost");
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  // printf("2");

  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(PORT);

  // printf("3");

  if (connect(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR connecting");

  int to_send = 20;
  for (int i=0; i<to_send; i++)
  {
    // printf("4");

    // send int to client
    int client_sen_int = htonl(i+1);
    int write_n = write(sockfd, &client_sen_int, sizeof(client_sen_int));
    if (write_n < 0) error("ERROR writing to socket");
    
    // receive factorial of int from client
    uint64_t client_rec_int = 0;
    int read_n = read(sockfd, &client_rec_int, sizeof(client_rec_int));
    if (read_n < 0) 
    {
      // printf("\nThe value is %lu\n", pthread_self());
      error("ERROR reading from socket");
    }
    printf("Client process: %lu | sent: %d   factorial(sent): %lu\n", pthread_self(), i+1, (unsigned long) client_rec_int);

    // print the total requests recieved
    pthread_mutex_lock(&a_lock);
    total_request_sent++;
    // printf("%d : total requests processed till now\n", total_request_sent);
    pthread_mutex_unlock(&a_lock);
  }

  close(sockfd);
  return NULL;
}

int main(int argc, char *argv[])
{
  total_request_sent = 0;

  int thread_no = 10;
  pthread_t threads[thread_no];

  for (int i=0; i<thread_no; i++)
  {
    int thread_sep = pthread_create(&threads[i], NULL, process, NULL);
    if (thread_sep < 0) error("ERROR: Creating client thread not working");
  }

  // printf("thread splitting has finished\n");

  for (int i=0; i<thread_no; i++)
  {
    int thread_join = pthread_join(threads[i], NULL);
    if (thread_join < 0) error("ERROR: Joining client thread not working");
  }
  
  // printf("%d is the total request sent\n", total_request_sent);
  return 0;
}

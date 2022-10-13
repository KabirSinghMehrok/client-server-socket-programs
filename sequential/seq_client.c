#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
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
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

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
      if (read_n < 0) error("ERROR reading from socket");
      printf("Client | sent: %d   factorial(sent): %lu\n", i+1, (unsigned long) client_rec_int);
    }

    close(sockfd);
    return 0;
}
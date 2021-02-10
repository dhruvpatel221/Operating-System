
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX 1000

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    char b[256],in[MAX];
    char str[50];
    char* token;
    int k,i = 0;
    char *p[50];


    printf("Please enter the command ");
      bzero(buffer,256);
      bzero(b,256);
      bzero(in,MAX);

      fgets(buffer,256,stdin);
      strcpy(b,buffer);
      token = strtok(buffer," ");
      while (token != NULL) {
          p[i++] = token;
         token =  strtok(NULL, "\n,' ' ");

    }
    //printf("%s : Buffer\n",x);
    portno = atoi(p[i-1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    n = write(sockfd,b,strlen(b));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,in,MAX);
    if (n < 0)
         error("ERROR reading from socket");
      printf("%s\n",in);
    close(sockfd);
    return 0;
}

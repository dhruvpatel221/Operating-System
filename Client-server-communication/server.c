/* Program: server.c
 * A simple server TCP sockets.
 * Server is executed before Client.
 * Port number is to be passed as an argument.
 *
 * To test: Open a terminal window.
 * At the prompt ($ is my prompt symbol) you may
 * type the following as a test:
 *
 * $ ./server 54554
 * Run client by providing host and port
 *
 *
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

void g(char* buffer){
  printf("%s", buffer);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void countf(char *c, char *name){

FILE *fp1, *fp2;
char *d;

int count=0;
  printf("%s\n",name );
   fp1 = fopen(name, "r");
   if (fp1 == NULL) {
      printf("cannot open this file /n");
      exit(1);
   }

   fp2 = fopen("filecount.txt", "w");
   if (fp2 == NULL) {
      printf("cannot open this file /n");
      exit(1);
   }

   do {
       *d = fgetc(fp1);
       if(strcmp(d,c)==0){
   		count++;

    }
  } while (*d != EOF);
   fprintf(fp2, "%d", count);


 printf("Number of characater is %d \n",count);

   fclose(fp1);
   fclose(fp2);
}
void toUpper(char* name){

 FILE *fp1, *fp2;
 char d,data;


 printf("%s\n",name );

   fp1 = fopen(name, "r");
   if (fp1 == NULL) {
      printf("cannot open this file /n");
      fclose(fp1);
      exit(1);
   }
   printf("rea");

   fp2 = fopen("fileUpper.txt", "w");


  do {
      d= fgetc(fp1);
      data = toupper(d);
      fputc(data, fp2);
   } while (data != EOF);
   printf("Uppercase in to fileUpper.txt  \n");
   fclose(fp1);
   fclose(fp2);
}

void getdata(char* buffer) {


  char* token;
  int i = 0;
  char c;
  char *filename;
   char* p[3];
  token = strtok(buffer," ");
  while (token != NULL) {
    if(i == 1){
     filename = token;
    }
        p[i++] = token;
       token = strtok(NULL, "\n,' '");

    }

    if(strcmp("toUpper",p[0])==0){
        printf("reached Uppercase function \n");
        toUpper(filename);
  }
    else if (strcmp("count",p[0])==0){
    printf("reached count function \n");

    countf(p[2], filename);
}
else
{
  printf("function not avaibale \n");
}
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     fprintf(stdout, "Run client by providing host and port\n");
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
     if (newsockfd < 0)
        error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0)
        error("ERROR reading from socket");
     printf("Here is the command  %s\n",buffer);
     getdata(buffer);
     n = write(newsockfd,"I got your command ",18);
     if (n < 0)
        error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     return 0;
}

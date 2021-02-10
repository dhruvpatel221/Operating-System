
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define MAX 1000
//Variable for custom shell
char host[MAX];
char user[MAX];
char line[MAX];
char execpath[MAX];
char execdir[MAX];
int F_IN=0;
int F_OUT=1;
int CMDCTR=1;
int PIPEIN=0;
pid_t SHELLPID;

//functions prototype for custom shell
void tokenize( char *line , char **tokens , char* delimiter);
void deletecmd(int cmdpid);
void checkINoperator(char *tokens);
void checkOUToperator(char *tokens);
int check_bg(char ** tokens);
void commandExe();
int check_space();

typedef struct cmds
{
    char name[100];
    pid_t pid;
}cmds;
cmds cmd[MAX];


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int getdata(char* b) {
  char *p[50];
  char* token;
  int i,j = 0;
  int k;
  char command[256];
  bzero(command,256);
  token = strtok(b," ");  // tokenizing string
  while (token != NULL) {
      p[i++] = token;
     token =  strtok(NULL, "\n,' ' ");
}

for( j=0; j<i-2; j++){   // seperating command form line
    strcat(command,p[j]);
    strcat(command," ");
}

if(strcmp(p[i-2],">@")==0){ // for command run on server side and output displayed on server side
  k=1;
}

else if(strcmp(p[i-2],"<@")==0){ // for command run on server side and output displayed on client side
  k=0;
}
else{ // for invalid command
  k=2;
}
  printf(" Shell Command : %s \n",command);
  if(k==1){ // for command run on server side and output displayed on server side


    SHELLPID = getpid();
    getcwd(execdir,MAX);
    line[0] = '\0';
    strcpy(line,command);
    commandExe(); //executing commands
}
else if(k==0){ // for command run on server side and output displayed on client side

  strcat(command," > text.txt");
  SHELLPID = getpid();
  getcwd(execdir,MAX);
  line[0] = '\0';
  strcpy(line,command);
  commandExe();
  printf(" Output of Command to client-side \n");
}
else{ // for invalid command
  printf("Invalid NOT Found '<@' & '>@'command ");
 }
return k;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     char b[256];
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
     bzero(b,256);
     n = read(newsockfd,b,255);
     if (n < 0)
        error("ERROR reading from socket");
     printf("Here is the command  %s\n",b);
     int k = getdata(b);
     if(k==1){
       n = write(newsockfd,"command run on server side",256);
     }
     else if(k==0) {
       char buff[MAX];
       bzero(buff,MAX );
       FILE *f = fopen("./text.txt", "rb");
       fseek(f, 0, SEEK_END);
       long fsize = ftell(f);
       fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

       char *string = malloc(fsize + 1);
       fread(string, 1, fsize, f);
       fclose(f);

       string[fsize] = 0;
       strcat(buff, string);
       n = write(newsockfd,buff,MAX);
     }
     else{
     n = write(newsockfd,"Invalid NOT Found '<@' & '>@'command ",256);
    }
     if (n < 0)
        error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     return 0;
}

//Functions for custom shell
// Used to tokenize lines into commands
void tokenize( char *line , char **tokens , char* delimiter)
{
    char *temp;
    int i = 0;
    //Splitting line by delimiter and saving it into temp
    temp = strtok( line , delimiter );
    while( temp != NULL )
    {
        tokens[i++] = temp;
        temp = strtok( NULL , delimiter );
    }
}

void deletecmd(int cmdpid)
{
    int i;
    int flag=0;
    for(i=1;i<CMDCTR;i++)
    {
        if( cmd[i].pid == cmdpid )
            flag=1;

        if(flag==1)
            cmd[i]=cmd[i+1];
    }
}

//Function to call shell

void checkINoperator(char *tokens)
{
    int i=0;
    char *temp[100]={NULL} ;
    tokenize( tokens , temp , "<" );

    if(temp[1]!=NULL)
    {
        char *temp1[100]={NULL};
        tokenize( temp[1] , temp1 , " " );
        F_IN = open(temp1[0],O_RDONLY);
    }
}

void checkOUToperator(char *tokens)
{
    int i=0;
    char *temp[100]={NULL} ;
    tokenize( tokens , temp , ">" );

    if(temp[1]!=NULL)
    {
        char *temp1[100]={NULL};
        tokenize( temp[1] , temp1 , " " );
        F_OUT = open(temp1[0],O_TRUNC | O_WRONLY | O_CREAT, S_IRWXU);
    }
    else
        F_OUT=1;
}

int check_bg(char ** tokens)
{
    int i=0;
    while(tokens[i]!=NULL)
    {
        if(strcmp(tokens[i],"&")==0)
        {
            tokens[i]=NULL;
            return 1;
        }
        i++;
    }
    return 0;
}

//Executes all the commands in a single line
void commandExe()
{
    char *temp[100]={NULL};
    char *temppipe[100]={NULL};

    // Checking if there are multiple commands seperated by ;
    tokenize( line , temppipe , ";" );
    int j=0;
    while(temppipe[j]!=NULL)
    {
        tokenize( temppipe[j] , temp , "|" );
        int i = 0;
        int fd[2];
        while( temp[i] != NULL )
        {
            char *tokens[100] ={NULL};
            char *temp1[100]={NULL};
            char *temp2[100]={NULL};

            checkOUToperator(temp[i]);
            checkINoperator(temp[i]);

            // Seperating commands and various arguments
            tokenize( temp[i] , temp1 , "<" );
            tokenize( temp1[0] , temp2 , ">" );
            tokenize( temp2[0] , tokens , " " );

            if(tokens[0]==NULL)
                return;

            int bg = check_bg(tokens);
            if ( strcmp(tokens[0],"exit") == 0)
                _exit(0);

            else
            {
                pipe(fd);
                if(bg)
                    strcpy(cmd[CMDCTR].name,tokens[0]);

                pid_t  pid;
                int flag;

                pid = fork();
                if( pid < 0 )
                {
                    perror("Fork_error ");
                }
                else if(pid  == 0 )
                {
                    if(F_IN != 0)
                    {
                        dup2(F_IN,STDIN_FILENO);
                        close(F_IN);
                    }
                    if(F_OUT != 1)
                    {
                        dup2(F_OUT,STDOUT_FILENO);
                        close(F_OUT);
                    }
                    if(temp[i+1]!=NULL)
                    {
                        dup2(fd[1],STDOUT_FILENO);
                        close(fd[1]);
                    }

                    // executing command
                    if(execvp(*tokens,tokens) < 0)
                    {
                        perror("Error ");
                        exit(0);
                    }
                }
                else
                {
                    //waiting for child process to end
                    if(!bg)
                        wait(&flag);
                    else
                    {
                        cmd[CMDCTR++].pid = pid;
                        printf("Process started: %s [%d]\n",tokens[0],pid);
                    }
                    F_IN=fd[0];
                    close(fd[1]);
                }
            }
            i++;
        }
        F_IN=0;
        F_OUT=1;
        j++;
    }
}

int check_space()
{
    int i=0;
    while( line[i] != '\0' )
    {
        if(line[i]!=' ' && line[i]!='\t')
            return 0;
        i++;
    }
    return 1;
}

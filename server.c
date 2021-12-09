
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
int total_tomatoes =0; 

struct player{
    char player_ID[10];
    int x_position; 
    int y_position;
};
struct socket_addr  { 
  uint16_t        sin_family;  /* Protocol family (always AF_INET) */ 
  uint16_t        sin_port;    /* Port num in network byte order */ 
  struct in_addr  sin_addr;    /* IP addr in network byte order */ 
  unsigned char   sin_zero[8]; /* Pad to sizeof(struct sockaddr) */ 
}; 

void func(int sockfd)
{
  int array[10][10] = {0};
  int numArr[3][3];
    // infinite loop for chat

        // read the message from client and copy it in buffer
        read(sockfd, array, sizeof(array));
        // print buffer which contains the client contents
        bzero(array, MAX);
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                
                int temp;
                array[i][j] =1;
                numArr[i][j] = array[i][j];
            }
    
        }  
        // and send that buffer to client
        write(sockfd, numArr, sizeof(numArr));
   
        // if msg contains "Exit" then server exit and chat ended.
        
}

int main(int argc, char* argv[]){
  int sockfd, connfd, len; 
  unsigned int port = atoi(argv[1]);
  struct socket_addr servaddr, cli; 
  typedef struct sockaddr SA;
  //creating a socket and verifying if it connected
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1){
    printf("socket creation failed.\n");
    exit(0);
  }
  else 
    printf("Socket Successfully created!\n");
  bzero(&servaddr, sizeof(servaddr));

  //populate struct
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  //binding socket to ip address
  if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))!= 0){
    printf("Socket failed binding\n");
    exit(0);
  }
  else
    printf("Socket binded successfully!\n");
  //listening and verifing
  if((listen(sockfd, 4))!=0){
    printf("Listen failed.\n");
    exit(0);
  }
  else 
    printf("server is listening!\n");
  len = sizeof(cli);
  //accept data from client
  connfd = accept(sockfd, (SA*)&cli, &len);
  if(connfd < 0){
    printf("server accept failed.\n");
    exit(0);
  }
  else 
    printf("Server accepted data!\n");

    func(connfd);

    close(sockfd);
}
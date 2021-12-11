#include "csapp.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#define MAX 4
void readInputs(int);
void readGrid(int);
void* thread_handler(void*);
char server_grid[10][10];
unsigned int port;



struct socket_addr  { 
  uint16_t        sin_family;  /* Protocol family (always AF_INET) */ 
  uint16_t        sin_port;    /* Port num in network byte order */ 
  struct in_addr  sin_addr;    /* IP addr in network byte order */ 
  unsigned char   sin_zero[8]; /* Pad to sizeof(struct sockaddr) */ 
}; 



void readInputs(int sockfd){
  for(;;){
    char buff[10];
    read(sockfd, &buff, sizeof(buff));
    if(strcmp(buff, "Q") == 0) break;
    printf("%s\n", buff);
    bzero(&buff, sizeof(buff));

  }

}

void readGrid(int sockfd){
  
  read(sockfd, &server_grid, sizeof(server_grid));
  for(int i =0; i <10; i++){
    for(int j =0; j < 10; j++){
      printf("%c ", server_grid[i][j]);
    }
    printf("\n");
  }
  bzero(&server_grid, sizeof(server_grid));


}

void* thread_handler(void* thread){
   
  printf("handler\n");

}


int main(int argc, char* argv[]){
  int sockfd, connfd, len,newsocket; 
  port = atoi(argv[1]);
  struct socket_addr servaddr, cli; 
  typedef struct sockaddr SA;
  pthread_t tid[4];
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
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    //binding socket to ip address
  if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))!= 0){
    printf("Socket failed binding\n");
    exit(0);
  }
  else
    printf("Socket binded successfully!\n");
  //listening and verifing
  if((listen(sockfd, 100))!=0){
    printf("Listen failed.\n");
    exit(0);
  }
  else 
    printf("server is listening!\n");
  len = sizeof(cli);
  int i =0; 
  while(1){
    //accept call creates a new socket for the income connection
    newsocket = accept(sockfd, (SA*)&cli, &len);
    if(pthread_create(&tid[i++],NULL,thread_handler,&newsocket)!=0){
      printf("Failed to create thread\n");
    }
    if(i >=4) break;

  }
  

  
  printf("Thanks for playing!\n");  
  close(sockfd);
} 
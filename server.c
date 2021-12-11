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
#define GRIDSIZE 10
void readInputs(int);
void readGrid();
void* thread_handler(void*);
void convertTo1D(char**, int);
char server_grid[10][10];
unsigned int port;
int total_tomatoes =0;
int total_connected =0; 



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

void readGrid(){
  printf("total tomatoes -> %d\n", total_tomatoes);
  for(int i =0; i <10; i++){
    for(int j =0; j < 10; j++){
      printf("%c ", server_grid[i][j]);
    }
    printf("\n");
  }


}


void convertTo1D(char** input, int tempsock)
{
    char result[GRIDSIZE*GRIDSIZE];
    int ctr = 0;
    for (int i = 0; i<GRIDSIZE; i++)
    {
        for (int j = 0; j<GRIDSIZE; j++)
        {
            result[ctr] = input[i][j];
            ctr++;
        }
    }
    //send(tempsock, result, sizeof(result)+1,0);
}

void* thread_handler(void* thread){
      total_connected++;
      convertTo1D(server_grid, thread);
      printf("sending grid\n");

}

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void updatedgrid(){


}

void initgrid(){
  for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            double r = rand01();
            if (r < 0.1) {
                server_grid[i][j] = 't';
                total_tomatoes++;
            }
            else{
                server_grid[i][j] = 'g';
            }
        }
    }
    while(total_tomatoes <1)initgrid();
  }

int main(int argc, char* argv[]){
  initgrid();
  readGrid();
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
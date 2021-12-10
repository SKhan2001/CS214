#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#define MAX 100
void readInputs(int);
void* reader(void*);
void* writer(void*);
int total_tomatoes =0; 
sem_t x,y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;



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

void* reader(void* param){

  //lock the semaphore
  sem_wait(&x);
  readercount++;

  if(readercount ==1)
    sem_wait(&y);
  
  //Unlock the semaphore 
  sem_post(&x);

  printf("\n%d reader is inside",readercount);

  //lock the semaphore 
  sem_wait(&x);
  readercount--;

  if(readercount==0){
    sem_post(&y);
  }

  //lock the semaphore
  sem_post(&x);

  printf("\n%dReader is leaving", readercount +1);
  pthread_exit(NULL);

}

void* writer(void* param){
  printf("\nWriter is trying to enter");

  //Lock the semaphore
  sem_wait(&y);

  printf("\nWriter has entered");

  sem_post(&y);

  printf("\nWriter is leaving");
  pthread_exit(NULL);

}

void readInputs(int sockfd){
  for(;;){
    char buff[10];
    read(sockfd, &buff, sizeof(buff));
    if(strcmp(buff, "Q") == 0) break;
    printf("%s\n", buff);
    bzero(&buff, sizeof(buff));

  }

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
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  


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
  
  //accept data from client

  connfd = accept(sockfd, (SA*)&cli, &len);
    if(connfd < 0){
      exit(0);
    }
  readInputs(connfd);
  printf("Thanks for playing!\n");
  close(sockfd);
} 
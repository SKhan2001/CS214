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
void singleArray();
char server_grid[10][10];
unsigned int port;
int total_tomatoes =0;
int connections =0; 
int sockfd;
int newsocket;
char new[100];
pthread_t pthread_readers[10];
int level=0;
typedef struct
{
    int x;
    int y;
} Position;
Position player[4];


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


void singleArray(){
  int ctr = 0;
  for(int i =0; i <10; i++){
      for(int j =0; j<10; j++){
        new[ctr++] = server_grid[i][j];
        printf("%c ", new[i]);
      }
        printf("\n");

  }
}
void* thread_handler(void* thread){
      int temp = atoi(thread);
      //temp = socket(AF_INET, SOCK_STREAM,0);

      for(int i =0 ; i < 10; i++){
        if(server_grid[0][i] == 'p') continue;
        if (server_grid[0][i] == 't'){
          server_grid[0][i] = 'p';
          total_tomatoes--;
          player[connections].x = 0;
          player[connections++].y = i;
          break;
         }
        server_grid[0][i] = 'p';
        player[connections].x = 0;
        player[connections++].y = i;
        break;
      }
      //readGrid();
      singleArray();
      printf("sending grid\n");
      send(newsocket, new, strlen(new),0);
      //memset(new, 0, 100);
}

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void updatedgrid(char move, int sockfd){
  if(move == 'w'){
    

  }


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
    level++;

  }

int main(int argc, char* argv[]){
  initgrid();
  int connfd, len; 
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
  while(newsocket = accept(sockfd, (SA*)&cli, &len)){

   if(pthread_create(&tid[i++],NULL,thread_handler,&newsocket)!=0){
      printf("Failed to create thread\n");
    }

  }
  

  
  printf("Thanks for playing!\n");  
  close(sockfd);
} 
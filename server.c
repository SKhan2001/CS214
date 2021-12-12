#include "csapp.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#define MAX 4
#define GRIDSIZE 10
void readInputs();
void readGrid();
void* thread_handler(void*);
void singleArray();
void placePlayer();
void pop_struct();
void initgrid();
char server_grid[10][10];
pthread_t tid[4];
unsigned int port;
int total_tomatoes =0;
int consumed_tomatoes =0;
int connections =0; 
int sockfd;
int newsocket, len;
char new[100];
pthread_t pthread_readers[10];
uint32_t level=1;
int current_level =1;
int game_over =0;
struct socket_addr servaddr, cli; 

struct gameInfo{
  int total_score; 
  int total_tomatoes;
  int game_level; 
  char game_grid[10][10]; 
};
struct gameInfo game;

typedef struct
{
    int id;
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

void send_struct(){
  for(int i =0 ; i < connections; i++){
    sendto(tid[i], &game, sizeof(struct gameInfo),0, (struct sockaddr *)&servaddr, sizeof(len));
  }
}

void pop_struct(){
  game.total_score = consumed_tomatoes;
  game.total_tomatoes = total_tomatoes;
  game.game_level = level;
  memcpy(game.game_grid, server_grid, sizeof(server_grid));

}

void readInputs(){
    
    for(int i =0 ; i < connections; i++){
        char buff;
        recv(tid[i], &buff, sizeof(buff),0);
        updatedgrid(buff, i);
        bzero(&buff, sizeof(buff));
        if(current_level < level){
          current_level++;
          initgrid();
        }
        pop_struct();
        send_struct();
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
  
  for(int i =0; i <10; i++){
      for(int j =0; j<10; j++){
        new[i] = server_grid[i][j];
      }
  }
  printf("%s\n", new);
}

void placePlayer(int thread){
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

}
void* thread_handler(void* thread){
      player->id = connections;
      int temp = atoi(thread);
      placePlayer(temp);
      uint32_t converted_level = htonl(level);
      //if(write(newsocket, server_grid, sizeof(char)*100)<0) perror("Error sending\n");
      //if(write(newsocket, &converted_level, sizeof(converted_level))<0) perror("Error sending\n");
      pop_struct();
      sendto(newsocket, &game, sizeof(struct gameInfo),0, (struct sockaddr *)&servaddr, sizeof(servaddr));

     
}

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}


//START OF UPDATED GRID
void updatedgrid(char move, int p){
  int i =p; 
  if(move == 'w'|| move =='W'){
    if(player[i].y ==0) return;
    if(server_grid[player[i].x][player[i].y+1] == 'p')return;
    if(server_grid[player[i].x][player[i].y+1] == 't'){
      total_tomatoes--;
      consumed_tomatoes++;
      if(consumed_tomatoes>=20)game_over=1;
      if(total_tomatoes <1)level++;
      server_grid[player[i].x][player[i].y] == 'g';
      player[i].y++;
      server_grid[player[i].x][player[i].y] == 'p';
      return;
    }
  }
    if(move == 'a'|| move == 'A'){
      if(player[i].x ==0) return;
      if(server_grid[player[i].x-1][player[i].y] == 'p')return;
      if(server_grid[player[i].x-1][player[i].y] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i].x][player[i].y] == 'g';
          player[i].x--;
          server_grid[player[i].x][player[i].y] == 'p';
          return;
      }
    }
    if(move =='s'|| move =='S'){
      if(player[i].y ==9) return;
      if(server_grid[player[i].x][player[i].y-1] == 'p')return;
      if(server_grid[player[i].x][player[i].y-1] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i].x][player[i].y] == 'g';
          player[i].y--;
          server_grid[player[i].x][player[i].y] == 'p';
          return;
      }
    }
    if(move =='d'|| move =='D'){
      if(player[i].x ==9) return;
      if(server_grid[player[i].x+1][player[i].y] == 'p')return;
      if(server_grid[player[i].x+1][player[i].y] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i].x][player[i].y] == 'g';
          player[i].x++;
          server_grid[player[i].x][player[i].y] == 'p';
          return;
      }
    if(move == 'q'||move == 'Q'){
      game_over=1;
      return;
    }
    
    }
  }//END OF UPDATEDGRID


void initgrid(){
  for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            double r = rand01();
            if (r < .4) {
                server_grid[i][j] = 't';
                total_tomatoes++;
            }
            else{
                server_grid[i][j] = 'g';
            }
        }
    }
    while(total_tomatoes <1){
      initgrid();
    }

  }

int main(int argc, char* argv[]){
  initgrid();
  int connfd; 
  port = atoi(argv[1]);
  typedef struct sockaddr SA;
  
  srand(time(NULL));
  //creating a socket and verifying if it connected
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1){
    exit(0);
  }

  bzero(&servaddr, sizeof(servaddr));

  //populate struct
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    //binding socket to ip address
  if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))!= 0){
    exit(0);
  }
  //listening and verifing
  if((listen(sockfd, 100))!=0){
    exit(0);
  }
  len = sizeof(cli);
  int i =0; 
  while(!game_over){
    if(i <4){ 
        newsocket = accept(sockfd, (SA*)&cli, &len);
        connections++;
        if(pthread_create(&tid[i],NULL,thread_handler,&newsocket)!=0){
        }
  
    }
    readInputs();
    }
  

  
  printf("Thanks for playing!\n");  
  close(sockfd);
} 
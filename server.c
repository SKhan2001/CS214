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
pthread_t tid;
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
    int connfd;
    int id;
    int x;
    int y;
} Position;
Position *player[4];

struct socket_addr  { 
  uint16_t        sin_family;  /* Protocol family (always AF_INET) */ 
  uint16_t        sin_port;    /* Port num in network byte order */ 
  struct in_addr  sin_addr;    /* IP addr in network byte order */ 
  unsigned char   sin_zero[8]; /* Pad to sizeof(struct sockaddr) */ 
}; 

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t topic_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_struct(){
  for(int i = 0 ; i < connections; i++){
    //printf("%d\n", player[i]->connfd);
    sendto(player[i]->connfd, &game, sizeof(struct gameInfo),0, (struct sockaddr *)&servaddr, sizeof(len));
    //printf("here!\n");
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
        recv(player[i]->connfd, &buff, sizeof(buff),0);
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

void placePlayer(Position *player){
   for(int i =0 ; i < 10; i++){
        if(server_grid[0][i] == 'p') continue;
        if (server_grid[0][i] == 't'){
          server_grid[0][i] = 'p';
          total_tomatoes--;
          player->x = 0;
          player->y = i;
          break;
         }
         server_grid[0][i] = 'p';
        player->x = 0;
        player->y = i;
        break;
      }

}
void* thread_handler(void* thread){
      Position *temp = (Position*)thread;
      int rlen;
      char buff;

      printf("Player %d:\nconnfd: %d\nx: %d\ny: %d\n\n", temp->id, temp->connfd, temp->x, temp->y);
      placePlayer(temp);
      pop_struct();
      send_struct();
      readGrid();

      while (!game_over)
      {
        rlen = read(temp->connfd, buff, sizeof(buff));
        printf("input bool: %d\n", rlen);
        updatedgrid(buff, temp->id);
        pop_struct();
        send_struct();

        printf("player %d: %c", temp->id, buff); 
      }
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
    if(player[i]->y ==0) return;
    if(server_grid[player[i]->x][player[i]->y+1] == 'p')return;
    if(server_grid[player[i]->x][player[i]->y+1] == 't'){
      total_tomatoes--;
      consumed_tomatoes++;
      if(consumed_tomatoes>=20)game_over=1;
      if(total_tomatoes <1)level++;
      server_grid[player[i]->x][player[i]->y] == 'g';
      player[i]->y++;
      server_grid[player[i]->x][player[i]->y] == 'p';
      return;
    }
  }
    if(move == 'a'|| move == 'A'){
      if(player[i]->x ==0) return;
      if(server_grid[player[i]->x-1][player[i]->y] == 'p')return;
      if(server_grid[player[i]->x-1][player[i]->y] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i]->x][player[i]->y] == 'g';
          player[i]->x--;
          server_grid[player[i]->x][player[i]->y] == 'p';
          return;
      }
    }
    if(move =='s'|| move =='S'){
      if(player[i]->y ==9) return;
      if(server_grid[player[i]->x][player[i]->y-1] == 'p')return;
      if(server_grid[player[i]->x][player[i]->y-1] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i]->x][player[i]->y] == 'g';
          player[i]->y--;
          server_grid[player[i]->x][player[i]->y] == 'p';
          return;
      }
    }
    if(move =='d'|| move =='D'){
      if(player[i]->x ==9) return;
      if(server_grid[player[i]->x+1][player[i]->y] == 'p')return;
      if(server_grid[player[i]->x+1][player[i]->y] == 't'){
          total_tomatoes--;
          consumed_tomatoes++;
          if(consumed_tomatoes>=20)game_over=1;
          if(total_tomatoes <1)level++;
          server_grid[player[i]->x][player[i]->y] == 'g';
          player[i]->x++;
          server_grid[player[i]->x][player[i]->y] == 'p';
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
  //initgrid();
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

  len = sizeof(servaddr);
  int i =0; 
  initgrid();

  while(!game_over){
    if(i <4){ 
        newsocket = accept(sockfd, (SA*)&servaddr, &len);
        connections++;

        Position *temp = (Position*)malloc(sizeof(Position));
        temp->connfd = newsocket;
        temp->id = connections;
        temp->y = connections - 1;
        temp->x = 0;
        
        player[connections-1] = temp;
        pthread_create(&tid,NULL,thread_handler,(void*)temp);
    }
    }
  

  
  printf("Thanks for playing!\n");  
  close(sockfd);
} 
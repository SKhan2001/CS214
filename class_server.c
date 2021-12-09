
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define GRIDSIZE 10
int total_tomatoes =0; 
char receive[20][20] = {0};
//char grid[10][10];
int moveUp = 0;
int moveDown = 0;
int moveLeft = 0;
int moveRight = 0;

typedef struct
{
    int x;
    int y;
} Position;

typedef enum
{
    TILE_GRASS,
    TILE_TOMATO
} TILETYPE;

TILETYPE grid[GRIDSIZE][GRIDSIZE];

Position playerPosition;
int score;
int level;
int numTomatoes;

struct socket_addr  { 
  uint16_t        sin_family;  /* Protocol family (always AF_INET) */ 
  uint16_t        sin_port;    /* Port num in network byte order */ 
  struct in_addr  sin_addr;    /* IP addr in network byte order */ 
  unsigned char   sin_zero[8]; /* Pad to sizeof(struct sockaddr) */ 
}; 

double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void clientInput(int sockfd)
{
    bzero(moveUp, sizeof(moveUp));
    read(sockfd, moveUp, sizeof(moveUp));

    bzero(moveDown, sizeof(moveDown));
    read(sockfd, moveDown, sizeof(moveDown));

    bzero(moveLeft, sizeof(moveLeft));
    read(sockfd, moveLeft, sizeof(moveLeft));

    bzero(moveRight, sizeof(moveRight));
    read(sockfd, moveRight, sizeof(moveRight));
}

void func(int sockfd)
{
    while(1)
    {
        clientInput(sockfd);
        handleKeyDown();

        bzero(grid, sizeof(grid));
        read(sockfd, grid, sizeof(grid));

        bzero(grid, sizeof(grid));
        write(sockfd, grid, sizeof(grid));
    }        
}

void moveTo(int x, int y)
{
    // Prevent falling off the grid
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return;

    // Sanity check: player can only move to 4 adjacent squares
    if (!(abs(playerPosition.x - x) == 1 && abs(playerPosition.y - y) == 0) &&
        !(abs(playerPosition.x - x) == 0 && abs(playerPosition.y - y) == 1)) {
        fprintf(stderr, "Invalid move attempted from (%d, %d) to (%d, %d)\n", playerPosition.x, playerPosition.y, x, y);
        return;
    }

    playerPosition.x = x;
    playerPosition.y = y;

    if (grid[x][y] == TILE_TOMATO) {
        grid[x][y] = TILE_GRASS;
        score++;
        numTomatoes--;
        if (numTomatoes == 0) {
            level++;
            initGrid();
        }
    }
}

void initGrid()
{
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            double r = rand01();
            if (r < 0.1) {
                grid[i][j] = TILE_TOMATO;
                numTomatoes++;
            }
            else
                grid[i][j] = TILE_GRASS;
        }
    }

    // force player's position to be grass
    if (grid[playerPosition.x][playerPosition.y] == TILE_TOMATO) {
        grid[playerPosition.x][playerPosition.y] = TILE_GRASS;
        numTomatoes--;
    }

    // ensure grid isn't empty
    while (numTomatoes == 0)
        initGrid();
}

void handleKeyDown()
{
    if (moveUp)
        moveTo(playerPosition.x, playerPosition.y - 1);

    if (moveDown)
        moveTo(playerPosition.x, playerPosition.y + 1);

    if (moveLeft)
        moveTo(playerPosition.x - 1, playerPosition.y);

    if (moveRight)
        moveTo(playerPosition.x + 1, playerPosition.y);
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
  {
      initGrid();
      printf("Server accepted data!\n");
  }

    func(connfd);

    close(sockfd);
}
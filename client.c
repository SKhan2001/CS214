#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#define MAX 80

#define SA struct sockaddr


// Dimensions for the drawn grid (should be GRIDSIZE * texture dimensions)
#define GRID_DRAW_WIDTH 640
#define GRID_DRAW_HEIGHT 640

#define WINDOW_WIDTH GRID_DRAW_WIDTH
#define WINDOW_HEIGHT (HEADER_HEIGHT + GRID_DRAW_HEIGHT)

// Header displays current score
#define HEADER_HEIGHT 50

// Number of cells vertically/horizontally in the grid
#define GRIDSIZE 10
char server_grid[10][10];
int master_socket; 
char new[10][10];
int PORT;
char move;

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
uint32_t level;
int numTomatoes;

bool shouldExit = false;

TTF_Font* font;

struct gameInfo{
  int total_score; 
  int total_tomatoes;
  uint32_t game_level; 
  char game_grid[10][10]; 
};
struct gameInfo game; 


void pop_global(){
    score = game.total_score;
    numTomatoes = game.total_tomatoes;
    level = game.game_level; 
    memcpy(server_grid, game.game_grid, sizeof(game.game_grid));
}

void readGrid(){
for(int i =0; i < 10; i++){
    for(int j =0; j<10; j++){
        printf("%c ", server_grid[i][j]);
    }
    printf("\n");
}

}

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void initGrid()
{
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            if (server_grid[i][j] ==  't') {
                grid[j][i] = TILE_TOMATO;
                numTomatoes;
            }
            else if(server_grid[i][j] =='p'){
                grid[j][i] = TILE_GRASS;
                playerPosition.x = j;
                playerPosition.y =i;
            }
            else
                grid[j][i] == TILE_GRASS;
        }
    }

    // force player's position to be grass
    if (grid[playerPosition.x][playerPosition.y] == TILE_TOMATO) {
        grid[playerPosition.x][playerPosition.y] = TILE_GRASS;
    }

}

void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int rv = IMG_Init(IMG_INIT_PNG);
    if ((rv & IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Error initializing IMG: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing TTF: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
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

void handleKeyDown(SDL_KeyboardEvent* event, int sockfd)
{
    // ignore repeat events if key is held down
    if (event->repeat)
        return;

    move = NULL;

    if (event->keysym.scancode == SDL_SCANCODE_Q || event->keysym.scancode == SDL_SCANCODE_ESCAPE){
        shouldExit = true;
        move = 'Q';
        //write(sockfd, move, sizeof(move));
    }

    if (event->keysym.scancode == SDL_SCANCODE_UP || event->keysym.scancode == SDL_SCANCODE_W){
        move = 'w';
        //write(sockfd, move, sizeof(move));        
        //moveTo(playerPosition.x, playerPosition.y - 1);
        
    }

    if (event->keysym.scancode == SDL_SCANCODE_DOWN || event->keysym.scancode == SDL_SCANCODE_S){
        move = 's';
        //write(sockfd, move, sizeof(move));   
        //moveTo(playerPosition.x, playerPosition.y + 1);
        
    }
    if (event->keysym.scancode == SDL_SCANCODE_LEFT || event->keysym.scancode == SDL_SCANCODE_A){
        move = 'a';
        //write(sockfd, move, sizeof(move));   
        //moveTo(playerPosition.x - 1, playerPosition.y);
        
    }
    if (event->keysym.scancode == SDL_SCANCODE_RIGHT || event->keysym.scancode == SDL_SCANCODE_D){
        move = 'd';
        //write(sockfd, move, sizeof(move));   
        //moveTo(playerPosition.x + 1, playerPosition.y);
        
    }
}

void processInputs(int sockfd)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				shouldExit = true;
				break;

            case SDL_KEYDOWN:
                handleKeyDown(&event.key, sockfd);
				break;

			default:
				break;
		}
	}
}

void drawGrid(SDL_Renderer* renderer, SDL_Texture* grassTexture, SDL_Texture* tomatoTexture, SDL_Texture* playerTexture)
{
    SDL_Rect dest;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            dest.x = 64 * i;
            dest.y = 64 * j + HEADER_HEIGHT;

            SDL_Texture* texture;

            switch (server_grid[i][j])
            {
            case 'g':
            texture = grassTexture;
                break;
            
            case 't':
            texture = tomatoTexture;
                break;
            
            case 'p':
            texture = playerTexture;
                break;
            
            default:
                break;
            }

            SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
            SDL_RenderCopy(renderer, texture, NULL, &dest);
        }
    }
}

void drawUI(SDL_Renderer* renderer)
{
    // largest score/level supported is 2147483647
    char scoreStr[18];
    char levelStr[18];
    sprintf(scoreStr, "Score: %d", score);
    sprintf(levelStr, "Level: %d", level);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreStr, white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelStr, white);
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);

    SDL_Rect scoreDest;
    TTF_SizeText(font, scoreStr, &scoreDest.w, &scoreDest.h);
    scoreDest.x = 0;
    scoreDest.y = 0;

    SDL_Rect levelDest;
    TTF_SizeText(font, levelStr, &levelDest.w, &levelDest.h);
    levelDest.x = GRID_DRAW_WIDTH - levelDest.w;
    levelDest.y = 0;

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreDest);
    SDL_RenderCopy(renderer, levelTexture, NULL, &levelDest);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
}

void doubleArray(){
    int ctr =0;
    for(int i =0; i < 10; i++){
        for(int j =0; j < 10; j++){
            server_grid[i][j] = new[ctr];
            printf("%c ", new[ctr]);
            ctr++;
        }
        printf(" \n");
    }


}

void* clienthread(void *args)
{
    uint32_t temp;
    int client_request = *((int*)args);
    int network_socket;

    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
     if(connect(network_socket, (SA*)&server_address, sizeof(server_address))!=0){
        printf("Connection with the server failed...\n");
        exit(0);
    }
    else printf("Connected to the server...\n");
    
    recvfrom(network_socket, &game, sizeof(struct gameInfo), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    pop_global();
    readGrid();
    
    close(network_socket);
    pthread_exit(NULL);

    return 0;
}

int main(int argc, char* argv[])
{
   
    int connfd;
    PORT = atoi(argv[1]);
    struct sockaddr_in servaddr, cli; 
    srand(time(NULL));
    initSDL();
    
    font = TTF_OpenFont("resources/Burbank-Big-Condensed-Bold-Font.otf", HEADER_HEIGHT);
    if (font == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == NULL) {
        fprintf(stderr, "Error creating app window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == NULL)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}

    SDL_Texture *grassTexture = IMG_LoadTexture(renderer, "resources/grass.png");
    SDL_Texture *tomatoTexture = IMG_LoadTexture(renderer, "resources/tomato.png");
    SDL_Texture *playerTexture = IMG_LoadTexture(renderer, "resources/player.png");

    uint32_t temp;

    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

     if(connect(master_socket, (SA*)&server_address, sizeof(server_address))!=0){
        printf("Connection with the server failed...\n");
        exit(0);
    }
    else printf("Connected to the server...\n");

    recvfrom(master_socket, &game, sizeof(struct gameInfo), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    pop_global();
    readGrid();
    printf("total tomatoes -> %d\n", numTomatoes);
   
    // main game loop
    while (!shouldExit) {
        SDL_SetRenderDrawColor(renderer, 0, 105, 6, 255);
        SDL_RenderClear(renderer);
        
        processInputs(master_socket);
        write(master_socket, move, sizeof(move));

        recvfrom(master_socket, &game, sizeof(struct gameInfo), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        pop_global();
        readGrid();

        drawGrid(renderer, grassTexture, tomatoTexture, playerTexture);
        drawUI(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 16 ms delay to limit display to 60 fps
    }

    // clean up everything
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(tomatoTexture);
    SDL_DestroyTexture(playerTexture);

    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

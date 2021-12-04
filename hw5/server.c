#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


int main(int argc, char **argv)
{
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    listenfd = Open_listenfd(argv[1]);
    while (1) 
    {
        clientlen=sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int)); 
        *connfdp = Accept(listenfd, 
        (SA *) &clientaddr, &clientlen); 
        Pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* Thread routine */
void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self()); 
    Free(vargp);                    
    echo(connfd);
    Close(connfd);
    return NULL;
}
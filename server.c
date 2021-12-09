
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#define MAX 80
#define MAX_CLIENTS 4
#define true 1
#define false 0
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

        // read the message from client and copy it in buffer
        read(sockfd, array, sizeof(array));
        // print buffer which contains the client contents
        bzero(array, MAX);
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                array[i][j] =1;
                numArr[i][j] = array[i][j];
            }
    
        }  
        // and send that buffer to client
        write(sockfd, numArr, sizeof(numArr));
   
        
}

int main(int argc, char* argv[]){
  int sockfd, connfd, len, max_sd, sd, activity, new_socket; 
  int client_connect[4];
  unsigned int port = atoi(argv[1]);
  struct socket_addr servaddr, cli; 
  typedef struct sockaddr SA;
  fd_set readfds;
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
  if((listen(sockfd, 4))<0){
    printf("Listen failed.\n");
    exit(0);
  }
  else 
    printf("server is listening!\n");
  len = sizeof(cli);
  puts("wating for connections...\n");
  //accept data from client
  while(true){
    //clearing the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(sockfd, &readfds);
    max_sd = sockfd;

    //adding child to sockets to set 
    for(int i =0; i < MAX_CLIENTS; i++){
      //socket descriptor
      sd = client_connect[i];

      //if valid socket descriptor then add to read list
      if(sd > 0)
        FD_SET(sd , &readfds);
      //hiighest file descriptor number, need it for the select function
      if(sd > max_sd)
        max_sd = sd;
    }
    /*wait for an activity on on of the sockets, 
    timeout is NULL, so wait indefinitely*/
    activity = select(sd, &readfds, NULL, NULL, NULL);

    if((activity < 0) && (errno != EINTR))
    printf("Select error!");

      /*if something happened to the master socket(sockfd), then its 
      and incoming connection */
      if(FD_ISSET(sockfd, &readfds))
      {
      if ((new_socket = accept(sockfd, (struct sockaddr *)&servaddr, (socklen_t*)&len))<0)            {
            perror("accept");
            exit(EXIT_FAILURE);
          }
          char *message = "Welcome to multithread!";
          write()
          puts("Welcome message sent successfully!\n");

          //add new socket to array of sockets
          for(int i = 0; i < MAX_CLIENTS; i++){
            if(client_connect[i] ==0){
              client_connect[i] = new_socket;
              printf("Adding to list of sockets as %d\n", i +1);

              break;
            }
          }
      }
  }

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
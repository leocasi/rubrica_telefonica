#include "utility.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){

  int sfd=socket(AF_INET,SOCK_STREAM,0);
  if(sfd==-1){
    perror("error creating socket\n");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(9002);

  int connection_status=connect(sfd,(struct sockaddr *) &server_addr,sizeof(server_addr));
  if(connection_status==-1){
    printf("error connecting\n");
    exit(EXIT_FAILURE);
  }
  while(1){
    login(sfd);
    break;
  }
 return 0;
}

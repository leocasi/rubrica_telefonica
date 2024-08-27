#include "utility.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc,char *argv[]){

  int sfd=socket(AF_INET,SOCK_STREAM,0);
  if(sfd==-1){
    perror("error creating socket\n");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(9002);

  if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
    perror("error binding\n");
    exit(EXIT_FAILURE);
  }
  if(listen(sfd,5)==-1){
    perror("error listening\n");
    exit(EXIT_FAILURE);
  }
  server_login(sfd);
  close(sfd);
  return 0;
}

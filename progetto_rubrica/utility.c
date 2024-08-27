#include "utility.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void clear_buffers(char *buffer,char *command){
  memset(buffer, 0, sizeof(buffer));
  memset(command, 0, sizeof(command));
}

void gui(){
   printf("Benvenuto nella rubrica, per accedere alle funzioni esegui l'accesso\n");
   printf("1. Login\n");
   printf("2. Sign up\n");
   printf("3. Continua senza accedere\n");

}

void gui_1(){
  printf("Scegli un'opzione:\n");
  printf("1. Aggiungi contatto\n");
  printf("2. Elimina contatto\n");
  printf("3. Visualizza contatti\n");
  printf("4. Esci\n");
  printf("Inserisci il numero dell'opzione: ");
}

void main_menu(int sfd,char *buffer,char *command,int is_authenticated){
  gui_1();
  while(1){
    clear_buffers(buffer,command);
    int choice=0;
    scanf("%d",&choice);
    getchar();
    switch(choice){
    case 1:
          if(is_authenticated==0){
            printf("You must be logged-in\n");
            clear_buffers(buffer,command);
            main_menu(sfd,buffer,command,is_authenticated);
          }
          clear_buffers(buffer,command);
          char nome_contatto[50];
          printf("Inserisci nome: ");
          fgets(nome_contatto, sizeof(nome_contatto), stdin);
          nome_contatto[strcspn(nome_contatto, "\n")] = 0;
          char numero_contatto[50];
          printf("Inserisci numero: ");
          fgets(numero_contatto,sizeof(numero_contatto),stdin);
          numero_contatto[strcspn(numero_contatto,"\n")]=0;

          snprintf(command,256, "ADD %s %s", nome_contatto,numero_contatto);
          send(sfd, command, strlen(command), 0);
          recv(sfd, buffer, 255, 0);
          printf("\n%s\n\n", buffer);
          clear_buffers(buffer,command);
          main_menu(sfd,buffer,command,is_authenticated);


    case 2:
          if(is_authenticated==0){
          printf("You must be logged-in\n");
          main_menu(sfd,buffer,command,is_authenticated);
          }
          clear_buffers(buffer,command);
          printf("inserisci il nome del contatto da eliminare: ");
          fgets(buffer, 50, stdin);
          buffer[strcspn(buffer, "\n")] = 0;
          snprintf(command, 57, "DELETE %s", buffer);
          send(sfd, command, strlen(command), 0);
          recv(sfd, buffer, 255, 0);
          buffer[255] = '\0';
          printf("%s\n", buffer);
          clear_buffers(buffer,command);
          main_menu(sfd,buffer,command,is_authenticated);
          break;


    case 3:
          clear_buffers(buffer,command);
          char contatti_ricevuti[10000];
          snprintf(command, sizeof(command), "VIEW");
          send(sfd, command, strlen(command), 0);
          recv(sfd, contatti_ricevuti, sizeof(contatti_ricevuti) - 1, 0);

          contatti_ricevuti[10000] = '\0';
          char *singolo_utente=strtok(contatti_ricevuti,"\n");
          char numero_ricevuto[50];
          char nome_ricevuto[50];

          printf("+----------------------------+\n");
          printf("|        RUBRICA CONTATTI     |\n");
          printf("+----------------------------+\n");
          while(singolo_utente!=NULL){
            sscanf(singolo_utente,"%s %s",nome_ricevuto,numero_ricevuto);
            printf("| %-20s | %10s |\n", nome_ricevuto, numero_ricevuto);
            singolo_utente=strtok(NULL,"\n");
          }
          printf("+----------------------------+\n");
          clear_buffers(buffer,command);
          main_menu(sfd,buffer,command,is_authenticated);
          break;

    case 4:
         printf("Esco...\n");
         close(sfd);
         exit(EXIT_SUCCESS);

    default:
          printf("Opzione non valida\n");
          break;
         }
      }
   }

int ricerca_utente(char *buffer,char *confronto){

  FILE *f1=fopen("Sicurezza.txt","r");
  if(f1==NULL){
    printf("error opening f1.txt\n");
    exit(EXIT_FAILURE);
  }
  char line[256];
  int found=0;
  long pos=0;
  while(fgets(line,sizeof(line),f1)!=NULL){
    char username[256];
    if(sscanf(line,"%s",username)==1){
      if(strcmp(username,buffer)==0){
        sscanf(line,"%*s %s",confronto);
        fclose(f1);
        return 1;
      }
    }
  }
  fclose(f1);
  return 0;
}


void login(int sfd){
  int is_authenticated=0;
  char buffer[200];
  int choice;
  char command[256];

  gui();
  scanf("%d",&choice);
  getchar();
  switch(choice){

  case 1:
    char username[50];
    printf("Inserisci utente: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    char password[50];
    printf("Inserisci password: ");
    fgets(password,sizeof(password),stdin);
    password[strcspn(password,"\n")]=0;
    snprintf(command, sizeof(command), "LOGIN %s %s", username,password);
    send(sfd, command, strlen(command), 0);
    recv(sfd, buffer, sizeof(buffer) - 1, 0);
    printf("%s",buffer);

    if(strncmp(buffer,"LOGIN effettuato",16)==0){
      is_authenticated=1;
      clear_buffers(buffer,command);
      main_menu(sfd,buffer,command,is_authenticated);
      break;
    }
    else{
      buffer[255] = '\0';
      printf("\n%s\n", buffer);
      clear_buffers(buffer,command);
      break;
  }

  case 2:
  char new_username[50];
  printf("Inserisci utente: ");
  fgets(new_username, sizeof(new_username), stdin);
  new_username[strcspn(new_username, "\n")] = 0;
  char new_password[50];
  printf("Inserisci password: ");
  fgets(new_password,sizeof(new_password),stdin);
  new_password[strcspn(new_password,"\n")]=0;
  snprintf(command, sizeof(command), "SIGNUP %s %s", new_username,new_password);
  send(sfd, command, strlen(command), 0);
  ssize_t recv_len=recv(sfd, buffer, sizeof(buffer) - 1, 0);
  buffer[recv_len]='\0';
  printf("%s\n",buffer);
  if(strncmp(buffer,"SIGN UP riuscito",16)==0){
    is_authenticated=1;
    clear_buffers(buffer,command);
    main_menu(sfd,buffer,command,is_authenticated);
  }

  case 3:
    send(sfd,"NOACC ",6,0);
    recv(sfd,buffer,sizeof(buffer)-1,0);
    printf("%s",buffer);
    clear_buffers(buffer,command);
    main_menu(sfd,buffer,command,is_authenticated);

  default:
    clear_buffers(buffer,command);
    printf("Opzione non valida\n");
    break;
    }
  }


void server_login(int sfd){
  while(1){
  int cfd=accept(sfd,NULL,NULL);
  if(cfd==-1){
    printf("error accepting connetion\n");
    continue;
  }
  printf("client conneted\n");

  char buffer[256];
  while(1){
  ssize_t security=recv(cfd,buffer,sizeof(buffer)-1,0);
  if(security<=0){
    if(security==0){
      printf("client disconnected\n");
    }
    else{
      printf("error reciving\n");
    }
    close(cfd);
    break;
  }
  buffer[security]='\0';
  if(strncmp(buffer,"LOGIN ",6)==0){
  char username[50];
  char password[50];
  sscanf(buffer+6,"%49s %49s",username,password);
  //ricerca utente
  char confronto[50];
  int result=ricerca_utente(username,confronto);
  if(result==1){
  //utente trovato
  //confronto password
  if(strcmp(password,confronto)==0){
    send(cfd,"LOGIN effettuato\n",18,0);
  }
  else{
    send(cfd,"nome utente o password errata\n",31,0);
  }
  }
  if(result==0){
  //utente non trovato
  char *not_found="Username non trovato\n";
  send(cfd,not_found,strlen(not_found),0);
        }
      }
  else if(strncmp(buffer,"SIGNUP ",7)==0){
    char username[50];
    char password[50];
    int num_params=sscanf(buffer+7,"%49s %49s",username,password);
    if (num_params != 2) {
        send(cfd, "Formato SIGNUP non valido. Usa: SIGNUP username password\n", 60, 0);
        continue;
    }
    char riga[256];
    char confronto[50];
    snprintf(riga,sizeof(riga),"%s %s\n",username,password);
    printf("%s",riga);
    int result=ricerca_utente(username,confronto);
    if(result==0){
      FILE *f1=fopen("Sicurezza.txt","a");
      if(f1==NULL){
        printf("error opening Sicurezza.txt\n");
        exit(EXIT_FAILURE);
      }
      fprintf(f1,"%s",riga);
      fclose(f1);
      send(cfd,"SIGN UP riuscito ",17,0);
    }
    else{
      send(cfd,"SIGN UP non riuscito ",21,0);
    }
}
  else if(strncmp(buffer,"NOACC ",6)==0){
    send(cfd,"!Warning! Se non esegui l'accesso non potrai aggiungere/rimuovere contatti\n",100,0);
  }
  else if(strncmp(buffer,"ADD ",4)==0){
    FILE *f1=fopen("Contatti.txt","a");
    if(f1==NULL){
      printf("error opening Contatti.txt\n");
      exit(EXIT_FAILURE);
    }
    fprintf(f1,"\n%s",buffer+4);
    fclose(f1);
    send(cfd,"Contatto aggiunto",17,0);
  }
  else if(strncmp(buffer,"DELETE ",7)==0){
    FILE *f1=fopen("Contatti.txt","r");
    FILE *f2=fopen("temp.txt","w");
    if(f1==NULL||f2==NULL){
      printf("error opening file\n");
      exit(EXIT_FAILURE);
    }
    char nome_ricevuto[50];
    int found=0;
    char riga[256];

    sscanf(buffer+7,"%s",nome_ricevuto);
    while(fgets(riga,256,f1)!=NULL){
      char nome_nel_file[50];
      char numero_nel_file[50];
      sscanf(riga,"%s %s",nome_nel_file,numero_nel_file);
      printf("%s\n",nome_nel_file);
      printf("%s\n",nome_ricevuto);
      if(strcmp(nome_nel_file,nome_ricevuto)==0){
        found=1;
      }
      else{
        fprintf(f2,"%s %s\n",nome_nel_file,numero_nel_file);
      }
    }
    fclose(f1);
    fclose(f2);

    if(found==0){
      remove("temp.txt");
      send(cfd,"Nome utente non trovato\n",25,0);
    }
    else{
      remove("Contatti.txt");
      rename("temp.txt","Contatti.txt");
      send(cfd,"Contatto eliminato\n",20,0);
    }
  }
  else if(strncmp(buffer,"VIEW",4)==0){
    FILE *f1 = fopen("Contatti.txt", "r");
    if (f1== NULL) {
        perror("Errore nell'aprire il file");
        send(cfd, "Errore nel recupero dei contatti\n", 34, 0);
        return;
        }
    char rubrica[10000];
    char riga[256]="Contatti:\n\n";
    while(fgets(riga,sizeof(riga),f1)!=NULL){
      strcat(buffer,riga);
    }
    fclose(f1);
    if(strlen(rubrica)==strlen(riga)){
      strcat(buffer,"Nessun contatto trovato\n");
    }
    send(cfd,buffer+4,sizeof(buffer)-4,0);
      }
    }
  }
}

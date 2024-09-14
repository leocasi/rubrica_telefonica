#include "utility.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MIN_USERNAME_LENGTH 3
#define MAX_USERNAME_LENGTH 20
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 30

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"

void print_error(const char* message) {
    fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", message);
  }

void print_warning(const char* message){
  fprintf(stdout, ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n",message);
}

void print_success(const char* message){
  fprintf(stdout, ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n",message);
}

int validate_username(char *username){  //controllo username (lunghezza e caratteri)
  int len=strlen(username);
  if(len<MIN_USERNAME_LENGTH||len>MAX_USERNAME_LENGTH){
    return 0;
  }
  for(int i=0;i<len;i++){
    if(!isalnum(username[i]) && username[i]!='_'){  //se diverso da alfanumerico o underscore
      return 0;
      }
    }
    return 1;
  }

int validate_password(char *password){  //controllo password
  int len=strlen(password);
  if(len<MIN_PASSWORD_LENGTH||len>MAX_PASSWORD_LENGTH){
    return 0;
  }
  int maiuscola=0,minuscola=0,cifra=0,carattere=0;
  for(int i=0;i<len;i++){
    if(isupper(password[i])) maiuscola=1;
    else if(islower(password[i])) minuscola=1;
    else if(isdigit(password[i])) cifra=1;
    else if(password[i]==' ') return 0;
    else carattere=1;
  }
  return maiuscola && minuscola && cifra && carattere;  //per essere valida devono valere tutti 1
}


void clear_buffers(char *buffer,char *command){   //libero buffer
  memset(buffer, 0, sizeof(buffer));
  memset(command, 0, sizeof(command));
}

void gui(){  //primo menù mostrato
   printf("Benvenuto nella rubrica, per accedere alle funzioni esegui l'accesso\n");
   printf("1. Login\n");
   printf("2. Sign up\n");
   printf("3. Continua senza accedere\n");

}

void gui_1(){  //secondo menù mostrato
  printf("Scegli un'opzione:\n");
  printf("1. Aggiungi contatto\n");
  printf("2. Elimina contatto\n");
  printf("3. Visualizza contatti\n");
  printf("4. Esci\n");
  printf("Inserisci il numero dell'opzione: ");
}

void main_menu(int sfd,char *buffer,char *command,int is_authenticated){  //gestione ed invio al server dei diversi case
  gui_1();

  while(1){
    clear_buffers(buffer,command);
    int choice=0;
    char line[256];
    if(fgets(line,sizeof(line),stdin)!=NULL){
      line[strcspn(line,"\n")]=0;
      if(strlen(line)==1 && isdigit(line[0])){
        choice=line[0]-'0';
      }
      else {
      print_error("input non valido\n");
      clear_buffers(buffer,command);
      memset(line, 0, sizeof(line));
      main_menu(sfd,buffer,command,is_authenticated);
      }
    }

    switch(choice){
    case 1:   //aggiungi contatto
          if(is_authenticated==0){
            print_error("You must be logged-in\n");
            clear_buffers(buffer,command);
            main_menu(sfd,buffer,command,is_authenticated);
          }
          clear_buffers(buffer,command);
          char nome_contatto[50];
          printf("Inserisci nome: ");
          fgets(nome_contatto, sizeof(nome_contatto), stdin);
          nome_contatto[strcspn(nome_contatto, "\n")] = 0;
          if(validate_username(nome_contatto)==0){
            print_error("Nome non valido, può contenere solo caratteri alfanumerici e underscore");
            clear_buffers(buffer,command);
            main_menu(sfd,buffer,command,is_authenticated);
          }
          char numero_contatto[50];
          printf("Inserisci numero: ");
          fgets(numero_contatto,sizeof(numero_contatto),stdin);
          numero_contatto[strcspn(numero_contatto,"\n")]=0;
          for(int i=0;i<numero_contatto[i]!='\0';i++){
            if(!isdigit(numero_contatto[i])&&numero_contatto[i]!='+'){
              print_error("numero non valido");
              clear_buffers(buffer,command);
              memset(numero_contatto,0,sizeof(numero_contatto));
              main_menu(sfd,buffer,command,is_authenticated);
            }
          }

          snprintf(command,256, "ADD %s %s", nome_contatto,numero_contatto);
          send(sfd, command, strlen(command), 0); //send di nome e numero
          recv(sfd, buffer, 255, 0); //ricezione messaggio
          printf("\n%s\n\n", buffer);
          clear_buffers(buffer,command);
          main_menu(sfd,buffer,command,is_authenticated);


    case 2:  //Elimina contatto
          if(is_authenticated==0){
          print_error("You must be logged-in\n");
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
          char contatti_ricevuti[10000];   //buffer per contenere tuuta la rubrica (circa 200 max)
          snprintf(command, sizeof(command), "VIEW");
          send(sfd, command, strlen(command), 0);
          recv(sfd, contatti_ricevuti, sizeof(contatti_ricevuti) - 1, 0);  //ricezione rubrica

          contatti_ricevuti[10000] = '\0';
          char *singolo_utente=strtok(contatti_ricevuti,"\n");  //lettura primo contatto
          char numero_ricevuto[50];
          char nome_ricevuto[50];

          printf("+----------------------------+\n");
          printf("|        RUBRICA CONTATTI     |\n");
          printf("+----------------------------+\n");
          while(singolo_utente!=NULL){
            sscanf(singolo_utente,"%s %s",nome_ricevuto,numero_ricevuto);
            printf("| %-20s | %10s |\n", nome_ricevuto, numero_ricevuto);
            singolo_utente=strtok(NULL,"\n");  //lettura ulteriori contatti
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
          print_error("Opzione non valida\n");
          clear_buffers(buffer,command);
          main_menu(sfd,buffer,command,is_authenticated);
         }
      }
   }

int ricerca_utente(char *buffer,char *confronto){  //controllo se utente esistente

  FILE *f1=fopen("Sicurezza.txt","r");  //apertura file con credenziali
  if(f1==NULL){
    printf("error opening f1.txt\n");
    exit(EXIT_FAILURE);
  }
  char line[256];
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


void login(int sfd){  //gestione registrazione/accesso
  int is_authenticated=0;
  char buffer[200];
  int choice;
  char line[100];
  char command[256];
  clear_buffers(buffer,command);

  gui();
  if(fgets(line,sizeof(line),stdin)!=NULL){
    line[strcspn(line,"\n")]=0;
    if(strlen(line)==1 && isdigit(line[0])){
      choice=line[0]-'0';
    }
    else {
    print_error("input non valido\n");
    clear_buffers(buffer,command);
    memset(line, 0, sizeof(line));
    login(sfd);
    }
  }

  switch(choice){

  case 1:   //login
    char username[50];
    printf("Inserisci utente: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    if(!validate_username(username)){
      print_error("Username non valido. Deve essere lungo tra 3 e 20 caratteri e contenere solo lettere, numeri e underscore.\n");
      clear_buffers(buffer,command);
      memset(username,0,sizeof(username));
      login(sfd);
    }
    char password[50];
    printf("Inserisci password: ");
    fgets(password,sizeof(password),stdin);
    password[strcspn(password,"\n")]=0;
    if(!validate_password(password)){
      print_error("Password non valida. Deve essere lunga tra 8 e 30 caratteri e contenere almeno una lettera maiuscola, una minuscola, un numero e un carattere speciale.\n");
      clear_buffers(buffer,command);
      memset(password,0,sizeof(password));
      memset(username,0,sizeof(username));
      login(sfd);
    }
    snprintf(command, sizeof(command), "LOGIN %s %s", username,password);
    send(sfd, command, strlen(command), 0);     //invio username e password
    recv(sfd, buffer, sizeof(buffer) - 1, 0);

    if(strncmp(buffer,"LOGIN effettuato",16)==0){  //successo
      print_success("LOGIN effettuato");
      is_authenticated=1;
      clear_buffers(buffer,command);
      main_menu(sfd,buffer,command,is_authenticated);
      break;
    }
    else{         //errore
      buffer[255] = '\0';
      print_error("Nome utente o password errato\n");
      clear_buffers(buffer,command);
      login(sfd);
  }

  case 2:       //sign-up
  char new_username[50];
  printf("Inserisci utente: ");
  fgets(new_username, sizeof(new_username), stdin);
  new_username[strcspn(new_username, "\n")] = 0;
  if(validate_username(new_username)==0){
    print_error("Username non valido. Deve essere lungo tra 3 e 20 caratteri e contenere solo lettere, numeri e underscore.\n");
    clear_buffers(buffer,command);
    login(sfd);
  }

  char new_password[50];
  printf("Inserisci password: ");
  fgets(new_password,sizeof(new_password),stdin);
  new_password[strcspn(new_password,"\n")]=0;
  if (!validate_password(new_password)) {
    print_error("Password non valida. Deve essere lunga tra 8 e 30 caratteri e contenere almeno una lettera maiuscola, una minuscola, un numero e un carattere speciale.\n");
    login(sfd);
  }

  snprintf(command, sizeof(command), "SIGNUP %s %s", new_username,new_password);
  send(sfd, command, strlen(command), 0);
  ssize_t recv_len=recv(sfd, buffer, sizeof(buffer) - 1, 0);
  buffer[recv_len]='\0';
  if(strncmp(buffer,"SIGN UP riuscito",16)==0){       //successo
    print_success("SIGN UP riuscito\n");
    is_authenticated=1;
    clear_buffers(buffer,command);
    main_menu(sfd,buffer,command,is_authenticated);
  }
  else{       //errore
    print_error("Utente già registrato\n");
    clear_buffers(buffer,command);
    login(sfd);
  }

  case 3:       //accesso senza account (limitato)
    send(sfd,"NOACC ",6,0);
    recv(sfd,buffer,sizeof(buffer)-1,0);
    print_warning("!Warning! Se non esegui l'accesso non potrai aggiungere/rimuovere contatti\n");
    clear_buffers(buffer,command);
    main_menu(sfd,buffer,command,is_authenticated);

  default:
    clear_buffers(buffer,command);
    printf("Opzione non valida\n");
    break;
    }
  }


void server_login(int sfd){       //ricezione e gestione segnali in arrivo da client
  while(1){
  int cfd=accept(sfd,NULL,NULL);
  if(cfd==-1){
    print_error("error accepting connetion\n");
    continue;
  }
  printf("client conneted\n");

  char buffer[256];
  while(1){       //inizio loop ricezione
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
    memset(buffer,0,sizeof(buffer));
    memset(username,0,sizeof(username));
    memset(password,0,sizeof(password));
  }
}
  if(result==0){
  //utente non trovato
  send(cfd,"nome utente o password errata\n",31,0);
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
    FILE *f1=fopen("Contatti.txt","r+");
    if(f1==NULL){
      printf("error opening Contatti.txt\n");
      exit(EXIT_FAILURE);
    }
    //controllo se contatto già esistente
    char nome_ricevuto[50];
    int found=0;
    sscanf(buffer+4,"%49s",nome_ricevuto);
    char riga[256];
    while(fgets(riga,sizeof(riga),f1)!=NULL){
      char nome_nel_file[50];
      sscanf(riga,"%49s",nome_nel_file);
        if(strcmp(nome_nel_file,nome_ricevuto)==0){
          found=1;
        }
      }
    if(found==0){
    fprintf(f1,"\n%s",buffer+4);
    fclose(f1);
    send(cfd,"Contatto aggiunto",17,0);
  }
  else{
    send(cfd,"Contatto già esistente",23,0);
    fclose(f1);
  }
}
  else if(strncmp(buffer,"DELETE ",7)==0){
  FILE *f1=fopen("Contatti.txt","r");
  FILE *f2=fopen("temp.txt","w");  //file temporaneo in cui salvare tutti i contatti meno che quello da eliminare
  if(f1==NULL||f2==NULL){
    perror("Error opening file");
    send(cfd,"Errore nell'eliminazione del contatto\n",39,0);
    if(f1) fclose(f1);
    if(f2) fclose(f2);
    continue;
  }
  char nome_ricevuto[50];
  int found=0;
  char riga[256];

  sscanf(buffer+7,"%49s",nome_ricevuto);
  while(fgets(riga,sizeof(riga),f1)!=NULL){
    char nome_nel_file[50];
    char numero_nel_file[50];
    if(sscanf(riga,"%49s %49s",nome_nel_file,numero_nel_file) == 2){
      if(strcmp(nome_nel_file,nome_ricevuto)!=0){
        fprintf(f2,"%s %s\n",nome_nel_file,numero_nel_file);  //scrittura nel file temporaneo
      } else {
        found=1;
      }
    }
  }
  fflush(f2);
  fclose(f1);
  fclose(f2);

  if(found==0){
    remove("temp.txt");
    send(cfd,"Nome utente non trovato\n",25,0);
  }
  else{
    if(remove("Contatti.txt") != 0){   //elimino file Contatti.txt (contiene contatto da eliminare)
      perror("Error deleting original file");
      remove("temp.txt");
      send(cfd,"Errore nell'eliminazione del contatto\n",39,0);
      continue;
    }
    if(rename("temp.txt","Contatti.txt") != 0){   //rinomino temp.txt
      perror("Error renaming file");
      send(cfd,"Errore nell'eliminazione del contatto\n",39,0);
      continue;
    }
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
    char rubrica[10000] = "Contatti:\n\n";    //contiene circa 200 contatti max
    char riga[256];
    while(fgets(riga,sizeof(riga),f1)!=NULL){
      char nome[50], numero[50];
      if(sscanf(riga,"%s %s",nome,numero) == 2){
        strcat(rubrica, riga);
      }
    }
    fclose(f1);
    if(strlen(rubrica)==strlen("Contatti:\n\n")){
      strcat(rubrica,"Nessun contatto trovato\n");
    }
    send(cfd,rubrica,strlen(rubrica),0);
      }
    }
  }
}

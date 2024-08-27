#ifndef UTILITY_H
#define UTILITY_H

void gui();
void gui_1();
int ricerca_utente(char *buffer, char *confronto);
void handle_login(int client_socket);
void login(int sfd);
void server_login(int sfd);
void main_menu(int sfd,char *buffer,char *command,int is_authenticated);
void clear_buffers(char *buffer,char *command);

#endif

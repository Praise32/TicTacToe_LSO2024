#ifndef FUNZIONI_H
#define FUNZIONI_H

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define CLEAR_SCREEN() printf("\033[H\033[J")
#define MAX_RETRIES 3

// Dichiarazioni delle funzioni
void handle_menu(int socket_fd);
void start_game(int socket_fd);
void play_again_menu(int socket_fd, int is_winner);
int get_valid_menu_choice(int min, int max);
int get_valid_move(char *input);

#endif  // FUNZIONI_H
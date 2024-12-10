#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gui.h"
#include "time.h"

// Per il client
void get_server_info(int argc, char *argv[], char *server_name, int *server_port, int* num_players);
int connect_to_server(const char *server_name, int server_port);
void generate_client_params(int *time_to_respond);

// Funzioni per la gestione delle partite di Tris
int create_game(int socket_fd);
int join_game(int socket_fd);
void play_game(int socket_fd);
int send_move(int socket_fd, int row, int col);
void receive_game_update(int socket_fd, char *buffer);
void handle_game_end(int socket_fd, const char *game_status);

// Funzioni di gestione dei messaggi
void receive_message_from_server(int socket_fd, char *buffer);

// Funzioni per la chiusura delle risorse
void close_resources(int socket_fd);

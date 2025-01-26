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

// Nuove definizioni
#define GAME_TIMEOUT 30
#define MAX_RETRIES 3
#define RETRY_DELAY 2

// Struttura per lo stato del gioco
typedef struct {
    int socket_fd;
    int game_id;
    char symbol;
    char board[3][3];
} GameState;

// Per il client
void get_server_info(int argc, char *argv[], char *server_name, int *server_port, int* num_players);
int connect_to_server(const char *server_name, int server_port);
int connect_with_retry(const char *server_name, int server_port);
void generate_client_params(int *time_to_respond);

// Funzioni per la gestione delle partite di Tris
int create_game(GameState *state);
int join_game(GameState *state, int game_id);
void play_game(GameState *state);
int send_move(GameState *state, int row, int col);
void receive_game_update(int socket_fd, char *buffer);
void handle_game_end(GameState *state, const char *game_status);

// Funzioni di gestione dello stato del gioco
void initialize_game_state(GameState *state, int socket_fd);
void cleanup_resources(GameState *state);
void update_game_board(char board[3][3], int row, int col, char symbol);

// Funzioni di gestione dei messaggi
void receive_message_from_server(int socket_fd, char *buffer);
void set_socket_timeout(int socket_fd, int seconds);
void print_game_status(const char *message);
void print_loading_animation();
void print_main_menu();

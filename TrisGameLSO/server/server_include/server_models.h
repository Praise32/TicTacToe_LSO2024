#ifndef SERVER_MODELS_H
#define SERVER_MODELS_H

#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdatomic.h>

// Costanti
#define MAX_PLAYERS 100
#define MAX_GAMES 50
#define BUFFER_SIZE 1024




// Stato della partita
typedef enum {
    NEW,
    WAITING,
    IN_PROGRESS,
    FINISHED
} GameState;

// Stato di fine partita
typedef enum {
    VICTORY,
    DEFEAT,
    DRAW
} GameResult;

// Struttura del giocatore
typedef struct {
    int id;
    int socket;
    char name[50];
    int current_game_id; // Indica l'ID della partita corrente
    int is_playing;
} Player;

// Struttura di una partita
typedef struct {
    int id;
    Player *player1;
    Player *player2;
    char board[3][3]; // Tabellone 3x3
    GameState state;
    pthread_mutex_t game_mutex;
    int current_turn; // 1 o 2 (giocatore 1 o 2)
} GameSession;

// Struttura globale che mantiene le partite attive
typedef struct {
    GameSession *games[MAX_GAMES];
    Player *players[MAX_PLAYERS];
    pthread_mutex_t global_mutex;
} GameManager;

typedef struct pthread_arg_t {
    GameManager *gamemanager;
    int new_socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;


// Variabile globale per il socket del server
extern int server_socket; // Variabile globale dichiarata qui

// Funzioni di connessione
void setup_server_socket(int server_port, int *socket_fd);
void accept_connections(int socket_fd, GameManager *gamemanager);
void signal_handler(int signal_number);
void *client_handler(void *arg);


// Funzioni del giocatore
void init_player(Player *player, int id, int socket, const char *name);
void display_player_info(Player *player);

// Funzioni di gestione delle sessioni
void init_game_manager(GameManager *manager);
int add_player(GameManager *manager, Player *player);
int create_game(GameManager *manager, Player *player);

// Funzioni di gestione della partita
void init_game(GameSession *game, int game_id, Player *player1);
void print_board(GameSession *game);
int make_move(GameSession *game, int player_number, int row, int col);
int check_winner(GameSession *game);



#endif

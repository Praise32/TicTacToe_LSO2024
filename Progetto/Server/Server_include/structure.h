#ifndef STRUCTURE_H
#define STRUCTURE_H

#define MAX_GAMES 10
#define MAX_GAME_NAME 32
#define PORT 12345
#define MAX_PLAYERS 2
#define BUFFER_SIZE 1024
#define CLEAR_SCREEN() printf("\033[H\033[J")

typedef struct {
    int player_sockets[MAX_PLAYERS];
    char board[3][3];
    char current_player;
} Game;

typedef struct {
    char name[MAX_GAME_NAME];
    int host_socket;
    int guest_socket;
    int is_active;
} GameSession;

typedef struct {
    GameSession games[MAX_GAMES];
    int num_games;
} Lobby;



#endif
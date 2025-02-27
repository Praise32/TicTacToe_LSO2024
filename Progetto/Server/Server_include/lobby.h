#ifndef LOBBY_H
#define LOBBY_H

#define MAX_GAMES 10
#define MAX_GAME_NAME 32

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

void init_lobby(Lobby *lobby);
int create_game_session(Lobby *lobby, const char *name, int host_socket);
int join_game_session(Lobby *lobby, int game_id, int guest_socket);
void list_games(Lobby *lobby, char *buffer);
void remove_game_session(Lobby *lobby, int game_id);

#endif

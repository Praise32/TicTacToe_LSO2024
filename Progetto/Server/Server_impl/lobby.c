#include <string.h>
#include <stdio.h>
#include "../Server_include/lobby.h"

void init_lobby(Lobby *lobby) {
    lobby->num_games = 0;
    for (int i = 0; i < MAX_GAMES; i++) {
        lobby->games[i].is_active = 0;
    }
}

int create_game_session(Lobby *lobby, const char *name, int host_socket) {
    if (lobby->num_games >= MAX_GAMES) {
        return -1;
    }
    
    for (int i = 0; i < MAX_GAMES; i++) {
        if (!lobby->games[i].is_active) {
            strncpy(lobby->games[i].name, name, MAX_GAME_NAME - 1);
            lobby->games[i].host_socket = host_socket;
            lobby->games[i].guest_socket = -1;
            lobby->games[i].is_active = 1;
            lobby->num_games++;
            return i;
        }
    }
    return -1;
}

int join_game_session(Lobby *lobby, int game_id, int guest_socket) {
    if (game_id < 0 || game_id >= MAX_GAMES || !lobby->games[game_id].is_active) {
        return -1;
    }
    
    lobby->games[game_id].guest_socket = guest_socket;
    return 0;
}

void list_games(Lobby *lobby, char *buffer) {
    buffer[0] = '\0';
    char temp[128];
    
    if (lobby->num_games == 0) {
        strcpy(buffer, "No games available.\n");
        return;
    }
    
    for (int i = 0; i < MAX_GAMES; i++) {
        if (lobby->games[i].is_active) {
            snprintf(temp, sizeof(temp), "%d. %s\n", i, lobby->games[i].name);
            strcat(buffer, temp);
        }
    }
}

void remove_game_session(Lobby *lobby, int game_id) {
    if (game_id >= 0 && game_id < MAX_GAMES && lobby->games[game_id].is_active) {
        lobby->games[game_id].is_active = 0;
        lobby->num_games--;
    }
}

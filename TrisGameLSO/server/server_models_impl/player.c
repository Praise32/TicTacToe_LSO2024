#include "../server_include/server_models.h"
#include <stdio.h>
#include <stdlib.h>

void init_player(Player *player, int id, int socket, const char *name) {
    player->id = id;
    player->socket = socket;
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->current_game_id = -1; // Nessuna partita attualmente
    player->is_playing = 0;
}

void display_player_info(Player *player) {
    printf("Giocatore: ID=%d, Nome=%s, Socket=%d, Partita=%d, In Gioco=%d\n", 
           player->id, player->name, player->socket, player->current_game_id, player->is_playing);
}

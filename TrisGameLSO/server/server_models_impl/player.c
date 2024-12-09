#include "../server_include/server_models.h"
#include <stdio.h>

void init_player(Player *player, int socket) {
    player->socket = socket;
    player->in_game = 0;
}

void display_player_info(Player *player) {
    printf("Giocatore con socket: %d, in partita: %d\n", player->socket, player->in_game);
}

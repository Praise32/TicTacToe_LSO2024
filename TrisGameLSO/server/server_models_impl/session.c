#include "../server_include/server_models.h"
#include <stdio.h>
#include <stdlib.h>

void init_game_manager(GameManager *manager) {
    for (int i = 0; i < MAX_GAMES; i++) {
        manager->games[i] = NULL;
    }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        manager->players[i] = NULL;
    }
    pthread_mutex_init(&manager->global_mutex, NULL);
}

int add_player(GameManager *manager, Player *player) {
    pthread_mutex_lock(&manager->global_mutex);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (manager->players[i] == NULL) {
            manager->players[i] = player;
            pthread_mutex_unlock(&manager->global_mutex);
            return 1; // Giocatore aggiunto con successo
        }
    }
    pthread_mutex_unlock(&manager->global_mutex);
    return 0; // Impossibile aggiungere il giocatore
}

int create_game(GameManager *manager, Player *player) {
    pthread_mutex_lock(&manager->global_mutex);
    for (int i = 0; i < MAX_GAMES; i++) {
        if (manager->games[i] == NULL) {
            GameSession *game = (GameSession *)malloc(sizeof(GameSession));
            init_game(game, i, player);
            manager->games[i] = game;
            player->current_game_id = i;
            pthread_mutex_unlock(&manager->global_mutex);
            return i; // ID della nuova partita
        }
    }
    pthread_mutex_unlock(&manager->global_mutex);
    return -1; // Impossibile creare una nuova partita
}

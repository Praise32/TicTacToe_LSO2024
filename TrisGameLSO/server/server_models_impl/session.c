#include "../server_include/server_models.h"
#include <stdio.h>
#include <stdlib.h>

Session active_sessions[10];

void init_session(Session *session, int player_socket) {
    session->player1_socket = player_socket;
    session->player2_socket = -1; // In attesa del secondo giocatore
    init_game(&session->game, player_socket);
}

void add_player_to_session(Session *session, int player_socket) {
    if (session->player2_socket == -1) {
        session->player2_socket = player_socket;
    }
}

void display_sessions() {
    for (int i = 0; i < 10; i++) {
        if (active_sessions[i].player1_socket != -1) {
            printf("Sessione attiva: ID %d, Player 1: %d, Player 2: %d\n", 
                active_sessions[i].game.id, 
                active_sessions[i].player1_socket, 
                active_sessions[i].player2_socket);
        }
    }
}

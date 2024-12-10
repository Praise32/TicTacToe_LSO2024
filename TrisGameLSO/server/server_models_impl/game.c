#include "../server_include/server_models.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void init_game(GameSession *game, int player1_socket) {
    game->id = rand() % 10000; // Generazione di un ID univoco
    game->player1_socket = player1_socket;
    game->player2_socket = -1; // Nessun giocatore 2 inizialmente
    memset(game->board, '_', sizeof(game->board)); // Inizializzazione del tabellone
    strcpy(game->state, "attesa"); // Stato iniziale della partita
    game->current_turn = 1; // Inizia il giocatore 1
}

void print_board(GameSession *game) {
    printf("\nTabellone di gioco (ID: %d):\n", game->id);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", game->board[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---+---+---\n");
    }
    printf("\n");
}

int make_move(GameSession *game, int player, int row, int col) {
    if (game->board[row][col] == '_') {
        game->board[row][col] = (player == 1) ? 'X' : 'O'; // X per player 1, O per player 2
        return 1; // Mossa accettata
    } else {
        return 0; // Mossa non valida (posizione già occupata)
    }
}

int check_victory(GameSession *game) {
    // Controlla righe, colonne e diagonali
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] == game->board[i][1] && game->board[i][1] == game->board[i][2] && game->board[i][0] != '_')
            return 1; // Vittoria
        if (game->board[0][i] == game->board[1][i] && game->board[1][i] == game->board[2][i] && game->board[0][i] != '_')
            return 1; // Vittoria
    }
    if (game->board[0][0] == game->board[1][1] && game->board[1][1] == game->board[2][2] && game->board[0][0] != '_')
        return 1; // Diagonale principale
    if (game->board[0][2] == game->board[1][1] && game->board[1][1] == game->board[2][0] && game->board[0][2] != '_')
        return 1; // Diagonale secondaria

    return 0; // Nessun vincitore
}

int check_draw(GameSession *game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == '_')
                return 0; // Ci sono ancora mosse disponibili
        }
    }
    return 1; // Pareggio
}

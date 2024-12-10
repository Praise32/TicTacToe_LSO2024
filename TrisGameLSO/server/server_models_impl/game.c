#include "../server_include/server_models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_game(GameSession *game, int game_id, Player *player1) {
    game->id = game_id;
    game->player1 = player1;
    game->player2 = NULL; // In attesa del giocatore 2
    memset(game->board, '_', sizeof(game->board));
    game->state = NEW;
    pthread_mutex_init(&game->game_mutex, NULL);
    game->current_turn = 1; // Inizia il giocatore 1
}

void print_board(GameSession *game) {
    printf("Tabellone (ID Partita: %d):\n", game->id);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", game->board[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---+---+---\n");
    }
}

int make_move(GameSession *game, int player_number, int row, int col) {
    if (game->board[row][col] == '_') {
        game->board[row][col] = (player_number == 1) ? 'X' : 'O';
        return 1; // Mossa accettata
    } 
    return 0; // Mossa non valida
}

int check_winner(GameSession *game) {
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] == game->board[i][1] && game->board[i][1] == game->board[i][2] && game->board[i][0] != '_')
            return 1;
        if (game->board[0][i] == game->board[1][i] && game->board[1][i] == game->board[2][i] && game->board[0][i] != '_')
            return 1;
    }
    if (game->board[0][0] == game->board[1][1] && game->board[1][1] == game->board[2][2] && game->board[0][0] != '_')
        return 1;
    if (game->board[0][2] == game->board[1][1] && game->board[1][1] == game->board[2][0] && game->board[0][2] != '_')
        return 1;
    return 0;
}

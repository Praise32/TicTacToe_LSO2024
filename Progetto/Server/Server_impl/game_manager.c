#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../Server_include/structure.h"
#include "../Server_include/game_logic.h"

// Inizializza un tabellone vuoto
void initialize_board(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

// Mostra il tabellone
void display_board(char board[3][3]) {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

// Controlla se la mossa è valida
bool is_valid_move(char board[3][3], int row, int col) {
    return row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ';
}

// Effettua una mossa
bool make_move(char board[3][3], int row, int col, char player) {
    if (is_valid_move(board, row, col)) {
        board[row][col] = player;
        return true;
    }
    return false;
}

// Controlla se c'è un vincitore
char check_winner(char board[3][3]) {
    // Controlla righe e colonne
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return board[i][0];
        }
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return board[0][i];
        }
    }

    // Controlla diagonali
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[0][2];
    }

    // Nessun vincitore
    return ' ';
}

// Controlla se il tabellone è pieno
bool is_draw(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

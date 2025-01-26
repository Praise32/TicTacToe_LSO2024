#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>

// Funzioni per la gestione del gioco
void initialize_board(char board[3][3]);
void display_board(char board[3][3]);
bool is_valid_move(char board[3][3], int row, int col);
bool make_move(char board[3][3], int row, int col, char player);
char check_winner(char board[3][3]);
bool is_draw(char board[3][3]);

#endif

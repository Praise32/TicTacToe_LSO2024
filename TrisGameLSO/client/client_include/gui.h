#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 60
#define HEIGHT 30
// Colore blu (sequenza ANSI)
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

// Funzioni per la GUI
void clear_screen();
void print_welcome_message();
void print_game_board(char board[3][3]);
void print_move_prompt();
void print_victory_message();
void print_draw_message();
void print_defeat_message();

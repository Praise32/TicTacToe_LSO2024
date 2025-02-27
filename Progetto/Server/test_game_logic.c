#include <stdio.h>
#include "game_logic.h"

int main() {
    char board[3][3];
    initialize_board(board);
    display_board(board);

    make_move(board, 0, 0, 'X');
    make_move(board, 1, 1, 'O');
    display_board(board);

    if (check_winner(board) != ' ') {
        printf("Il vincitore è: %c\n", check_winner(board));
    } else if (is_draw(board)) {
        printf("La partita è un pareggio.\n");
    } else {
        printf("La partita è ancora in corso.\n");
    }

    return 0;
}

#include "client_include/client_models.h"
#include "client_include/gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int server_socket;

int main() {
    print_welcome_message();

    connect_to_server();

    char buffer[1024];
    char board[3][3];

    printf("Inserisci il tuo nome: ");
    char player_name[50];
    scanf("%s", player_name);
    send_message_to_server(player_name);

    while (1) {
        receive_message_from_server(buffer, sizeof(buffer));

        if (strcmp(buffer, "start_game") == 0) {
            printf("Il gioco è iniziato!\n");
        } else if (strcmp(buffer, "your_turn") == 0) {
            receive_game_board(board);
            print_game_board(board);
            print_move_prompt();

            int row, col;
            printf("Inserisci la tua mossa (riga,colonna): ");
            scanf("%d,%d", &row, &col);

            char move_command[50];
            snprintf(move_command, sizeof(move_command), "MOVE:%d,%d", row, col);
            send_message_to_server(move_command);
        } else if (strcmp(buffer, "game_over_victory") == 0) {
            print_victory_message();
            break;
        } else if (strcmp(buffer, "game_over_defeat") == 0) {
            print_defeat_message();
            break;
        } else if (strcmp(buffer, "game_over_draw") == 0) {
            print_draw_message();
            break;
        } else {
            printf("Messaggio dal server: %s\n", buffer);
        }
    }

    printf("Connessione chiusa.\n");
    close(server_socket);
    return 0;
}

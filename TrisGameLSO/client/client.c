#include "client_include/client_models.h"
#include "client_include/gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Mostra il messaggio di benvenuto
    print_welcome_message();

    // Connessione al server
    connect_to_server();

    // Buffer per ricevere messaggi dal server
    char buffer[1024];

    // Chiede all'utente di inserire il proprio nome
    printf("Inserisci il tuo nome: ");
    char player_name[50];
    scanf("%s", player_name);
    send_message_to_server(player_name); // Invio del nome al server

    // Ciclo principale del client
    while (1) {
        // Riceve un messaggio dal server
        receive_message_from_server(buffer, sizeof(buffer));

        if (strcmp(buffer, "start_game") == 0) {
            printf("Il gioco è iniziato!\n");
        } 
        else if (strcmp(buffer, "your_turn") == 0) {
            // Mostra il tabellone ricevuto
            char board[3][3];
            receive_game_board(board); // Riceve il tabellone aggiornato
            print_game_board(board);
            print_move_prompt();

            int row, col;
            printf("Inserisci la tua mossa (formato riga,colonna): ");
            scanf("%d,%d", &row, &col);

            char move_command[50];
            sprintf(move_command, "MOVE:%d,%d", row, col);
            send_message_to_server(move_command);
        } 
        else if (strcmp(buffer, "game_over_victory") == 0) {
            print_victory_message();
            break;
        } 
        else if (strcmp(buffer, "game_over_defeat") == 0) {
            print_defeat_message();
            break;
        } 
        else if (strcmp(buffer, "game_over_draw") == 0) {
            print_draw_message();
            break;
        } 
        else {
            printf("Messaggio dal server: %s\n", buffer);
        }
    }

    printf("Connessione chiusa.\n");
    close(server_socket);
    return 0;
}

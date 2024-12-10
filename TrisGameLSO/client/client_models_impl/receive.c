#include "../client_include/client_models.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

extern int server_socket; // Definito in connection.c

void receive_game_board(char board[3][3]) {
    char buffer[1024];
    receive_message_from_server(buffer, sizeof(buffer));

    // Parsing della stringa ricevuta (es. "X,O,_,_,X,O,_,_,_")
    char *token = strtok(buffer, ",");
    for (int i = 0; i < 3 && token != NULL; i++) {
        for (int j = 0; j < 3 && token != NULL; j++) {
            board[i][j] = token[0];
            token = strtok(NULL, ",");
        }
    }
}

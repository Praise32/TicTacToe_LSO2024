#include "../client_include/client_models.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Funzione per ricevere un messaggio generico dal server
void receive_message_from_server(char *buffer, int buffer_size) {
    int bytes_received = recv(server_socket, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        perror("Errore durante la ricezione del messaggio");
    } else {
        buffer[bytes_received] = '\0'; // Termina la stringa ricevuta
    }
}

// Funzione per ricevere e aggiornare il tabellone di gioco
void receive_game_board(char board[3][3]) {
    char buffer[1024];
    receive_message_from_server(buffer, sizeof(buffer));

    // Parsing della stringa ricevuta (formato es: "X,O,_,_,X,O,_,_,_")
    char *token = strtok(buffer, ",");
    int i = 0, j = 0;

    while (token != NULL) {
        board[i][j] = token[0];
        j++;
        if (j == 3) {
            i++;
            j = 0;
        }
        token = strtok(NULL, ",");
    }
}

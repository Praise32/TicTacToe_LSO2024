#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../server_include/server_models.h"

// Funzione per gestire i client connessi
void *gestisci_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg); // Libera la memoria dell'argomento passato

    char buffer[BUFFER_SIZE];
    SessionePartite sessione; // Si può passare come parametro o rendere globale

    printf("Client connesso: socket_fd=%d\n", client_fd);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Legge il comando inviato dal client
        int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            printf("Connessione chiusa dal client: socket_fd=%d\n", client_fd);
            close(client_fd);
            return NULL;
        }

        buffer[bytes_read] = '\0';
        printf("Ricevuto comando: %s\n", buffer);

        // Interpreta il comando
        if (strcmp(buffer, "CREATE_GAME") == 0) {
            int game_id = crea_partita(&sessione);
            if (game_id != -1) {
                snprintf(buffer, BUFFER_SIZE, "GAME_CREATED %d", game_id);
            } else {
                snprintf(buffer, BUFFER_SIZE, "ERROR");
            }
            write(client_fd, buffer, strlen(buffer));
        } else if (strncmp(buffer, "JOIN_GAME", 9) == 0) {
            int game_id = atoi(buffer + 10);
            if (cerca_partita_disponibile(&sessione) == game_id) {
                snprintf(buffer, BUFFER_SIZE, "GAME_JOINED %d", game_id);
            } else {
                snprintf(buffer, BUFFER_SIZE, "ERROR");
            }
            write(client_fd, buffer, strlen(buffer));
        } else if (strncmp(buffer, "MOVE", 4) == 0) {
            int row, col;
            sscanf(buffer + 5, "%d %d", &row, &col);
            snprintf(buffer, BUFFER_SIZE, "MOVE_ACCEPTED %d %d", row, col);
            write(client_fd, buffer, strlen(buffer));
        } else {
            snprintf(buffer, BUFFER_SIZE, "UNKNOWN_COMMAND");
            write(client_fd, buffer, strlen(buffer));
        }
    }

    close(client_fd);
    return NULL;
}

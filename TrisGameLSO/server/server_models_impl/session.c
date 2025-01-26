#include "../server_include/server_models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

// Configura il server socket per ascoltare le connessioni in entrata
void setup_server_socket(int server_port, int *socket_fd) {
    struct sockaddr_in server_address;

    // Impostazione della struttura del server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Creazione del socket
    if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Associazione del socket all'indirizzo del server
    if (bind(*socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(1);
    }

    // Inizia ad ascoltare le connessioni in entrata
    if (listen(*socket_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server in ascolto sulla porta %d...\n", server_port);
}

// Funzioni di utilità per la sincronizzazione

// Invia un messaggio al client
void send_message(int socket_fd, const char *message) {
    if (send(socket_fd, message, strlen(message), 0) == -1) {
        perror("Errore nell'invio del messaggio");
    }
}

// Riceve un messaggio dal client
void receive_message(int socket_fd, char *buffer) {
    int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Errore nella ricezione del messaggio");
    } else if (bytes_received == 0) {
        printf("Il client ha chiuso la connessione.\n");
    }
}

// Define client_handler for use across files
void log_client_action(int client_socket, const char* action) {
    time_t now;
    time(&now);
    char* date = ctime(&now);
    date[strlen(date) - 1] = '\0';  // Remove newline
    printf("[%s] Client %d: %s\n", date, client_socket, action);
}

void *client_handler(void *arg) {
    ClientData *data = (ClientData *)arg;
    int client_socket = data->client_socket;
    SessionePartite *sessione = data->sessione;
    char buffer[BUFFER_SIZE];

    log_client_action(client_socket, "Connected");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (received <= 0) {
            log_client_action(client_socket, "Disconnected");
            break;
        }

        log_client_action(client_socket, buffer);

        if (strncmp(buffer, "CREATE_GAME", 11) == 0) {
            int game_id = crea_partita(client_socket);
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "GAME_CREATED %d", game_id);
            send_message(client_socket, response);
        } else if (strncmp(buffer, "JOIN_GAME", 9) == 0) {
            int game_id;
            sscanf(buffer + 10, "%d", &game_id);
            if (unisci_a_partita(client_socket, game_id) == 0) {
                send_message(client_socket, "GAME_JOINED");
            } else {
                send_message(client_socket, "GAME_JOIN_FAILED");
            }
        } else if (strncmp(buffer, "MOVE", 4) == 0) {
            int game_id, row, col;
            sscanf(buffer + 5, "%d %d %d", &game_id, &row, &col);
            if (invia_mossa(client_socket, game_id, row, col) == 0) {
                send_message(client_socket, "MOVE_ACCEPTED");
                // Aggiorna e notifica gli altri giocatori
                aggiorna_partita(client_socket, game_id);
            } else {
                send_message(client_socket, "MOVE_REJECTED");
            }
        }
    }

    close(client_socket);
    free(data);
    return NULL;
}

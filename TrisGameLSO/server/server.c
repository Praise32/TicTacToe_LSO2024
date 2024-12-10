#include "server_include/server_models.h"
#include "server_include/colors.h"
#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdatomic.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100
#define MAX_GAMES 50

int server_socket;

void start_server(int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Errore nella creazione del socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nel bind del socket");
        exit(1);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Errore nel listen");
        exit(1);
    }

    printf(COLOR_GREEN "Server in ascolto sulla porta %d\n" COLOR_RESET, port);
}

void *client_handler(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    // Example client interaction (you can expand this based on game logic)
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (received <= 0) {
            perror("Errore nella ricezione dei dati");
            close(client_socket);
            return NULL;
        }
        
        // Handle messages (create game, join game, make move, etc.)
        if (strncmp(buffer, "create_game", 11) == 0) {
            // Logic to create a game
            printf("Creazione della partita richiesta\n");
        } else if (strncmp(buffer, "join_game", 9) == 0) {
            // Logic to join an existing game
            printf("Unione a partita richiesta\n");
        }
        // Add more cases for different actions (move, exit game, etc.)
    }

    return NULL;
}

void accept_connections(int socket_fd, SessionePartite *sessione) {
    while (1) {
        int client_socket = accept(socket_fd, NULL, NULL);
        if (client_socket < 0) {
            perror("Errore nell'accettazione del client");
            continue;
        }
        printf(COLOR_BLUE "Nuovo client connesso con socket %d\n" COLOR_RESET, client_socket);

        // Crea una struttura ClientData per passare i dati al thread
        ClientData *data = (ClientData *)malloc(sizeof(ClientData));
        if (data == NULL) {
            perror("Errore nell'allocazione della memoria per i dati del client");
            close(client_socket);  // Chiudi il socket se non riusciamo ad allocare memoria
            continue;
        }

        data->client_socket = client_socket;
        data->sessione = sessione;

        // Crea un nuovo thread per gestire il client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, client_handler, data) != 0) {
            perror("Errore nella creazione del thread del client");
            free(data);  // Libera la memoria allocata in caso di errore
            close(client_socket);  // Chiudi il socket
            continue;
        }

        // Non aspettare il thread in modo sincrono, lascialo lavorare in parallelo
        pthread_detach(client_thread);  // Detach per evitare di dover fare join manualmente
    }
}

// Funzione gestore del segnale SIGINT
void signal_handler(int signal_number) {
    if (signal_number == SIGINT) {
        printf("SIGINT received. Shutting down server.\n");
        close(server_socket);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int server_port = (argc > 1) ? atoi(argv[1]) : 8080;  // Default port 8080
    signal(SIGINT, signal_handler);  // Register signal handler

    start_server(server_port);  // Start the server
    accept_connections(server_socket);  // Accept incoming connections

    return 0;
}

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

// Declare client_handler as external
extern void *client_handler(void *arg);

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

void initialize_server(SessionePartite *sessione) {
    memset(sessione, 0, sizeof(SessionePartite));
    pthread_mutex_init(&sessione->mutex_partite, NULL);
    for (int i = 0; i < MAX_GAMES; i++) {
        sessione->partite[i] = NULL;
    }
    sessione->num_partite = 0;
}

void cleanup_server(SessionePartite *sessione) {
    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i] != NULL) {
            pthread_mutex_destroy(&sessione->partite[i]->mutex);
            pthread_cond_destroy(&sessione->partite[i]->cond);
            free(sessione->partite[i]);
        }
    }
    pthread_mutex_destroy(&sessione->mutex_partite);
}

int main(int argc, char *argv[]) {
    int server_port = (argc > 1) ? atoi(argv[1]) : 8080;  // Default port 8080
    signal(SIGINT, signal_handler);  // Register signal handler

    SessionePartite sessione;
    initialize_server(&sessione);
    start_server(server_port);  // Start the server
    accept_connections(server_socket, &sessione);  // Accept incoming connections
    cleanup_server(&sessione);

    return 0;
}

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

// Accetta connessioni e crea un thread per ogni cliente
void accept_connections(int socket_fd, SessionePartite *sessione) {
    pthread_t client_thread;
    socklen_t client_address_len;
    int new_socket_fd;
    struct sockaddr_in client_address;

    while (1) {
        client_address_len = sizeof(client_address);
        new_socket_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket_fd == -1) {
            perror("Errore di accept");
            continue;
        }

        printf("Nuova connessione da %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Crea un thread per gestire la connessione del client
        if (pthread_create(&client_thread, NULL, client_handler, (void *)&new_socket_fd) != 0) {
            perror("Errore creazione thread per client");
            close(new_socket_fd);
            continue;
        }
    }
}

// Gestione della connessione con il client
void *client_handler(void *arg) {
    int socket_fd = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};

    // Stacca il thread dal chiamante per gestire la connessione in modo indipendente
    pthread_detach(pthread_self());

    while (read(socket_fd, buffer, BUFFER_SIZE) > 0) {
        // Processa il comando ricevuto dal client
        char command[20] = {0};
        sscanf(buffer, "%s", command);

        if (strncmp(command, "CREA_PARTITA", 12) == 0) {
            // Logica per la creazione della partita
            // ...
        } else if (strncmp(command, "UNISCI_PARTITA", 14) == 0) {
            // Logica per unire un giocatore a una partita
            // ...
        }

        // Resetta il buffer per la prossima lettura
        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("Connessione con il client chiusa.\n");
    close(socket_fd);
    pthread_exit(NULL);
    return NULL;
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "server_include/server_models.h"
#include "server_include/client_handler.h"

// Variabile globale per il socket del server
int server_fd;

// Gestore del segnale SIGINT per chiusura server
void handle_sigint(int sig) {
    printf("\nServer chiuso. Arrivederci!\n");
    close(server_fd);
    exit(0);
}

// Funzione principale del server
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <porta_server>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    SessionePartite sessione;

    // Inizializza la sessione delle partite
    inizializza_sessione(&sessione);

    // Configura il socket del server
    setup_server_socket(port, &server_fd);

    // Gestione del segnale per terminazione
    signal(SIGINT, handle_sigint);

    printf(GREEN_COLOR "Server pronto. In attesa di connessioni...\n" RESET_COLOR);

    // Accetta connessioni dai client
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("Errore durante l'accettazione della connessione");
            continue;
        }

        // Crea un nuovo thread per gestire il client
        pthread_t client_thread;
        int *arg = malloc(sizeof(int));
        *arg = client_fd;
        if (pthread_create(&client_thread, NULL, (void *(*)(void *))gestisci_client, arg) != 0) {
            perror("Errore creazione thread");
            close(client_fd);
            free(arg);
            continue;
        }

        pthread_detach(client_thread);
    }

    return 0;
}

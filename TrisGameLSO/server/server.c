#include "server_include/server_models.h"
#include "server_include/colors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    int server_port = 10000; // Porta predefinita
    GameManager game_manager;

    // Inizializza il GameManager
    init_game_manager(&game_manager);

    // Imposta il segnale di terminazione
    signal(SIGINT, signal_handler);

    if (argc > 1) {
        server_port = atoi(argv[1]);
    }

    printf("Avvio del server sulla porta %d...\n", server_port);

    // Avvio del server e creazione del socket
    start_server(server_port);

    // Accetta le connessioni dai client
    accept_connections(server_socket);

    close(server_socket);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "Server_include/structure.h"
#include "Server_include/game_logic.h"
#include "Server_include/lobby.h"
#include "Server_include/handle.h"


Lobby game_lobby;
pthread_mutex_t lobby_mutex = PTHREAD_MUTEX_INITIALIZER;


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    // Creazione del socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Abilita il riutilizzo dell'indirizzo
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Errore nel settaggio del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione del server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Errore nella bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        perror("Errore nella listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("=== Server Tris ===\n");
    printf("Server in ascolto sulla porta %d...\n", PORT);

    init_lobby(&game_lobby);
    
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket == -1) {
            perror("Errore nella accept");
            continue;
        }
        
        // Imposta il socket come non bloccante
        int flags = 1;
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Nuovo client connesso da %s\n", client_ip);
        
        pthread_t client_thread;
        int *client_sock = malloc(sizeof(int));
        *client_sock = client_socket;
        
        if (pthread_create(&client_thread, NULL, handle_client_connection, client_sock) != 0) {
            perror("Errore nella creazione del thread");
            free(client_sock);
            close(client_socket);
        }
        pthread_detach(client_thread);
    }

    close(server_socket);
    return 0;
}



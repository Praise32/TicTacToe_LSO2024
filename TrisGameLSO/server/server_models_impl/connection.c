#include "../server_include/server_models.h"
#include "../server_include/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

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
    printf(COLOR_GREEN "Socket bind completato con successo sulla porta %d\n" COLOR_RESET, port);

    if (listen(server_socket, 10) < 0) {
        perror("Errore nel listen");
        exit(1);
    }
    printf(COLOR_GREEN "Server in ascolto sulla porta %d\n" COLOR_RESET, port);
}


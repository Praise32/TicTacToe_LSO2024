#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../server_include/server_models.h"

void setup_server_socket(int port, int *server_fd) {
    struct sockaddr_in server_addr;

    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd == -1) {
        perror("Errore creazione socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(*server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore binding socket");
        close(*server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(*server_fd, MAX_CLIENTS) < 0) {
        perror("Errore listen");
        close(*server_fd);
        exit(EXIT_FAILURE);
    }

    printf(GREEN_COLOR "Server in ascolto sulla porta %d\n" RESET_COLOR, port);
}

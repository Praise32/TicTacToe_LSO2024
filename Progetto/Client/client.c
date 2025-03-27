#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "funzioni.h"



int main() {
    int socket_fd;
    struct sockaddr_in server_address;

    // Creazione del socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Connessione al server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Errore nella connessione al server");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Imposta il socket come non bloccante
    int flags = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));

    printf("Connessione al server riuscita!\n");
    sleep(1);
    
    while (1) {
        handle_menu(socket_fd);
        start_game(socket_fd);
    }

    close(socket_fd);
    return 0;
}

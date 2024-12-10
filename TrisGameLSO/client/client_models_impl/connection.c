#include "../client_include/client_models.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server_socket; // Socket globale

void connect_to_server() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore di connessione al server");
        exit(EXIT_FAILURE);
    }
    printf("Connesso al server!\n");
}

void send_message_to_server(const char *message) {
    if (send(server_socket, message, strlen(message), 0) < 0) {
        perror("Errore durante l'invio del messaggio");
    }
}

void receive_message_from_server(char *buffer, int buffer_size) {
    int bytes_received = recv(server_socket, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        perror("Errore durante la ricezione del messaggio");
    } else {
        buffer[bytes_received] = '\0';
    }
}

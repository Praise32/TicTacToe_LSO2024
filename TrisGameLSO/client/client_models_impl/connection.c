#include "../client_include/client_models.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int server_socket;

void connect_to_server() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Errore nella creazione del socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore di connessione al server");
        exit(1);
    }
    printf("Connesso al server!\n");
}

void send_message_to_server(const char *message) {
    send(server_socket, message, strlen(message), 0);
}

void receive_message_from_server(char *buffer, int buffer_size) {
    int bytes_received = recv(server_socket, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        perror("Errore durante la ricezione del messaggio");
    } else {
        buffer[bytes_received] = '\0'; // Termina la stringa
    }
}

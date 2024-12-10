#include "../server_include/server_models.h"
#include "../server_include/colors.h"

int server_socket; // Variabile globale definita qui

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

void accept_connections(int socket_fd) {
    while (1) {
        int client_socket = accept(socket_fd, NULL, NULL);
        if (client_socket < 0) {
            perror("Errore nell'accettazione del client");
            continue;
        }
        printf(COLOR_BLUE "Nuovo client connesso con socket %d\n" COLOR_RESET, client_socket);
    }
}

// Gestore del segnale SIGINT
void signal_handler(int signal_number) {
    if (signal_number == SIGINT) {
        printf("SIGINT received. Shutting down server.\n");
        exit(0);
    }
}

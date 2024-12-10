#include "server_include/server_models.h"
#include "server_include/colors.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *client_socket) {
    int socket = *(int *)client_socket;
    char buffer[1024];

    while (1) {
        int bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf(COLOR_RED "Il client si è disconnesso\n" COLOR_RESET);
            close(socket);
            break;
        }
        buffer[bytes_received] = '\0';

        pthread_mutex_lock(&session_mutex);
        printf(COLOR_YELLOW "Messaggio dal client: %s\n" COLOR_RESET, buffer);
        pthread_mutex_unlock(&session_mutex);
    }

    return NULL;
}


int main() {
    start_server(10000);
    printf(COLOR_GREEN "Server avviato con successo!\n" COLOR_RESET);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Errore nell'accettazione del client");
            continue;
        }

        printf(COLOR_BLUE "Nuovo client connesso\n" COLOR_RESET);
        
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void *)&client_socket);
    }

    return 0;
}

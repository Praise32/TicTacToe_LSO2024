#include "client_include/client_models.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#define SERVER_NAME_LEN_MAX 255
#define BUFFER_SIZE 1024

// Implementation of game-related functions
int create_game(GameState *state) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "CREATE_GAME");
    write(state->socket_fd, buffer, strlen(buffer));

    receive_game_update(state->socket_fd, buffer);
    if (strcmp(buffer, "GAME_CREATED") == 0) {
        printf("Partita creata con successo!\n");
        return 0;
    } else {
        printf("Errore nella creazione della partita: %s\n", buffer);
        return -1;
    }
}

int main(int argc, char *argv[]) {
    char server_name[SERVER_NAME_LEN_MAX + 1] = {0};
    int server_port, socket_fd, num_players=2;
    
    get_server_info(argc, argv, server_name, &server_port, &num_players);
    print_welcome_message();

    socket_fd = connect_with_retry(server_name, server_port);

    // Initialize game state
    GameState game_state;
    initialize_game_state(&game_state, socket_fd);

    // Send connection request
    const char *connect_message = "CONNECT";
    send(socket_fd, connect_message, strlen(connect_message), 0);

    // Handle connection acknowledgment
    char buffer[BUFFER_SIZE];
    int received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (received > 0) {
        buffer[received] = '\0';
        if (strcmp(buffer, "CONNECTED") == 0) {
            print_game_status("Connessione al server riuscita!");
        } else {
            print_game_status("Errore nella connessione al server");
            cleanup_resources(&game_state);
            exit(1);
        }
    } else {
        print_game_status("Errore nella ricezione dell'acknowledgment");
        cleanup_resources(&game_state);
        exit(1);
    }

    // Main game loop
    while (1) {
        clear_screen();
        print_main_menu();
        
        int choice;
        scanf("%d", &choice);

        switch(choice) {
            case 1: // Create game
                if (create_game(&game_state) == 0) {
                    play_game(&game_state);
                }
                break;
            case 2: { // Join game
                int game_id;
                printf("Inserisci l'ID della partita: ");
                scanf("%d", &game_id);
                if (join_game(&game_state, game_id) == 0) {
                    play_game(&game_state);
                }
                break;
            }
            case 3: // Exit
                cleanup_resources(&game_state);
                return 0;
            default:
                print_game_status("Scelta non valida!");
                sleep(1);
        }
    }

    cleanup_resources(&game_state);
    return 0;
}

// Rest of the implementation...

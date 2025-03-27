#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "../Server_include/structure.h"
#include "../Server_include/game_logic.h"
#include "../Server_include/lobby.h"
#include "../Server_include/handle.h"

extern Lobby game_lobby;
extern pthread_mutex_t lobby_mutex;



void send_to_player(int socket, const char *message) {
    send(socket, message, strlen(message), 0);
}

void display_server_status(int num_clients, int num_games) {
    CLEAR_SCREEN();
    printf("=== Server Tris ===\n");
    printf("Porta: %d\n", PORT);
    printf("Client connessi: %d\n", num_clients);
    printf("Partite attive: %d\n", num_games);
    printf("==============================\n\n");
}


// Funzione per gestire una singola partita
void *handle_game(void *arg) {
    Game *game = (Game *)arg;
    char buffer[BUFFER_SIZE];
    int current_player_index = 0;
    char board_status[BUFFER_SIZE];
    int rematch = 1;  // Flag per il ciclo di rematch

    while (rematch) {  // Loop esterno per gestire i rematch
        // Inizializza il tabellone
        initialize_board(game->board);
        game->current_player = 'X';
        current_player_index = 0;

        // Invia messaggio iniziale ai giocatori
        send_to_player(game->player_sockets[0], "\nSei il giocatore X\n");
        send_to_player(game->player_sockets[1], "\nSei il giocatore O\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            memset(board_status, 0, sizeof(board_status));

            // Prepara il tabellone
            sprintf(board_status, "\n=== Stato Partita ===\n");
            for (int i = 0; i < 3; i++) {
                strcat(board_status, "\n");
                for (int j = 0; j < 3; j++) {
                    char cell = game->board[i][j] == ' ' ? '.' : game->board[i][j];
                    sprintf(buffer, " %c ", cell);
                    strcat(board_status, buffer);
                    if (j < 2) strcat(board_status, "|");
                }
                if (i < 2) strcat(board_status, "\n---|---|---");
            }
            strcat(board_status, "\n\n");

            // Invia stato attuale a entrambi i giocatori
            for (int i = 0; i < 2; i++) {
                send_to_player(game->player_sockets[i], board_status);
                if (i == current_player_index) {
                    send_to_player(game->player_sockets[i], "\nÈ il tuo turno! Inserisci mossa (riga,colonna): ");
                } else {
                    send_to_player(game->player_sockets[i], "\nIn attesa della mossa dell'avversario... (Premi INVIO per aggiornare)\n");
                }
            }

            // Gestisci la mossa del giocatore corrente
            if (recv(game->player_sockets[current_player_index], buffer, sizeof(buffer), 0) <= 0) {
                send_to_player(game->player_sockets[1 - current_player_index], "\nL'avversario si è disconnesso. Vittoria!\n");
                break;
            }

            // Se è il giocatore non attivo che preme INVIO, continua il ciclo
            if (buffer[0] == '\n' && strlen(buffer) <= 2) {
                continue;
            }

            // Parse della mossa (formato: "row,col")
            int row, col;
            if (sscanf(buffer, "%d,%d", &row, &col) != 2 || !is_valid_move(game->board, row, col)) {
                send_to_player(game->player_sockets[current_player_index], "Mossa non valida! Riprova.\n");
                continue;
            }

            // Applica la mossa
            make_move(game->board, row, col, game->current_player);

            // Controlla lo stato del gioco
            char winner = check_winner(game->board);
            if (winner != ' ' || is_draw(game->board)) {
                // Invia stato finale
                send_to_player(game->player_sockets[0], board_status);
                send_to_player(game->player_sockets[1], board_status);
                
                if (winner != ' ') {
                    sprintf(buffer, "\nIl giocatore %c ha vinto!\n", winner);
                } else {
                    strcpy(buffer, "\nPareggio!\n");
                }
                send_to_player(game->player_sockets[0], buffer);
                send_to_player(game->player_sockets[1], buffer);
                break;
            }

            // Passa al prossimo giocatore
            game->current_player = (game->current_player == 'X') ? 'O' : 'X';
            current_player_index = 1 - current_player_index;
        }

        // Dopo la fine della partita, gestisci il rematch
        int choices[2] = {0, 0};
        int responses = 0;
        int winner_index = -1;

        // Determina il vincitore
        if (check_winner(game->board) != ' ') {
            winner_index = (game->current_player == 'X') ? 0 : 1;
        }

        // Ricevi le scelte dei giocatori
        for (int i = 0; i < 2; i++) {
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(game->player_sockets[i], buffer, BUFFER_SIZE - 1, 0) > 0) {
                choices[i] = atoi(buffer);
                responses++;
            }
        }

        if (responses == 2) {
            if (winner_index != -1) {
                // Solo il vincitore decide
                if (choices[winner_index] == 1) {
                    send_to_player(game->player_sockets[winner_index], "wait");
                    send_to_player(game->player_sockets[1 - winner_index], "exit");
                    pthread_mutex_lock(&lobby_mutex);
                    create_game_session(&game_lobby, "Rematch", game->player_sockets[winner_index]);
                    pthread_mutex_unlock(&lobby_mutex);
                } else {
                    send_to_player(game->player_sockets[0], "exit");
                    send_to_player(game->player_sockets[1], "exit");
                }
            } else {
                // In caso di pareggio, entrambi decidono
                if (choices[0] == 1 && choices[1] == 1) {
                    send_to_player(game->player_sockets[0], "continue");
                    send_to_player(game->player_sockets[1], "continue");
                    continue;  // Torna all'inizio del loop esterno
                } else {
                    send_to_player(game->player_sockets[0], "exit");
                    send_to_player(game->player_sockets[1], "exit");
                }
            }
            rematch = 0;  // Termina il loop di rematch
        }
    }

    free(game);
    pthread_exit(NULL);
}









void *handle_client_connection(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
    
    // Invia messaggio di benvenuto e attende conferma
    send_to_player(client_socket, "\nBenvenuto al gioco del Tris!\nPremi INVIO per continuare...");
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    
    while (1) {
        // Menu principale
        send_to_player(client_socket, "\n=== Menu Principale ===\n\n1. Crea partita\n2. Unisciti a partita\n\nScegli un'opzione: ");
        
        // Ricevi scelta
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE - 1, 0) <= 0) {
            printf("Client disconnesso.\n");
            close(client_socket);
            return NULL;
        }
        
        int choice = atoi(buffer);
        if (choice != 1 && choice != 2) {
            send_to_player(client_socket, "\nERRORE: Scelta non valida.\nPremi INVIO per continuare...");
            recv(client_socket, buffer, BUFFER_SIZE, 0); // Attende INVIO
            continue;
        }

        if (choice == 1) {
            send_to_player(client_socket, "\nInserisci il nome della partita: ");
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(client_socket, buffer, BUFFER_SIZE - 1, 0) <= 0) {
                close(client_socket);
                return NULL;
            }
            buffer[strcspn(buffer, "\n")] = 0;
            
            pthread_mutex_lock(&lobby_mutex);
            int game_id = create_game_session(&game_lobby, buffer, client_socket);
            
            if (game_id >= 0) {
                // Usa snprintf invece di sprintf per evitare overflow
                snprintf(msg, BUFFER_SIZE - 1, "\nPartita '%.*s' creata! ID: %d\nIn attesa di un avversario...\n", 
                        MAX_GAME_NAME - 1, buffer, game_id);
                send_to_player(client_socket, msg);
                
                while (1) {
                    if (!game_lobby.games[game_id].is_active) {
                        pthread_mutex_unlock(&lobby_mutex);
                        return NULL;
                    }
                    pthread_mutex_unlock(&lobby_mutex);
                    sleep(1);
                    pthread_mutex_lock(&lobby_mutex);
                }
            } else {
                pthread_mutex_unlock(&lobby_mutex);
                send_to_player(client_socket, "\nERRORE: Impossibile creare la partita.\n");
            }
        } else if (choice == 2) {
            while (1) {  // Main game selection loop
                pthread_mutex_lock(&lobby_mutex);
                if (game_lobby.num_games == 0) {
                    pthread_mutex_unlock(&lobby_mutex);
                    send_to_player(client_socket, "\nNessuna partita disponibile.\nPremi INVIO per continuare...");
                    recv(client_socket, buffer, BUFFER_SIZE, 0);
                    break;  // Return to main menu
                }
                
                char games_list[BUFFER_SIZE] = "\nPartite disponibili:\n\n";
                list_games(&game_lobby, games_list + strlen(games_list));
                strcat(games_list, "\nScegli il numero della partita (0-9) o 'q' per tornare al menu: ");
                send_to_player(client_socket, games_list);
                pthread_mutex_unlock(&lobby_mutex);
                
                // Read input
                memset(buffer, 0, BUFFER_SIZE);
                if (recv(client_socket, buffer, BUFFER_SIZE - 1, 0) <= 0) {
                    close(client_socket);
                    return NULL;
                }
                
                // Remove newline
                buffer[strcspn(buffer, "\n")] = 0;
                
                // Check for quit command
                if (buffer[0] == 'q' || buffer[0] == 'Q') {
                    break;  // Return to main menu
                }
                
                // Controlla se l'input è un numero valido
                char *endptr;
                long game_id = strtol(buffer, &endptr, 10);
                
                if (*endptr != '\0' || game_id < 0 || game_id >= MAX_GAMES) {
                    send_to_player(client_socket, "\nERRORE: Scelta non valida.\nPremi INVIO per continuare...");
                    recv(client_socket, buffer, BUFFER_SIZE, 0); // Attende INVIO
                    continue;
                }
                
                pthread_mutex_lock(&lobby_mutex);
                
                // Check if game exists and is active
                if (!game_lobby.games[game_id].is_active) {
                    pthread_mutex_unlock(&lobby_mutex);
                    send_to_player(client_socket, "\nLa partita selezionata non esiste più.\nPremi INVIO per continuare...");
                    recv(client_socket, buffer, BUFFER_SIZE, 0); // Attende INVIO
                    continue;
                }
                
                // Check if game is full
                if (game_lobby.games[game_id].guest_socket != -1) {
                    pthread_mutex_unlock(&lobby_mutex);
                    send_to_player(client_socket, "\nLa partita è già piena.\nPremi INVIO per continuare...");
                    recv(client_socket, buffer, BUFFER_SIZE, 0); // Attende INVIO
                    continue;
                }
                
                // Join game if everything is valid
                if (join_game_session(&game_lobby, game_id, client_socket) == 0) {
                    // ... existing game setup code ...
                    Game *game = malloc(sizeof(Game));
                    game->player_sockets[0] = game_lobby.games[game_id].host_socket;
                    game->player_sockets[1] = client_socket;
                    
                    send_to_player(game->player_sockets[0], "\nAvversario trovato! La partita sta per iniziare...\n");
                    send_to_player(game->player_sockets[1], "\nConnessione riuscita! La partita sta per iniziare...\n");
                    
                    pthread_t game_thread;
                    pthread_create(&game_thread, NULL, handle_game, game);
                    pthread_detach(game_thread);
                    
                    remove_game_session(&game_lobby, game_id);
                    pthread_mutex_unlock(&lobby_mutex);
                    return NULL;
                }
                
                pthread_mutex_unlock(&lobby_mutex);
                send_to_player(client_socket, "\nErrore nell'unirsi alla partita. Riprova.\n");
            }
            continue;  // Return to main menu
        }
    }
    return NULL;
}
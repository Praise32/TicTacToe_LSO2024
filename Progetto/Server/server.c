#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "Server_include/game_logic.h"
#include "Server_include/lobby.h"

#define PORT 12345
#define MAX_PLAYERS 2
#define BUFFER_SIZE 1024
#define CLEAR_SCREEN() printf("\033[H\033[J")

typedef struct {
    int player_sockets[MAX_PLAYERS];
    char board[3][3];
    char current_player;
} Game;

Lobby game_lobby;
pthread_mutex_t lobby_mutex = PTHREAD_MUTEX_INITIALIZER;

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

void *handle_game(void *arg) {
    Game *game = (Game *)arg;
    char buffer[BUFFER_SIZE];
    int current_player_index = 0;

    // Inizializza la board e imposta il primo giocatore
    initialize_board(game->board);
    game->current_player = 'X';

    // Notifica i giocatori
    send_to_player(game->player_sockets[0], "\nSei il giocatore X\n");
    send_to_player(game->player_sockets[1], "\nSei il giocatore O\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // Segnala il turno del giocatore
        send_to_player(game->player_sockets[current_player_index], "\nÈ il tuo turno! Inserisci mossa (riga,colonna) o '1' per vedere il tabellone aggiornato: ");

        // Ricezione input
        while (1) {
            memset(buffer, 0, sizeof(buffer));  // Pulizia buffer
            int bytes_received = recv(game->player_sockets[current_player_index], buffer, sizeof(buffer), 0);

            // Controlla se il giocatore si è disconnesso
            if (bytes_received <= 0) {
                send_to_player(game->player_sockets[1 - current_player_index], "\nL'avversario si è disconnesso. Vittoria!\n");
                free(game);
                pthread_exit(NULL);
            }

            // Rimuove eventuali caratteri di nuova linea
            buffer[strcspn(buffer, "\r\n")] = 0;

            // Se l'input è vuoto dopo la pulizia, chiede nuovamente
            if (strlen(buffer) == 0) {
                send_to_player(game->player_sockets[current_player_index], "Input vuoto! Inserisci una mossa valida (riga,colonna) o '1' per vedere il tabellone aggiornato:\n");
                continue;
            }

            // Se l'input è "1", invia il tabellone aggiornato a entrambi i giocatori
            if (strcmp(buffer, "1") == 0) {
                // Invio del tabellone aggiornato a entrambi i giocatori
                char board_status[BUFFER_SIZE] = "\n=== Stato Partita ===\n";
                for (int i = 0; i < 3; i++) {
                    strcat(board_status, "\n");
                    for (int j = 0; j < 3; j++) {
                        char cell = (game->board[i][j] == ' ') ? '.' : game->board[i][j];
                        char cell_buffer[10];
                        sprintf(cell_buffer, " %c ", cell);
                        strcat(board_status, cell_buffer);
                        if (j < 2) strcat(board_status, "|");
                    }
                    if (i < 2) strcat(board_status, "\n---|---|---");
                }
                strcat(board_status, "\n\n");

                send_to_player(game->player_sockets[current_player_index], board_status);
                // Dopo aver inviato il tabellone, richiedi di nuovo la mossa
                send_to_player(game->player_sockets[current_player_index], "\nInserisci mossa (riga,colonna): ");
                continue;  // Continua a chiedere la mossa, senza entrare nel ciclo di validazione della mossa
            }

            // Se l'input non è "1", processa la mossa
            break;
        }

        // Parsing della mossa
        int row, col;
        if (sscanf(buffer, "%d,%d", &row, &col) != 2) {
            send_to_player(game->player_sockets[current_player_index], "Formato non valido! Usa 'riga,colonna'.\n");
            continue;
        }

        // Verifica se la mossa è valida
        if (!is_valid_move(game->board, row, col)) {
            send_to_player(game->player_sockets[current_player_index], "Mossa non valida! Riprova.\n");
            continue;
        }

        // Registra la mossa
        make_move(game->board, row, col, game->current_player);

        // Controlla se c'è un vincitore
        char winner = check_winner(game->board);
        if (winner != ' ' || is_draw(game->board)) {
            // Invio del tabellone aggiornato a entrambi i giocatori
            char board_status[BUFFER_SIZE] = "\n=== Stato Partita ===\n";
            for (int i = 0; i < 3; i++) {
                strcat(board_status, "\n");
                for (int j = 0; j < 3; j++) {
                    char cell = (game->board[i][j] == ' ') ? '.' : game->board[i][j];
                    char cell_buffer[10];
                    sprintf(cell_buffer, " %c ", cell);
                    strcat(board_status, cell_buffer);
                    if (j < 2) strcat(board_status, "|");
                }
                if (i < 2) strcat(board_status, "\n---|---|---");
            }
            strcat(board_status, "\n\n");

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

        // Passa il turno
        current_player_index = 1 - current_player_index;
        game->current_player = (game->current_player == 'X') ? 'O' : 'X';
    }

    // Libera la memoria e termina il thread
    free(game);
    pthread_exit(NULL);
}














void *handle_client_connection(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    
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
                char msg[BUFFER_SIZE];
                sprintf(msg, "\nPartita '%s' creata! ID: %d\nIn attesa di un avversario...\n", buffer, game_id);
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
            pthread_mutex_lock(&lobby_mutex);
            if (game_lobby.num_games == 0) {
                pthread_mutex_unlock(&lobby_mutex);
                send_to_player(client_socket, "\nNessuna partita disponibile.\nPremi INVIO per continuare...");
                recv(client_socket, buffer, BUFFER_SIZE, 0); // Attende INVIO
                continue;
            }
            char games_list[BUFFER_SIZE] = "\nPartite disponibili:\n\n";
            list_games(&game_lobby, games_list + strlen(games_list));
            
            strcat(games_list, "\nScegli il numero della partita: ");
            send_to_player(client_socket, games_list);
            pthread_mutex_unlock(&lobby_mutex);
            
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(client_socket, buffer, BUFFER_SIZE - 1, 0) <= 0) {
                close(client_socket);
                return NULL;
            }
            
            int game_id = atoi(buffer);
            pthread_mutex_lock(&lobby_mutex);
            if (game_id >= 0 && game_id < MAX_GAMES && game_lobby.games[game_id].is_active) {
                if (join_game_session(&game_lobby, game_id, client_socket) == 0) {
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
            }
            pthread_mutex_unlock(&lobby_mutex);
            send_to_player(client_socket, "\nERRORE: Partita non valida o piena.\n");
        }
    }
    return NULL;
}

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



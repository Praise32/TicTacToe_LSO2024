#ifndef CLIENT_MODELS_H
#define CLIENT_MODELS_H

// Struttura per rappresentare il tabellone
typedef struct {
    char board[3][3];     // Tabellone 3x3
    int current_turn;     // Giocatore corrente (1 o 2)
} GameState;

// Costanti di configurazione
#define SERVER_PORT 10000
#define SERVER_IP "127.0.0.1"

extern int server_socket;

// Funzioni generali (dichiarate in connection.c)
void connect_to_server();
void send_message_to_server(const char *message);
void receive_message_from_server(char *buffer, int buffer_size);

// Funzioni specifiche del gioco
void receive_game_board(char board[3][3]);

#endif

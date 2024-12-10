#ifndef SERVER_MODELS_H
#define SERVER_MODELS_H

extern int server_socket;  // Dichiarazione esterna, senza definizione

// Struttura per rappresentare un giocatore
typedef struct {
    int socket;       // Socket del giocatore
    int in_game;      // Indica se il giocatore è in partita
} Player;

// Stato di una singola partita
typedef struct {
    int id;                // ID della partita
    int player1_socket;    // Socket del giocatore 1
    int player2_socket;    // Socket del giocatore 2
    char board[3][3];      // Tabellone 3x3
    char state[10];        // Stato (in_corso, attesa, terminata)
    char result[10];       // Risultato (vittoria, sconfitta, pareggio)
    int current_turn;      // Turno corrente (1 o 2)
} GameSession;

// Funzioni dichiarate in session.c
void init_game(GameSession *game, int player1_socket);
int make_move(GameSession *game, int player, int row, int col);
int check_victory(GameSession *game);
int check_draw(GameSession *game);

// Funzioni per sessioni
void init_session(GameSession *session, int player_socket);
void display_sessions();

void start_server(int port);

#endif

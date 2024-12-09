#ifndef SERVER_MODELS_H
#define SERVER_MODELS_H

// Stato di una singola partita
typedef struct {
    int id;               // ID della partita
    int player1_socket;   // Socket del giocatore 1
    int player2_socket;   // Socket del giocatore 2
    char board[3][3];     // Tabellone 3x3
    char state[10];       // Stato (in_corso, attesa, terminata)
    char result[10];      // Risultato (vittoria, sconfitta, pareggio)
    int current_turn;     // Turno corrente (1 o 2)
} GameSession;

// Funzioni dichiarate in session.c
void create_game_session(int player_socket);
void join_game_session(int player_socket, int game_id);
void update_game_board(int game_id, int player, int row, int col);
void check_game_status(int game_id);

#endif

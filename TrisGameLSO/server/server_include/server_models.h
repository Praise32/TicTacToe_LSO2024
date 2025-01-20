#ifndef SERVER_MODELS_H
#define SERVER_MODELS_H

#include <pthread.h>
#include <netinet/in.h>

// Definizione costanti
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100
#define MAX_GAMES 50

// Definizione colori ANSI per log
#define RESET_COLOR    "\x1b[0m"
#define GREEN_COLOR    "\x1b[32m"
#define BLUE_COLOR     "\x1b[34m"
#define CYAN_COLOR     "\x1b[36m"

// Struttura del Giocatore
typedef struct {
    int id;                     // ID univoco del giocatore
    int socket_fd;              // Socket associato al giocatore
    char simbolo;               // 'X' o 'O'
    int partita_attiva;         // ID della partita in corso (-1 se nessuna)
} Giocatore;

// Struttura della Partita
typedef struct {
    int id;                     // ID univoco della partita
    Giocatore *giocatore_1;     // Primo giocatore
    Giocatore *giocatore_2;     // Secondo giocatore
    char board[3][3];           // Tabellone 3x3
    int stato;                  // Stato della partita (0: in attesa, 1: in corso, 2: terminata)
    int turno;                  // 0: turno giocatore 1, 1: turno giocatore 2
    pthread_mutex_t mutex;      // Mutex per sincronizzazione
    pthread_cond_t cond;        // Condizione per sincronizzazione
} Partita;

// Gestione sessione di partite
typedef struct {
    Partita *partite[MAX_GAMES];
    int num_partite;
    pthread_mutex_t mutex_partite;
} SessionePartite;

// Dichiarazioni delle funzioni di gestione delle connessioni
void setup_server_socket(int port, int *server_fd);

// Dichiarazioni delle funzioni di gestione dei giocatori
int crea_giocatore(int socket_fd, char simbolo);

// Dichiarazioni delle funzioni di gestione delle partite
int crea_partita(SessionePartite *sessione);
int verifica_vittoria(Partita *partita);
int verifica_pareggio(Partita *partita);

// Dichiarazioni delle funzioni di gestione della sessione
void inizializza_sessione(SessionePartite *sessione);
int cerca_partita_disponibile(SessionePartite *sessione);

#endif // SERVER_MODELS_H

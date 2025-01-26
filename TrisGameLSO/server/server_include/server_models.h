#include <pthread.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdatomic.h>

// Definizione colori per il terminale
#define RESET_COLOR    "\x1b[0m"
#define GREEN_COLOR    "\x1b[32m"
#define BLUE_COLOR     "\x1b[34m"
#define CYAN_COLOR     "\x1b[36m"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100
#define MAX_GAMES 50

// Struttura del Giocatore
typedef struct {
    int id;
    int socket_fd;
    char simbolo;  // 'X' o 'O'
    int partita_attiva; // ID della partita a cui è associato
} Giocatore;

// Struttura per la Partita
typedef struct {
    int id;                    // Identificatore unico della partita
    Giocatore *giocatore_1;    // Giocatore 1
    Giocatore *giocatore_2;    // Giocatore 2
    char board[3][3];          // Tavolo di gioco 3x3
    int stato;                 // 0: in attesa, 1: in corso, 2: terminata
    int turno;                 // 0: Giocatore 1, 1: Giocatore 2
    pthread_mutex_t mutex;     // Protezione dei dati della partita
    pthread_cond_t cond;       // Condizione per la sincronizzazione dei giocatori
} Partita;

// Struttura per la gestione delle partite attive
typedef struct {
    Partita *partite[MAX_GAMES];
    int num_partite;
    pthread_mutex_t mutex_partite;
} SessionePartite;

// Struttura per la gestione dei dati del client
typedef struct {
    int client_socket;
    SessionePartite *sessione;
} ClientData;

// Funzioni per la gestione dei giocatori
Giocatore* crea_giocatore(int socket_fd, char simbolo);
int aggiungi_giocatore_a_partita(Giocatore *giocatore, Partita *partita);
void rimuovi_giocatore_da_partita(Giocatore *giocatore);

// Funzioni per la gestione delle partite
int crea_partita(int socket_fd);
int unisci_a_partita(int socket_fd, int partita_id);
int invia_mossa(int socket_fd, int partita_id, int riga, int colonna);
int aggiorna_partita(int socket_fd, int partita_id);
void termina_partita(int partita_id);

// Funzioni di gestione della sessione delle partite
int cerca_partita_disponibile(SessionePartite *sessione);
void inizializza_sessione(SessionePartite *sessione);
void gestisci_partite(SessionePartite *sessione);
void print_welcome_message(int num_players, int server_port);

// Funzioni di gestione della connessione client
void setup_server_socket(int server_port, int *socket_fd);
void accept_connections(int socket_fd, SessionePartite *sessione);
void *client_handler(void *arg);

// Funzioni di utilità per la sincronizzazione
void send_message(int socket_fd, const char *message);
void receive_message(int socket_fd, char *buffer);

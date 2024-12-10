#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../server_include/server_models.h"

// Funzione per creare una nuova partita
int crea_partita(int socket_fd) {
    static int partita_counter = 1;  // Contatore per le partite
    Partita *partita = malloc(sizeof(Partita));
    if (partita == NULL) {
        perror("Errore nell'allocazione della memoria per la partita");
        return -1;
    }

    // Assegna un ID unico alla partita
    partita->id = partita_counter++;
    partita->giocatore_1 = NULL;
    partita->giocatore_2 = NULL;
    partita->stato = 0;  // Stato "in attesa"
    partita->turno = 0;  // Inizia con Giocatore 1
    pthread_mutex_init(&partita->mutex, NULL);
    pthread_cond_init(&partita->cond, NULL);
    
    // Inizializza il tabellone a vuoto
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            partita->board[i][j] = ' ';
        }
    }

    printf("Partita creata con ID: %d\n", partita->id);
    // Salva la partita nella sessione delle partite attive
    SessionePartite *sessione = // Trova la sessione delle partite attive
    sessione->partite[sessione->num_partite++] = partita;

    return partita->id;
}

// Funzione per unire un giocatore a una partita esistente
int unisci_a_partita(int socket_fd, int partita_id) {
    SessionePartite *sessione = // Trova la sessione delle partite attive
    Partita *partita = sessione->partite[partita_id];

    if (partita == NULL) {
        printf("Partita con ID %d non trovata.\n", partita_id);
        return -1;
    }

    pthread_mutex_lock(&partita->mutex);

    if (partita->giocatore_1 == NULL) {
        printf("Aggiungo il giocatore come Giocatore 1 alla partita %d\n", partita_id);
        partita->giocatore_1 = crea_giocatore(socket_fd, 'X');  // Giocatore 1
    } else if (partita->giocatore_2 == NULL) {
        printf("Aggiungo il giocatore come Giocatore 2 alla partita %d\n", partita_id);
        partita->giocatore_2 = crea_giocatore(socket_fd, 'O');  // Giocatore 2
        partita->stato = 1;  // La partita inizia
        pthread_cond_signal(&partita->cond);  // Avvia la partita
    } else {
        pthread_mutex_unlock(&partita->mutex);
        return -1;  // La partita è piena
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

// Funzione per inviare una mossa al server
int invia_mossa(int socket_fd, int partita_id, int riga, int colonna) {
    SessionePartite *sessione = // Trova la sessione delle partite attive
    Partita *partita = sessione->partite[partita_id];

    if (partita == NULL || riga < 0 || riga > 2 || colonna < 0 || colonna > 2) {
        return -1;  // Partita non trovata o mossa invalida
    }

    pthread_mutex_lock(&partita->mutex);

    // Controlla se la cella è vuota
    if (partita->board[riga][colonna] != ' ') {
        pthread_mutex_unlock(&partita->mutex);
        return -1;  // La cella è già occupata
    }

    // Aggiorna la tabella con la mossa del giocatore
    if (partita->turno == 0 && partita->giocatore_1->socket_fd == socket_fd) {
        partita->board[riga][colonna] = 'X';  // Giocatore 1
        partita->turno = 1;
    } else if (partita->turno == 1 && partita->giocatore_2->socket_fd == socket_fd) {
        partita->board[riga][colonna] = 'O';  // Giocatore 2
        partita->turno = 0;
    } else {
        pthread_mutex_unlock(&partita->mutex);
        return -1;  // Non è il turno del giocatore
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

// Funzione per aggiornare lo stato della partita
int aggiorna_partita(int socket_fd, int partita_id) {
    SessionePartite *sessione = // Trova la sessione delle partite attive
    Partita *partita = sessione->partite[partita_id];

    if (partita == NULL) {
        return -1;  // Partita non trovata
    }

    pthread_mutex_lock(&partita->mutex);

    // Controlla se la partita è finita (vittoria, pareggio, ecc.)
    if (verifica_vittoria(partita)) {
        partita->stato = 2;  // Vittoria
        printf("La partita %d è terminata con una vittoria.\n", partita_id);
    } else if (verifica_pareggio(partita)) {
        partita->stato = 2;  // Pareggio
        printf("La partita %d è terminata con pareggio.\n", partita_id);
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

// Funzione per terminare una partita
void termina_partita(int partita_id) {
    SessionePartite *sessione = // Trova la sessione delle partite attive
    Partita *partita = sessione->partite[partita_id];

    if (partita != NULL) {
        pthread_mutex_lock(&partita->mutex);

        // Esegui le operazioni di fine partita
        printf("La partita %d è stata terminata.\n", partita_id);

        // Libera le risorse associate alla partita
        free(partita);

        pthread_mutex_unlock(&partita->mutex);
    } else {
        printf("Partita con ID %d non trovata.\n", partita_id);
    }
}

// Funzione per simulare una partita
void simula_partita(int socket_fd_giocatore_1, int socket_fd_giocatore_2) {
    // Messaggio di benvenuto prima di iniziare la partita
    int num_players = 2;  // Poiché stiamo simulando una partita a 2 giocatori
    int server_port = 8080;  // Imposta una porta fittizia

    int partita_id = crea_partita(socket_fd_giocatore_1);  // Creiamo una nuova partita
    if (partita_id == -1) {
        printf("Errore nella creazione della partita.\n");
        return;
    }

    // Uniamo i giocatori alla partita
    if (unisci_a_partita(socket_fd_giocatore_1, partita_id) != 0) {
        printf("Errore nell'unire Giocatore 1 alla partita.\n");
        return;
    }

    if (unisci_a_partita(socket_fd_giocatore_2, partita_id) != 0) {
        printf("Errore nell'unire Giocatore 2 alla partita.\n");
        return;
    }

    // Simuliamo un ciclo di mosse (alternando tra Giocatore 1 e Giocatore 2)
    int mosse[9][2] = {
        {0, 0}, {0, 1}, {0, 2},
        {1, 0}, {1, 1}, {1, 2},
        {2, 0}, {2, 1}, {2, 2}
    };

    for (int i = 0; i < 9; i++) {
        int riga = mosse[i][0];
        int colonna = mosse[i][1];
        int giocatore = i % 2;  // Alterna tra 0 (Giocatore 1) e 1 (Giocatore 2)

        int socket_fd = (giocatore == 0) ? socket_fd_giocatore_1 : socket_fd_giocatore_2;

        if (invia_mossa(socket_fd, partita_id, riga, colonna) != 0) {
            printf("Errore nell'inviare la mossa.\n");
            break;
        }

        printf("Giocatore %d ha fatto la mossa: (%d, %d)\n", giocatore + 1, riga, colonna);
        aggiorna_partita(socket_fd, partita_id);  // Aggiorniamo lo stato della partita

        // Verifica se la partita è finita (vittoria o pareggio)
        SessionePartite *sessione = // Trova la sessione delle partite attive
        Partita *partita = sessione->partite[partita_id];

        if (partita->stato == 2) {  // Stato 2 indica che la partita è terminata
            if (verifica_vittoria(partita)) {
                printf("Giocatore %d ha vinto!\n", (giocatore == 0) ? 1 : 2);
            } else {
                printf("La partita è finita in pareggio.\n");
            }
            break;
        }

        // Introduci una piccola pausa per simulare il tempo tra le mosse
        sleep(1);
    }

    termina_partita(partita_id);  // Termina la partita
}

// Funzione per stampare il messaggio di benvenuto per il gioco del Tris
void print_welcome_message(int num_players, int server_port) {
    // Stampa un'intestazione colorata e formattata
    printf(CYAN_COLOR);
    printf("========================================\n");
    printf("        WELCOME TO THE TIC-TAC-TOE     \n");
    printf("========================================\n");
    printf(RESET_COLOR);

    // Dettagli sul numero di giocatori e la porta del server
    printf(BLUE_COLOR);
    printf("🎮 Number of players in the game: %d\n", num_players);
    printf("💻 The server is listening on port: %d\n", server_port);
    printf(RESET_COLOR);

    // Messaggio finale
    printf(GREEN_COLOR);
    printf("========================================\n");
    printf("         Good luck and have fun!        \n");
    printf("========================================\n\n");
    printf(RESET_COLOR);
}


// Funzione main per eseguire la simulazione
int main() {
    int socket_fd_giocatore_1 = 1;  // ID socket fittizio per Giocatore 1
    int socket_fd_giocatore_2 = 2;  // ID socket fittizio per Giocatore 2

    // Simula la partita tra i due giocatori
    simula_partita(socket_fd_giocatore_1, socket_fd_giocatore_2);

    return 0;
}

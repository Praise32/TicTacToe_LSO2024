#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../server_include/server_models.h"

// Funzione per creare una nuova partita
int crea_partita(SessionePartite *sessione) {
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

    pthread_mutex_lock(&sessione->mutex_partite);
    if (sessione->num_partite < MAX_GAMES) {
        sessione->partite[sessione->num_partite++] = partita;
        printf("Partita creata con ID: %d\n", partita->id);
    } else {
        printf("Limite massimo di partite raggiunto.\n");
        free(partita);
        pthread_mutex_unlock(&sessione->mutex_partite);
        return -1;
    }
    pthread_mutex_unlock(&sessione->mutex_partite);

    return partita->id;
}

// Funzione per unire un giocatore a una partita esistente
int unisci_a_partita(SessionePartite *sessione, int socket_fd, int partita_id) {
    pthread_mutex_lock(&sessione->mutex_partite);
    Partita *partita = NULL;

    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i]->id == partita_id) {
            partita = sessione->partite[i];
            break;
        }
    }
    pthread_mutex_unlock(&sessione->mutex_partite);

    if (partita == NULL) {
        printf("Partita con ID %d non trovata.\n", partita_id);
        return -1;
    }

    pthread_mutex_lock(&partita->mutex);

    if (partita->giocatore_1 == NULL) {
        partita->giocatore_1 = malloc(sizeof(Giocatore));
        partita->giocatore_1->socket_fd = socket_fd;
        partita->giocatore_1->simbolo = 'X';
        partita->giocatore_1->partita_attiva = partita_id;
        printf("Giocatore 1 aggiunto alla partita %d.\n", partita_id);
    } else if (partita->giocatore_2 == NULL) {
        partita->giocatore_2 = malloc(sizeof(Giocatore));
        partita->giocatore_2->socket_fd = socket_fd;
        partita->giocatore_2->simbolo = 'O';
        partita->giocatore_2->partita_attiva = partita_id;
        partita->stato = 1;  // La partita è in corso
        printf("Giocatore 2 aggiunto alla partita %d.\n", partita_id);
    } else {
        pthread_mutex_unlock(&partita->mutex);
        printf("Partita %d è piena.\n", partita_id);
        return -1;
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

// Funzione per inviare una mossa
int invia_mossa(SessionePartite *sessione, int partita_id, int socket_fd, int riga, int colonna) {
    pthread_mutex_lock(&sessione->mutex_partite);
    Partita *partita = NULL;

    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i]->id == partita_id) {
            partita = sessione->partite[i];
            break;
        }
    }
    pthread_mutex_unlock(&sessione->mutex_partite);

    if (partita == NULL || riga < 0 || riga > 2 || colonna < 0 || colonna > 2) {
        printf("Partita o mossa invalida.\n");
        return -1;
    }

    pthread_mutex_lock(&partita->mutex);

    if (partita->board[riga][colonna] != ' ') {
        pthread_mutex_unlock(&partita->mutex);
        printf("Cella occupata.\n");
        return -1;
    }

    if (partita->turno == 0 && partita->giocatore_1->socket_fd == socket_fd) {
        partita->board[riga][colonna] = 'X';
        partita->turno = 1;
    } else if (partita->turno == 1 && partita->giocatore_2->socket_fd == socket_fd) {
        partita->board[riga][colonna] = 'O';
        partita->turno = 0;
    } else {
        pthread_mutex_unlock(&partita->mutex);
        printf("Non è il turno del giocatore.\n");
        return -1;
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

// Funzione per verificare la vittoria
int verifica_vittoria(Partita *partita) {
    char (*board)[3] = partita->board;

    for (int i = 0; i < 3; i++) {
        if ((board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) ||
            (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])) {
            return 1;
        }
    }

    if ((board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
        (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])) {
        return 1;
    }

    return 0;
}

// Funzione per verificare il pareggio
int verifica_pareggio(Partita *partita) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (partita->board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}

// Funzione per aggiornare lo stato della partita
int aggiorna_partita(SessionePartite *sessione, int partita_id) {
    pthread_mutex_lock(&sessione->mutex_partite);
    Partita *partita = NULL;

    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i]->id == partita_id) {
            partita = sessione->partite[i];
            break;
        }
    }
    pthread_mutex_unlock(&sessione->mutex_partite);

    if (partita == NULL) {
        return -1;
    }

    pthread_mutex_lock(&partita->mutex);

    if (verifica_vittoria(partita)) {
        partita->stato = 2;
        printf("Partita %d vinta!\n", partita_id);
    } else if (verifica_pareggio(partita)) {
        partita->stato = 2;
        printf("Partita %d terminata in pareggio.\n", partita_id);
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;
}

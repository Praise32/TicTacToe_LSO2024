#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../server_include/server_models.h"

// Dichiarazione della funzione get_sessione_partite
SessionePartite* get_sessione_partite();

// Funzione per creare un nuovo giocatore
Giocatore* crea_giocatore(int socket_fd, char simbolo) {
    static int id_counter = 1;  // Contatore per gli ID dei giocatori
    Giocatore *giocatore = malloc(sizeof(Giocatore));
    if (giocatore == NULL) {
        perror("Errore nell'allocazione della memoria per il giocatore");
        return NULL;
    }

    // Assegna un ID unico al giocatore
    giocatore->id = id_counter++;
    giocatore->socket_fd = socket_fd;
    giocatore->simbolo = simbolo;
    giocatore->partita_attiva = -1;  // Nessuna partita attiva inizialmente

    return giocatore;
}

// Funzione per aggiungere un giocatore a una partita
int aggiungi_giocatore_a_partita(Giocatore *giocatore, Partita *partita) {
    pthread_mutex_lock(&partita->mutex);

    if (partita->giocatore_1 == NULL) {
        partita->giocatore_1 = giocatore;
        giocatore->partita_attiva = partita->id;
        printf("Giocatore %d (simbolo %c) è stato aggiunto come Giocatore 1 alla partita %d\n",
               giocatore->id, giocatore->simbolo, partita->id);
    } else if (partita->giocatore_2 == NULL) {
        partita->giocatore_2 = giocatore;
        giocatore->partita_attiva = partita->id;
        printf("Giocatore %d (simbolo %c) è stato aggiunto come Giocatore 2 alla partita %d\n",
               giocatore->id, giocatore->simbolo, partita->id);
    } else {
        pthread_mutex_unlock(&partita->mutex);
        return -1;  // La partita è piena
    }

    pthread_mutex_unlock(&partita->mutex);
    return 0;  // Successo
}

// Funzione per rimuovere un giocatore da una partita
void rimuovi_giocatore_da_partita(Giocatore *giocatore) {
    SessionePartite *sessione = get_sessione_partite(); // Trova la sessione delle partite attive
    Partita *partita = NULL;

    // Cerca la partita a cui il giocatore è associato
    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i]->id == giocatore->partita_attiva) {
            partita = sessione->partite[i];
            break;
        }
    }

    if (partita != NULL) {
        pthread_mutex_lock(&partita->mutex);

        if (partita->giocatore_1 == giocatore) {
            partita->giocatore_1 = NULL;
        } else if (partita->giocatore_2 == giocatore) {
            partita->giocatore_2 = NULL;
        }

        giocatore->partita_attiva = -1;  // Resetta la partita attiva
        pthread_mutex_unlock(&partita->mutex);

        printf("Giocatore %d rimosso dalla partita %d\n", giocatore->id, partita->id);
    } else {
        printf("Giocatore %d non è associato a nessuna partita.\n", giocatore->id);
    }

    free(giocatore);
}
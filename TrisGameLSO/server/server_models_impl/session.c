#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../server_include/server_models.h"

void inizializza_sessione(SessionePartite *sessione) {
    sessione->num_partite = 0;
    pthread_mutex_init(&sessione->mutex_partite, NULL);
    printf("Sessione inizializzata.\n");
}

int cerca_partita_disponibile(SessionePartite *sessione) {
    pthread_mutex_lock(&sessione->mutex_partite);

    for (int i = 0; i < sessione->num_partite; i++) {
        if (sessione->partite[i]->stato == 0) { // In attesa
            pthread_mutex_unlock(&sessione->mutex_partite);
            return sessione->partite[i]->id;
        }
    }

    pthread_mutex_unlock(&sessione->mutex_partite);
    return -1; // Nessuna partita disponibile
}

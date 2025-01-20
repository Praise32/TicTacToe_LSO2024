#include <stdio.h>
#include <stdlib.h>
#include "../server_include/server_models.h"

int crea_giocatore(int socket_fd, char simbolo) {
    Giocatore *giocatore = malloc(sizeof(Giocatore));
    if (!giocatore) {
        perror("Errore allocazione giocatore");
        return -1;
    }

    static int id_counter = 1;
    giocatore->id = id_counter++;
    giocatore->socket_fd = socket_fd;
    giocatore->simbolo = simbolo;
    giocatore->partita_attiva = -1;

    printf("Giocatore creato con ID: %d, simbolo: %c\n", giocatore->id, giocatore->simbolo);
    return giocatore->id;
}

#include "client_include/client_models.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#define SERVER_NAME_LEN_MAX 255
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  char server_name[SERVER_NAME_LEN_MAX + 1] = {0};
  int server_port, socket_fd, num_players=2;
  
  get_server_info(argc, argv, server_name, &server_port, &num_players);  // Non usiamo più num_clients
  print_welcome_message();

  socket_fd = connect_to_server(server_name, server_port);

  while (1) {
    // Creazione della partita solo se il giocatore non è già in gioco
    if (create_game(socket_fd) == 0) {
      play_game(socket_fd);
      
      // Gestisci la fine della partita
      char buffer[BUFFER_SIZE];
      receive_game_update(socket_fd, buffer);
      if (strcmp(buffer, "GAME_OVER") == 0) {
        handle_game_end(socket_fd, buffer);
        
        // Dopo la fine della partita, chiedi se il giocatore vuole iniziare una nuova partita
        char choice[BUFFER_SIZE];
        printf("Vuoi iniziare una nuova partita? (sì/no): ");
        scanf("%s", choice);
        if (strcmp(choice, "no") == 0) {
          break; // Esci dal ciclo se il giocatore non vuole iniziare una nuova partita
        }
      }
    }
  }

  close(socket_fd);
  exit(0);
}

// Funzione che genera i parametri del client (in questo caso il tempo di risposta)
void generate_client_params(int *time_to_respond) {
    srand(time(NULL) + getpid());
    *time_to_respond = (rand() % 30) + 10;  // Ad esempio, tempo di risposta tra 10 e 30 secondi
}

// Crea una nuova partita
int create_game(int socket_fd) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "CREATE_GAME");
    write(socket_fd, buffer, strlen(buffer));

    receive_game_update(socket_fd, buffer);
    if (strcmp(buffer, "GAME_CREATED") == 0) {
        printf("Partita creata con successo!\n");
        return 0;
    } else {
        printf("Errore nella creazione della partita: %s\n", buffer);
        return -1;
    }
}

// Unisciti a una partita esistente
int join_game(int socket_fd) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "JOIN_GAME");
    write(socket_fd, buffer, strlen(buffer));

    receive_game_update(socket_fd, buffer);
    if (strcmp(buffer, "GAME_JOINED") == 0) {
        printf("Sei stato unito alla partita!\n");
        return 0;
    } else {
        printf("Errore nell'unirsi alla partita: %s\n", buffer);
        return -1;
    }
}

// Funzione che gestisce il ciclo di gioco
void play_game(int socket_fd) {
    int row, col;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Fai la tua mossa (riga colonna): ");
        scanf("%d %d", &row, &col);

        // Invia la mossa al server
        if (send_move(socket_fd, row, col) != 0) {
            break; // Se c'è un errore, esci dal gioco
        }

        // Ricevi l'aggiornamento dello stato del gioco
        receive_game_update(socket_fd, buffer);

        if (strcmp(buffer, "GAME_OVER") == 0) {
            handle_game_end(socket_fd, buffer);
            break;
        }
    }
}

// Invia la mossa del giocatore al server
int send_move(int socket_fd, int row, int col) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "MOVE %d %d", row, col);
    write(socket_fd, buffer, strlen(buffer));

    // Ricevi la risposta dal server
    receive_game_update(socket_fd, buffer);
    if (strcmp(buffer, "MOVE_ACCEPTED") != 0) {
        printf("Mossa non accettata: %s\n", buffer);
        return -1;
    }

    return 0;
}

// Ricevi aggiornamenti sullo stato del gioco
void receive_game_update(int socket_fd, char *buffer) {
    int bytes_received = read(socket_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_received <= 0) {
        perror("Errore nella lettura dal server");
        exit(1);
    }
    buffer[bytes_received] = '\0';  // Aggiungi terminatore di stringa
    printf("Aggiornamento: %s\n", buffer);
}

// Gestisci la fine della partita
void handle_game_end(int socket_fd, const char *game_status) {
    if (strcmp(game_status, "VICTORY") == 0) {
        printf("Hai vinto la partita!\n");
    } else if (strcmp(game_status, "DEFEAT") == 0) {
        printf("Hai perso la partita.\n");
    } else if (strcmp(game_status, "DRAW") == 0) {
        printf("La partita è finita in pareggio.\n");
    }
    
    // Chiedi ai giocatori se vogliono iniziare una nuova partita
    char buffer[BUFFER_SIZE];
    printf("Vuoi iniziare una nuova partita? (sì/no): ");
    scanf("%s", buffer);
    if (strcmp(buffer, "sì") == 0) {
        create_game(socket_fd);  // Crea una nuova partita
    } else {
        printf("Grazie per aver giocato!\n");
    }
}

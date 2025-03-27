#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "funzioni.h"


// Funzione per validare l'input del menu
int get_valid_menu_choice(int min, int max) {
    char input[BUFFER_SIZE];
    int choice;
    int retries = 0;

    while (retries < MAX_RETRIES) {
        printf("\nScelta (1-%d): ", max);
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            continue;
        }

        if (sscanf(input, "%d", &choice) == 1 && choice >= min && choice <= max) {
            return choice;
        }

        printf("\nScelta non valida. Per favore inserisci un numero tra %d e %d\n", min, max);
        retries++;
    }
    return -1;
}

// Funzione per ottenere la mossa del client
int get_valid_move(char *input) {
    int row, col;
    while (1) {
        printf("\nInserisci la tua mossa (riga,colonna) [0-2]: ");
        
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            continue;
        }

        // Rimuove i caratteri di nuova linea (\n o \r\n) se presenti
        input[strcspn(input, "\r\n")] = 0;

        // Se l'input è vuoto, invia "1" al server per chiedere il tabellone
        if (strlen(input) == 0) {
            strcpy(input, "1"); // Imposta "1" per il tabellone
            return 0; // Ritorna 0 per segnalare che è stato premuto INVIO
        }

        // Se il formato non è valido (non è riga,colonna), chiedi di nuovo
        if (sscanf(input, "%d,%d", &row, &col) != 2) {
            printf("\nFormato non valido! Usa: numero,numero\n");
            continue;
        }
        
        if (row < 0 || row > 2 || col < 0 || col > 2) {
            printf("\nPosizione non valida! Usa numeri tra 0 e 2\n");
            continue;
        }

        return 1; // Input valido
    }
}



// Funzione per ottenere la mossa del client
int get_valid_match(char *input) {
    while (1) {
        printf("\nInserisci la tua mossa (riga,colonna) [0-2]: ");
        
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            continue;
        }

        // Rimuove i caratteri di nuova linea (\n o \r\n) se presenti
        input[strcspn(input, "\r\n")] = 0;

        // Se l'input è vuoto, invia "1" al server per chiedere il tabellone
        if (strlen(input) == 0) {
            strcpy(input, "1"); // Imposta "1" per il tabellone
            return 0; // Ritorna 0 per segnalare che è stato premuto INVIO
        }

        if (strcmp(input, "1") == 0 || strcmp(input, "2") == 0 || strcmp(input, "3") == 0 || strcmp(input, "4") == 0 || strcmp(input, "5") == 0 || strcmp(input, "6") == 0 || strcmp(input, "7") == 0 || strcmp(input, "8") == 0 || strcmp(input, "9") == 0)
        {
            return 1; // Input valido
        }
        

        return 1; // Input valido
    }
}

void play_again_menu(int socket_fd, int is_winner) {
    CLEAR_SCREEN();
    printf("\n=== Partita Terminata ===\n");
    if (is_winner) {
        printf("1. Gioca ancora\n");
        printf("2. Esci\n");
    } else {
        printf("Hai perso. Tornerai al menu principale.\n");
        send(socket_fd, "2", 1, 0);  // Invia direttamente "2" per uscire
        return;
    }

    int choice = get_valid_menu_choice(1, 2);
    char choice_str[2];
    sprintf(choice_str, "%d", choice);
    send(socket_fd, choice_str, strlen(choice_str), 0);

    // Attendi risposta dal server
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(socket_fd, buffer, BUFFER_SIZE - 1, 0) <= 0) {
        printf("Connessione al server persa.\n");
        exit(EXIT_FAILURE);
    }

    if (strstr(buffer, "wait")) {
        printf("In attesa dell'avversario...\n");
        return;
    }

    if (strstr(buffer, "exit")) {
        exit(EXIT_SUCCESS);
    }
    // Altrimenti continua con il gioco
}

void start_game(int socket_fd) {
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    
    while (1) {
        // Ricevi stato del gioco
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(socket_fd, buffer, BUFFER_SIZE - 1, 0) <= 0) {
            printf("Connessione al server persa.\n");
            exit(EXIT_FAILURE);
        }
        CLEAR_SCREEN();
        printf("%s", buffer);  // Mostra il tabellone

        // Ricevi messaggio del turno
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(socket_fd, buffer, BUFFER_SIZE - 1, 0) <= 0) {
            printf("Connessione al server persa.\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", buffer);

        // Se la partita è finita, gestisci il risultato
        if (strstr(buffer, "vinto") || strstr(buffer, "Pareggio") || strstr(buffer, "disconnesso")) {
            int is_winner = strstr(buffer, "vinto") ? 1 : 0;
            printf("\nPremi INVIO per continuare...");
            getchar();
            play_again_menu(socket_fd, is_winner);
            break;
        }

        // Se è il turno del giocatore, chiedi la mossa
        if (strstr(buffer, "tuo turno")) {
            while (1) {
                memset(input, 0, BUFFER_SIZE);
                if (get_valid_move(input)) {
                    send(socket_fd, input, strlen(input), 0);
                    break; // Manda l'input valido e esce dal loop
                }
            }
        } else {
            // Aspetta INVIO, ma senza inviare newline vuoti al server
            // Solo quando non è il turno del giocatore
            if (strlen(input) > 1) { // Evita di inviare solo "\n"
                send(socket_fd, input, strlen(input), 0);
            }
        }
    }
}

void handle_menu(int socket_fd) {
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    
    while (1) {
        // Ricevi messaggio dal server
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(socket_fd, buffer, BUFFER_SIZE, 0) <= 0) {
            printf("Connessione al server persa.\n");
            exit(EXIT_FAILURE);
        }
        
        CLEAR_SCREEN();
        printf("%s", buffer);
        
        // Se è richiesto un input
        if (strstr(buffer, "Scegli") || strstr(buffer, "Premi INVIO") || 
            strstr(buffer, "nome della partita") || strstr(buffer, "numero della partita")) {
            
            fgets(input, BUFFER_SIZE, stdin);
            if (send(socket_fd, input, strlen(input), 0) <= 0) {
                printf("Errore nell'invio dei dati al server.\n");
                exit(EXIT_FAILURE);
            }
            
            // Se stiamo attendendo una partita
            if (strstr(buffer, "nome della partita")) {
                memset(buffer, 0, BUFFER_SIZE);
                recv(socket_fd, buffer, BUFFER_SIZE, 0);
                printf("%s", buffer);
                
                if (strstr(buffer, "In attesa")) {
                    printf("\nIn attesa di un avversario... (Ctrl+C per annullare)\n");
                    recv(socket_fd, buffer, BUFFER_SIZE, 0);
                    break;
                }
            }
            
            // Se ci siamo uniti a una partita
            if (strstr(buffer, "numero della partita")) {
                memset(buffer, 0, BUFFER_SIZE);
                recv(socket_fd, buffer, BUFFER_SIZE, 0);
                printf("%s", buffer);
                
                if (strstr(buffer, "partita sta per iniziare")) {
                    break;
                }
            }
        }
    }
}

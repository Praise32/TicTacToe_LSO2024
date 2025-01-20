#include "../client_include/gui.h"
#include <stdlib.h>
#include <stdio.h>

void clear_screen() {
    system("clear");
}

// Messaggio di benvenuto
void print_welcome_message() {
    clear_screen();
    printf("\033[1;36m");  // Colore ciano
    printf("+-------------------------------------------------------+\n");
    printf("|                                                       |\n");
    printf("|          🎉 Benvenuto al Gioco del Tris! 🎉           |\n");
    printf("|                                                       |\n");
    printf("|        🕹️ Gioca con amici e dimostra il tuo 🧠        |\n");
    printf("|                                                       |\n");
    printf("|    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    |\n");
    printf("|      🟡 🟡 🟡 Creazione e partecipazione ai giochi    |\n");
    printf("|    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    |\n");
    printf("|                                                       |\n");
    printf("|    Buona fortuna e che vinca il migliore!             |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\033[0m");  // Reset colore
}

// Mostra il tabellone del Tris
void print_game_board(char board[3][3]) {
    clear_screen();
    printf("\033[1;34m");  // Colore blu
    printf("+-----------------------+\n");
    printf("|       TRIS BOARD      |\n");
    printf("+-----------------------+\n");
    for (int i = 0; i < 3; i++) {
        printf("| ");
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i][j]);
            if (j < 2) printf("|");
        }
        printf(" |\n");
        if (i < 2) printf("+---+---+---+\n");
    }
    printf("+-----------------------+\n");
    printf("\033[0m");  // Reset colore
}

// Messaggio per la scelta della mossa
void print_move_prompt() {
    printf("\033[1;33m");  // Colore giallo
    printf("+-------------------------------------------------------+\n");
    printf("|    Tocca a te! Inserisci la tua mossa (formato riga,colonna)  |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\033[0m");  // Reset colore
}

// Messaggio di vittoria
void print_victory_message() {
    printf("\033[1;32m");  // Colore verde
    printf("+-------------------------------------------------------+\n");
    printf("|                                                       |\n");
    printf("|      🎉🎉  Complimenti, hai vinto la partita!  🎉🎉   |\n");
    printf("|                                                       |\n");
    printf("|       Sei il campione! Vuoi giocare un'altra partita?  |\n");
    printf("|                                                       |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\033[0m");  // Reset colore
}

// Messaggio di pareggio
void print_draw_message() {
    printf("\033[1;33m");  // Colore giallo
    printf("+-------------------------------------------------------+\n");
    printf("|                                                       |\n");
    printf("|     🤝  È un pareggio! Entrambi i giocatori sono bravi! |\n");
    printf("|                                                       |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\033[0m");  // Reset colore
}

// Messaggio di sconfitta
void print_defeat_message() {
    printf("\033[1;31m");  // Colore rosso
    printf("+-------------------------------------------------------+\n");
    printf("|                                                       |\n");
    printf("|     💥 Hai perso la partita... Ritenta la prossima! 💥 |\n");
    printf("|                                                       |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\033[0m");  // Reset colore
}

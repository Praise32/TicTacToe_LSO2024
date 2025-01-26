#include "../client_include/client_models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialize_game_state(GameState *state, int socket_fd) {
    state->socket_fd = socket_fd;
    state->game_id = -1;
    state->symbol = ' ';
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            state->board[i][j] = ' ';
        }
    }
}

void cleanup_resources(GameState *state) {
    if(state->socket_fd != -1) {
        send(state->socket_fd, "QUIT", 4, 0);
        close(state->socket_fd);
        state->socket_fd = -1;
    }
}

int join_game(GameState *state, int game_id) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "JOIN_GAME %d", game_id);
    write(state->socket_fd, buffer, strlen(buffer));

    receive_game_update(state->socket_fd, buffer);
    if (strcmp(buffer, "GAME_JOINED") == 0) {
        state->game_id = game_id;
        printf("Sei stato unito alla partita %d!\n", game_id);
        return 0;
    } else {
        printf("Errore nell'unirsi alla partita: %s\n", buffer);
        return -1;
    }
}

void receive_game_update(int socket_fd, char *buffer) {
    int bytes_received = read(socket_fd, buffer, 1024 - 1);
    if (bytes_received <= 0) {
        perror("Errore nella lettura dal server");
        exit(1);
    }
    buffer[bytes_received] = '\0';
    printf("Aggiornamento: %s\n", buffer);
}

void play_game(GameState *state) {
    char buffer[1024];
    while (1) {
        print_game_board(state->board);
        print_move_prompt();
        
        int row, col;
        if (scanf("%d %d", &row, &col) != 2 || row < 0 || row > 2 || col < 0 || col > 2) {
            printf("Mossa non valida! Usa numeri da 0 a 2.\n");
            continue;
        }

        if (send_move(state, row, col) == 0) {
            update_game_board(state->board, row, col, state->symbol);
            print_game_board(state->board);
        }

        receive_game_update(state->socket_fd, buffer);
        if (strstr(buffer, "VICTORY")) {
            print_victory_message();
            break;
        } else if (strstr(buffer, "DEFEAT")) {
            print_defeat_message();
            break;
        } else if (strstr(buffer, "DRAW")) {
            print_draw_message();
            break;
        }
    }
}

int send_move(GameState *state, int row, int col) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "MOVE %d %d %d", state->game_id, row, col);
    write(state->socket_fd, buffer, strlen(buffer));

    receive_game_update(state->socket_fd, buffer);
    if (strcmp(buffer, "MOVE_ACCEPTED") == 0) {
        return 0;
    } else {
        printf("Mossa non accettata: %s\n", buffer);
        return -1;
    }
}

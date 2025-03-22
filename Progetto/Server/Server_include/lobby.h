#ifndef LOBBY_H
#define LOBBY_H

void init_lobby(Lobby *lobby);
int create_game_session(Lobby *lobby, const char *name, int host_socket);
int join_game_session(Lobby *lobby, int game_id, int guest_socket);
void list_games(Lobby *lobby, char *buffer);
void remove_game_session(Lobby *lobby, int game_id);

#endif

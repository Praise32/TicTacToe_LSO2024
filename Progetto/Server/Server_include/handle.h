#ifndef Handle_H
#define Handle_H
 
 void send_to_player(int socket, const char *message);
 void display_server_status(int num_clients, int num_games);
 void *handle_game(void *arg);
 void *handle_client_connection(void *arg);
 
 
#endif
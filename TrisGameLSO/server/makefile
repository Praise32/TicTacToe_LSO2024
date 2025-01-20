# Compilazione del server
server: server.o session.o game.o player.o connection.o client_handler.o
	gcc -pthread server.o session.o game.o player.o connection.o client_handler.o -o server

server.o: server.c
	gcc -c -pthread server.c -o server.o

session.o: server_models_impl/session.c
	gcc -c -pthread server_models_impl/session.c -o session.o

game.o: server_models_impl/game.c
	gcc -c -pthread server_models_impl/game.c -o game.o

player.o: server_models_impl/player.c
	gcc -c -pthread server_models_impl/player.c -o player.o

connection.o: server_models_impl/connection.c
	gcc -c -pthread server_models_impl/connection.c -o connection.o

client_handler.o: server_models_impl/client_handler.c
	gcc -c -pthread server_models_impl/client_handler.c -o client_handler.o

clean:
	rm -f server server.o session.o game.o player.o connection.o client_handler.o

run_server: server
	./server 10000

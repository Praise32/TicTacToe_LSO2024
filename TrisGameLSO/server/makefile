# Obiettivo principale
server: server.o session.o game.o player.o connection.o
	 gcc -pthread server.o session.o game.o player.o connection.o -o server

# Compilazione dei file sorgente
server.o: 
	 gcc -c -pthread server.c -o server.o

connection.o:
	gcc -c -pthread server_models_impl/connection.c -o connection.o

session.o: 
	 gcc -c -pthread server_models_impl/session.c -o session.o

game.o: 
	 gcc -c -pthread server_models_impl/game.c -o game.o

player.o: 
	 gcc -c -pthread server_models_impl/player.c -o player.o

# Pulizia dei file oggetto e dell'eseguibile
clean:
	 rm -f server.o session.o game.o player.o server connection.o

run_server: server
	./server 10000 127.0.0.1 5
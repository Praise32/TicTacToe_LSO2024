server: server.o game_manager.o
    gcc -pthread server.o game_manager.o -o server

server.o: server.c Server_include/game_logic.h
    gcc -c -pthread server.c -o server.o

game_manager.o: Server_impl/game_manager.c Server_include/game_logic.h
    gcc -c -pthread Server_impl/game_manager.c -o game_manager.o

clean:
    rm -f server.o game_manager.o server

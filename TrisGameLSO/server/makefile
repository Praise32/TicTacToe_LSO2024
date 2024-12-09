# Variabili
CC = gcc
CFLAGS = -Wall -pthread -g
TARGET = server

# File sorgenti
SRC = server.c server_models_impl/connection.c server_models_impl/game.c \
      server_models_impl/player.c server_models_impl/session.c

# File oggetto
OBJ = server.o server_models_impl/connection.o server_models_impl/game.o \
      server_models_impl/player.o server_models_impl/session.o

# Obiettivo predefinito
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regola per compilare server.c
server.o: server.c
	$(CC) $(CFLAGS) -c server.c -o server.o

# Regola per compilare connection.c
server_models_impl/connection.o: server_models_impl/connection.c
	$(CC) $(CFLAGS) -c server_models_impl/connection.c -o server_models_impl/connection.o

# Regola per compilare game.c
server_models_impl/game.o: server_models_impl/game.c
	$(CC) $(CFLAGS) -c server_models_impl/game.c -o server_models_impl/game.o

# Regola per compilare player.c
server_models_impl/player.o: server_models_impl/player.c
	$(CC) $(CFLAGS) -c server_models_impl/player.c -o server_models_impl/player.o

# Regola per compilare session.c
server_models_impl/session.o: server_models_impl/session.c
	$(CC) $(CFLAGS) -c server_models_impl/session.c -o server_models_impl/session.o

# Esegui il server
run_server: $(TARGET)
	./server 10000 127.0.0.1 5

# Pulisce i file oggetto e l'eseguibile
clean:
	rm -f $(TARGET) $(OBJ)

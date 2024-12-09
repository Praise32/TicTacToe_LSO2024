# Variabili
CC = gcc
CFLAGS = -Wall -pthread -g
TARGET = client

# File sorgenti
SRC = client.c client_models_impl/receive.c client_models_impl/connection.c client_models_impl/gui.c

# File oggetto
OBJ = client.o client_models_impl/receive.o client_models_impl/connection.o client_models_impl/gui.o

# Obiettivo predefinito
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regola per compilare il client
client.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

# Regola per compilare receive.c
client_models_impl/receive.o: client_models_impl/receive.c
	$(CC) $(CFLAGS) -c client_models_impl/receive.c -o client_models_impl/receive.o

# Regola per compilare connection.c
client_models_impl/connection.o: client_models_impl/connection.c
	$(CC) $(CFLAGS) -c client_models_impl/connection.c -o client_models_impl/connection.o

# Regola per compilare gui.c
client_models_impl/gui.o: client_models_impl/gui.c
	$(CC) $(CFLAGS) -c client_models_impl/gui.c -o client_models_impl/gui.o

# Esegui il client
run_client: $(TARGET)
	./client 127.0.0.1 10000 20

# Pulisce i file oggetto e l'eseguibile
clean:
	rm -f $(TARGET) $(OBJ)

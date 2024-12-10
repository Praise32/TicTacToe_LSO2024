CC = gcc
CFLAGS = -Wall -pthread -g
TARGET = client

SRC = client.c client_models_impl/receive.c client_models_impl/connection.c client_models_impl/gui.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

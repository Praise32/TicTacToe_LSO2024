CC = gcc
CFLAGS = -Wall -pthread
INCLUDES = -IServer_include

SRCS = server.c \
       Server_impl/game_manager.c \
       Server_impl/lobby.c

OBJS = $(SRCS:.c=.o)
TARGET = server

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean

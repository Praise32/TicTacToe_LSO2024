CC = gcc
CFLAGS = -Wall -pthread
INCLUDES = -IServer_include

# All source files
SRCS = server.c \
	Server_impl/game_manager.c \
	Server_impl/lobby.c \
	Server_impl/handle.c

# Object files
OBJS = $(SRCS:.c=.o)
TARGET = server

# Header files
HEADERS = Server_include/structure.h \
	Server_include/game_logic.h \
	Server_include/lobby.h \
	Server_include/handle.h

# Main target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

# Pattern rule for object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Test target
test: test_game_logic.c Server_impl/game_manager.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o test_game_logic
	./test_game_logic

# Debug target
debug: CFLAGS += -g
debug: $(TARGET)

.PHONY: clean test debug
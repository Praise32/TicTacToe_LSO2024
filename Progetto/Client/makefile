CC = gcc
CFLAGS = -Wall
INCLUDES = -I.

SRCS = client.c funzioni.c
OBJS = $(SRCS:.c=.o)
TARGET = client

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

%.o: %.c funzioni.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean

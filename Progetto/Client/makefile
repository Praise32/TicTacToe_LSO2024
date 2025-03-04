CC = gcc
CFLAGS = -Wall

SRCS = client.c
OBJS = $(SRCS:.c=.o)
TARGET = client

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean

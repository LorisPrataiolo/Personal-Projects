# Variables - making it easy to change the compiler or flags later
CC = gcc
CFLAGS = -Wall -Wextra -g

# The 'all' target is what runs when you just type 'make'
all: client server

# Rule to build the client
client: client.c
	$(CC) $(CFLAGS) -o client client.c

# Rule to build the server
server: server.c
	$(CC) $(CFLAGS) -o server server.c

# Rule to delete the executables (so you can start fresh)
clean:
	rm -f client server
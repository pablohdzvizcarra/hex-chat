CC = gcc
CFLAGS = -Wall -Wextra -I./src/common
LDFLAGS = 

# source files
SRC = src/client/client.c src/common/protocol.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: client server

client: $(OBJ)
	$(CC) $(OBJ) -o client $(LDFLAGS)

server:
	$(CC) -o server src/server/server.c

clean:
	rm -f $(OBJ) client server
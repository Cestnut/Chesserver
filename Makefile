.PHONY: all client server
all: client server

CC=gcc
CFLAGS= -Wall
LIBS= -lpthread

client: client/client.c
	$(CC) $(CFLAGS) client/client.c client/connection.c common/common.c common/chess.c -o client.out $(LIBS)

server: server/server.c
	$(CC) $(CFLAGS) server/server.c server/server_connection.c server/game_handling.c common/common.c common/chess.c -o server.out $(LIBS)


clean: client server
	rm client.out
	rm server.out
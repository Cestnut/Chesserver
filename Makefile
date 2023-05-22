all: client server

CC=gcc
CFLAGS= -Wall
LIBS= -lpthread

client: client/client.c
	$(CC) $(CFLAGS) client/client.c client/connection.c common/common.c -o client.out $(LIBS)

server: server/server.c
	$(CC) $(CFLAGS) server/server.c server/server_connection.c common/common.c -o server.out $(LIBS)


clean: client server
	rm client
	rm server
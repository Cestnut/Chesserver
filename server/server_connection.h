#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <pthread.h>

#define PORT 5555
#define BUFFER_LEN 1024

//Funzione per creare la server socket che riceve le connessioni
int create_server_socket(int port);


//Funzioni e strutture dati per gestire i client
typedef struct worker_args{
    int client_fd;
} worker_args;
void *client_worker(void* args);

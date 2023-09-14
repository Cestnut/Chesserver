#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#define PORT 5555
#define h_addr h_addr_list[0]
#define BUFFER_LEN 1024
#include "../common/common.h"

int connect_to_server(char* hostname, int port);
void send_token(int client_fd);
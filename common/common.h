#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define FALSE 0
#define TRUE 1

//enum begins from 1 because values are casted from string with atoi(). 
//atoi returns 0 when an invalid character is met.

typedef enum {RUNNING=1, CHECKMATE, STALEMATE, TIMEOUT} game_status;
typedef enum {CREATE_GAME=1, JOIN_GAME, EXIT} client_choice;
typedef enum {INVALID_MOVE=1, VALID_MOVE} move_validation_result;
typedef enum{
    NO_ERROR=0,
    GAME_NAME_TAKEN,
    GAME_DOESNT_EXIST,
    GAME_FULL,
    INVALID_INPUT
} error;


void flush_stdin();
int is_number(char *string);
void strip_newlines(char *buffer, size_t len);

//Sends buffer and a newline to socket
ssize_t sendline(int client_fd, char *buffer, size_t buffer_len, int flags);
//Receives from socket one byte at a time until newline character is met
ssize_t recvline(int sockfd, char *buf, size_t len, int flags);
//Sends integer value to socket
ssize_t sendint(int client_fd, int value, int flags);
//Receives an integer (4 bytes) from socket
ssize_t recvint(int client_fd, int *result, int flags);

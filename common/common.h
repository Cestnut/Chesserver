#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

//enum begins from 1 because values are casted from string with atoi(). 
//atoi returns 0 when an invalid character is met.
typedef enum {CREATE_GAME=1, JOIN_GAME, EXIT} client_choice;

typedef enum{
    NO_ERROR=0,
    GAME_NAME_TAKEN,
    GAME_DOESNT_EXIST,
    GAME_FULL,
    INVALID_INPUT
} error;


void flush_stdin();
int is_number(char *string);
char *random_string(int bytes_length);
ssize_t recvline(int sockfd, void *buf, size_t len, int flags);


#define GAME_NAME_MAX_LENGTH 32
#define TOKEN_LENGTH 64
#define MAX_PLAYERS 2
#define DEBUG 1
#define FALSE 0
#define TRUE 1
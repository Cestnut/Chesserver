#include "common.h"
#include <errno.h>

void flush_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Discard characters until newline or end of file
    }
}

int is_number(char *string){
    for (char *chr = string ; *chr && *chr != '\n'; chr++){
        if (!isdigit(*chr))
        return 0;
    }
    return 1;
}

void strip_newlines(char *buffer, size_t len){
    for(size_t i=0; i<len; i++){
        if(buffer[len-i]=='\n'){
            buffer[len-i] = 0x0;
            break;
        }
    }
}

ssize_t recvline(int sockfd, void *buf, size_t len, int flags){
    ssize_t bytes_read = recv(sockfd, buf, len, flags);
    if(bytes_read == -1){
        printf("Error receiving message: errno %d\n", errno);
    }
    strip_newlines(buf, len);
    return bytes_read;
}
#include "common.h"

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

char *random_string(int bytes_length){
    //Adds 1 element to account for trailing zero
    char *output_string = malloc((bytes_length * sizeof(char)) + 1);

    for(int i = 0; i < bytes_length; i++){
        //Prints to every character and hex value between 0x0 and 0xF
        sprintf(&output_string[i], "%hhX", rand()%16);
    }
    output_string[bytes_length] = 0x0;

    return output_string;
}

void strip_newlines(char *buffer, size_t len){
    for(size_t i= 1; i<len; i++){
        if(buffer[len-i]=='\n'){
            buffer[len-i] = 0x0;
        }
    }
}

ssize_t recvline(int sockfd, void *buf, size_t len, int flags){
    ssize_t bytes_read = recv(sockfd, buf, len, flags);
    strip_newlines(buf, len);
    return bytes_read;
}
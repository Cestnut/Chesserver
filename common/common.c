#include "common.h"
#include <errno.h>
#include <stdint.h>
#include <arpa/inet.h>

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

ssize_t sendline(int client_fd, char *buffer, size_t buffer_len, int flags){
    char newline[]="\n";
    ssize_t sent_bytes=0;
    strip_newlines(buffer, buffer_len);
    sent_bytes+=send(client_fd, buffer, buffer_len, flags);
    if(sent_bytes == -1) return sent_bytes;
    else if(send(client_fd, newline, strlen(newline), flags) == -1) return -1;
    else return sent_bytes + 1;
}

ssize_t recvline(int client_fd, char *buffer, size_t buffer_len, int flags){
    printf("Receiving line\n");
    ssize_t received_bytes=0, tmp_received_bytes;
    while(received_bytes < buffer_len){
        tmp_received_bytes = recv(client_fd, buffer+received_bytes, 1, flags);
        //Calling function has to handle errors
        if(tmp_received_bytes < 1) return tmp_received_bytes;
        else{
            if(buffer[received_bytes] == '\n'){
                buffer[received_bytes] = '\0';
                received_bytes += tmp_received_bytes;
                break;
            }
            else{
                received_bytes += tmp_received_bytes;
            }
        }
        printf("Bytes read in total: %ld\nTotal data received: %s\n", received_bytes, buffer);
    }
    return received_bytes;
}

ssize_t sendint(int client_fd, int value, int flags){
    int32_t converted_value = htonl(value);
    ssize_t sent_bytes=0;
    sent_bytes+=send(client_fd, &converted_value, sizeof(converted_value), flags);
    return sent_bytes;
}

ssize_t recvint(int client_fd, int *result, int flags){

    ssize_t received_bytes=0, tmp_received_bytes, bytes_to_receive=sizeof(int32_t);
    char tmp_buffer[sizeof(int32_t)];
    while(received_bytes < bytes_to_receive){
        tmp_received_bytes = recv(client_fd, tmp_buffer+received_bytes, sizeof(tmp_buffer)-received_bytes, flags);
        //Calling function has to handle errors
        if(tmp_received_bytes<1) return tmp_received_bytes;
        received_bytes+=tmp_received_bytes;
    }
    *result = ntohl(*(int32_t*)tmp_buffer);
    return received_bytes;
}
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 5555
#define BUFFER_LEN 1024

int main(int argc, char **argv){

    int socket_fd;
    struct sockaddr_in server_addr;
    char input_buffer[BUFFER_LEN], output_buffer[BUFFER_LEN];
    
    memset(input_buffer, 0, BUFFER_LEN);
    memset(output_buffer, 0, BUFFER_LEN);


    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        printf("Error opening socket: errno %d\n", errno);
        exit(0);
    }
    printf("Socket created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Error connecting: errno %d\n", errno);
        exit(0);
    }
    printf("Connected succesfully\n");

    fgets(output_buffer, sizeof(output_buffer), stdin);
    output_buffer[strlen(output_buffer) - 1] = '\0';
    printf("%lu\n", strlen(output_buffer));
    if(send(socket_fd, output_buffer, strlen(output_buffer), 0) == -1){
        printf("Error sending message: errno %d\n", errno);
        exit(0);
    }
    printf("Message sent\n");

    if(recv(socket_fd, input_buffer, sizeof(input_buffer), 0) == -1){
        printf("Error receiving message: errno %d\n", errno);
        exit(0);
    }
    printf("Message received\n");

    printf("client message: %s\n", output_buffer);
    printf("server message: %s\n", input_buffer);

    close(socket_fd);


    return 0;
}

/*
1: create socket
2: set port and IP
3: bind to the set port and ip
4: listen for clients
5: accept incoming connection
6: Receive client's message and send response
7: close socket


sockaddr_in.sin_family
sockaddr_in.sin_port
sockaddr_in.sin_addr.s_addr
listen
accept
recv
send
*/
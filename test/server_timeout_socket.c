#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#define PORT 5555
#define BUFFER_LEN 1024

int main(int argc, char **argv){

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char input_buffer[BUFFER_LEN];
    
    memset(input_buffer, 0, BUFFER_LEN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        printf("Error opening socket: errno %d\n", errno);
        exit(0);
    }
    printf("Server socket created\n");
    
    //Tell the OS to reuse the socket
    const int enable;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Error binding socket: errno %d\n", errno);
        exit(0);
    }
    printf("Socket bound\n");

    if(listen(server_fd, 1) == -1){
        printf("Error listening: errno %d\n", errno);
        exit(0);
    }
    printf("Listening\n");
    
    
    client_addr_len = sizeof(client_addr);  
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
    if(client_fd == -1){
        printf("Error accepting connection: errno %d\n", errno);
        exit(0);
    }

    printf("Connection received\n");

    int nfds = 1, ready;
    struct pollfd *pfds;

    pfds = calloc(nfds, sizeof(struct pollfd));
    pfds[0].fd = client_fd;
    pfds[0].events = POLLIN;
    pfds[0].revents = POLLIN;
    sleep(2);
    ready = poll(pfds, nfds, 3);


    clock_t beginning, end;
    time_t beginning_t, end_t;
    beginning = clock();
    time(&beginning_t);

    if(ready > 0){
        recv(client_fd, input_buffer, sizeof(input_buffer), 0);
        send(client_fd, input_buffer, strlen(input_buffer), 0);
    }
    else if(ready == 0){
        printf("Timeout\n");
    }
    else{
        printf("Error polling client_fd: errno %d\n", errno);
        exit(0);
    }

    end = clock();
    time(&end_t);

    printf("%e\n", (end - beginning)/CLOCKS_PER_SEC);
    printf("%ld\n", (end_t - beginning_t));
    
    close(client_fd);
    close(server_fd);

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
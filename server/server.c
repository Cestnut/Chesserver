#include "server_connection.h"


int main(int argc, char **argv){

    int server_fd, client_fd;
    int port = 5555;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    char input_buffer[BUFFER_LEN];
    
    memset(input_buffer, 0, BUFFER_LEN);

    server_fd = create_server_socket(port);
    
    pthread_t tid;
    worker_args *worker_args;
    client_addr_len = sizeof(client_addr);  
    while(1){
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
        if(client_fd == -1){
            printf("Error accepting connection: errno %d\n", errno);
            exit(0);
        }
        printf("Connection received\n");

        worker_args = malloc(sizeof(worker_args));
        worker_args->client_fd = client_fd;

        pthread_create(&tid, NULL, client_worker, (void *)worker_args);    
    }
        int nfds = 1, ready;
        struct pollfd *pfds;

        pfds = calloc(nfds, sizeof(struct pollfd));
        pfds[0].fd = client_fd;
        pfds[0].events = POLLIN;
        pfds[0].revents = POLLIN;
        ready = poll(pfds, nfds, 3000);

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
#include "server_connection.h"
#include "game_handling.h"
#include <signal.h>
    

int main(int argc, char **argv){

    int server_fd, client_fd;
    int port = 5555;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    server_fd = create_server_socket(port);
    
    //Ignores SIGPIPE signal
    sigaction(SIGPIPE, &(struct sigaction){{SIG_IGN}}, NULL);
    srand(time(NULL));
    init_games();

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
        
    close(server_fd);

    return 0;
}
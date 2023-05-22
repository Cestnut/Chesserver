#include "server_connection.h"
#include "../common/common.h"
#include <pthread.h>

int create_server_socket(int port){
    int server_fd;
    struct sockaddr_in server_addr;

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

    if(listen(server_fd, 0) == -1){
        printf("Error listening: errno %d\n", errno);
        exit(-1);
    }
    printf("Listening\n");

    return server_fd;
}

void *client_worker(void *args){
    worker_args *data = (worker_args*)args;
    int client_fd = data->client_fd;
    
    char input_buffer;
    client_choice choice;
    ssize_t bytes_read;
    while(1){
        bytes_read = recv(client_fd, &input_buffer, sizeof(char), 0);
        if(bytes_read){
            printf("%c\n", input_buffer);        
            choice = atoi(&input_buffer);
            switch(choice){
                case CREATE_GAME:
                    printf("GAME CREATED\n");
                    break;
                    
                case JOIN_GAME:
                    printf("GAME JOINED\n");
                    break;

                case SPECTATE_GAME:
                    printf("SPECTATING GAME\n");
                    break;

                case EXIT:
                    printf("GAME EXITED\n");        
                    pthread_exit(0);
                    break;
            }
        }
        else{
            //Exit from thread
            printf("Exiting Thread\n");
            pthread_exit(0);
        }
    }
    return NULL;
}
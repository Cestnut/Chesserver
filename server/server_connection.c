#include "server_connection.h"
#include "../common/common.h"
#include "game_handling.h"
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
    
    char input_buffer[64];
    char game_name[GAME_NAME_MAX_LENGHT]; 
    unsigned int timer_lenght, max_spectators;
    client_choice choice;
    ssize_t bytes_read;
    error error_code = 0;

    while(1){
        bytes_read = recv(client_fd, &choice, sizeof(client_choice), 0);
        if(bytes_read){
            switch(choice){
                case CREATE_GAME:
                    printf("GAME CREATED\n");
                    recv(client_fd, &input_buffer, sizeof(game_name), 0);

                    char *token;
                    token = strtok(input_buffer, ":");
                    strncpy(game_name, token, GAME_NAME_MAX_LENGHT);
                        
                    token = strtok(NULL, "");
                    if(token == NULL || !(is_number(token))){
                        error_code = INVALID_INPUT;
                        break;
                    }
                    else{
                        timer_lenght = strtoul(token,0,0);
                    }
                    
                    token = strtok(NULL, "");
                    if(token == NULL || !(is_number(token))){
                        error_code = INVALID_INPUT;
                        break;
                    }
                    else{
                        max_spectators = strtoul(token,0,0);
                    } 

                    error_code = create_game(client_fd, game_name, timer_lenght, max_spectators);
                    break;
                    
                case JOIN_GAME:
                    printf("GAME JOINED\n");
                    error_code = join_game(client_fd, game_name);
                    break;

                case SPECTATE_GAME:
                    printf("SPECTATING GAME\n");
                    error_code = spectate_game(client_fd, game_name);
                    break;

                case EXIT:
                    printf("GAME EXITED\n");        
                    pthread_exit(0);
                    break;
            }
            send(client_fd, &error_code, 1, 0);
        }
        else{
            //Exit from thread
            printf("Exiting Thread\n");
            pthread_exit(0);
        }
    }
    return NULL;
}

error create_game(int client_fd, char *game_name, unsigned int timer_lenght, unsigned int max_spectators){
    error error_code;
    
    error_code = insert_game(game_name, timer_lenght, max_spectators);

    if(error_code){
        return error_code;
    }
    else{
        return join_game(game_name, client_fd);
    }
}

error join_game(int client_fd, char *game_name){

}

error spectate_game(int client_fd, char *game_name){

}
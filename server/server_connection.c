#include "server_connection.h"

error join_game(int client_fd, char *game_name);
error create_game(int client_fd, char *game_name, unsigned int timer_length);

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

char *receive_token(int client_fd){
    char *token = malloc((TOKEN_LENGTH+1) * sizeof(char));
    recv(client_fd, token, (TOKEN_LENGTH+1), 0);
    printf("%s\n", token);
    printf("%d\n", strlen(token));
    if(strlen(token)!=TOKEN_LENGTH){
        free(token);
        token = random_string(TOKEN_LENGTH);
        printf("%d\n", strlen(token));
        send(client_fd, token, (TOKEN_LENGTH+1), 0);
    }
    return token;

}

void *client_worker(void *args){
    worker_args *data = (worker_args*)args;
    int client_fd = data->client_fd;
    
    char input_buffer[64];
    char game_name[GAME_NAME_MAX_LENGTH]; 
    unsigned int timer_length;
    client_choice choice;
    ssize_t bytes_read;
    error error_code = 0;

    char *client_token = receive_token(client_fd);
    printf("%s\n", client_token);

    while(1){
        bytes_read = recv(client_fd, &choice, sizeof(client_choice), 0);
        if(bytes_read){
            switch(choice){
                case CREATE_GAME:
                    printf("GAME CREATED\n");
                    recv(client_fd, &input_buffer, sizeof(game_name), 0);

                    char *split_token;
                    split_token = strtok(input_buffer, ":");
                    strncpy(game_name, split_token, GAME_NAME_MAX_LENGTH);
                        
                    split_token = strtok(NULL, "");
                    if(split_token == NULL || !(is_number(split_token))){
                        error_code = INVALID_INPUT;
                        break;
                    }
                    else{
                        timer_length = strtoul(split_token,0,0);
                    }

                    error_code = create_game(client_fd, game_name, timer_length);
                    break;
                    
                case JOIN_GAME:
                    printf("GAME JOINED\n");
                    error_code = join_game(client_fd, game_name);
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

error create_game(int client_fd, char *game_name, unsigned int timer_length){
    error error_code;
    
    error_code = insert_game(game_name, timer_length);

    if(error_code){
        return error_code;
    }
    else{
        return join_game(client_fd, game_name);
    }
}

error join_game(int client_fd, char *game_name){
    error error_code;
    game *game = get_game(game_name);
    player* new_player = malloc(sizeof(player));
    new_player->socket_fd = client_fd;
    new_player->timer = game->match_data->timer_length;
    new_player->next_player = NULL;
    game->match_data->connected_players++;

    if(game->match_data->connected_players == 0){
        game->match_data->players = new_player;
        error_code = NO_ERROR;
    }
    else if(game->match_data->max_players < game->match_data->connected_players){
        game->match_data->players->next_player = new_player;
        error_code = NO_ERROR;
    }
    else{
        error_code = GAME_FULL;
    }
    return error_code;
}
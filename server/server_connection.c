#include "server_connection.h"

error join_game(int client_fd, char *game_name, char *token);
error create_game(int client_fd, char *client_token, char *game_name, unsigned int timer_length);
int validate_token(char *game_name, char *token);


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
    if (DEBUG) printf("Received token %s of length: %lu\n", token, strlen(token));
    if(strlen(token)!=TOKEN_LENGTH){
        free(token);
        token = random_string(TOKEN_LENGTH);
        send(client_fd, token, (TOKEN_LENGTH+1), 0);
        if(DEBUG) printf("Sent token length: %lu\n", strlen(token));        
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

    while(1){
        bytes_read = recv(client_fd, &choice, sizeof(client_choice), 0);
        if(bytes_read){
            switch(choice){
                case CREATE_GAME:
                    recvline(client_fd, input_buffer, sizeof(input_buffer), 0);

                    char *split_token;
                    split_token = strtok(input_buffer, ":");
                    strncpy(game_name, split_token, GAME_NAME_MAX_LENGTH);
                        
                    split_token = strtok(NULL, "");
                    if(split_token == NULL || !(is_number(split_token))){
                        error_code = INVALID_INPUT;
                        if(DEBUG) printf("Input in creating room is not valid, wrong token: %s\n", split_token);
                        
                        break;
                    }
                    else{
                        timer_length = strtoul(split_token,0,0);
                    }

                    error_code = create_game(client_fd, client_token, game_name, timer_length);
                    printf("GAME CREATED\n");
                    break;
                    
                case JOIN_GAME:
                    recvline(client_fd, game_name, sizeof(game_name), 0);
                    error_code = join_game(client_fd, game_name, client_token);
                    printf("GAME JOINED\n");
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

error create_game(int client_fd, char *client_token, char *game_name, unsigned int timer_length){
    error error_code;
    
    error_code = insert_game(game_name, timer_length);

    if(error_code != NO_ERROR){
        return error_code;
    }
    else{
        if (DEBUG) printf("Game created. Game name: %s", game_name);
        return join_game(client_fd, game_name, client_token);
    }
}

error join_game(int client_fd, char *game_name, char *token){
    error error_code;
    game *game = get_game(game_name);
    //Checks first whether a game with the given name exists
    if(game == NULL){
        if(DEBUG){
            printf("Game %s does not exist\n", game_name);
        }        
        error_code = GAME_DOESNT_EXIST;
        return error_code;
    }
    player* new_player = malloc(sizeof(player));
    new_player->socket_fd = client_fd;
    new_player->timer = game->match_data->timer_length;
    new_player->next_player = NULL;
    new_player->token = token;


    pthread_rwlock_wrlock(&game->rwlock);

    //If game is empty
    if(game->match_data->connected_players == 0){
        game->match_data->players = new_player;
        game->match_data->connected_players++;
        error_code = NO_ERROR;

        if(DEBUG){
            printf("Game is empty, joining player\n");
        }
    }//If game is not full
    else if(game->match_data->max_players < game->match_data->connected_players){
        game->match_data->players->next_player = new_player;
        game->match_data->connected_players++;
        error_code = NO_ERROR;
        if(DEBUG){
            printf("Game is not full, joining player\n");
        }        

    }//If game is full but the player is playing
    else if(validate_token(game_name, token)){
        error_code = NO_ERROR;
        if(DEBUG){
            printf("Game full, but token %s is validated\n", token);
        }
    }//If the game is full and the player is not playing
    else{
        if(DEBUG){
            printf("Game is full, player cannot join\n");
        }
        free(new_player);
        error_code = GAME_FULL;
    }

    pthread_rwlock_unlock(&game->rwlock);
    return error_code;
}

//Given a name and a token, tells whether one of the players has the given token 
int validate_token(char *game_name, char *token){
    game *game_data = get_game(game_name);
    if(game_data != NULL){
        player *curr = game_data->match_data->players;
        for(int i=0; i<game_data->match_data->connected_players && curr!=NULL; i++){
            if(!strcmp(curr->token, token)){
                if(DEBUG){
                    printf("Token %s is validated\n", token);
                }
                return 1;
            }
            curr = curr->next_player;
        }
    }
    return 0;
}
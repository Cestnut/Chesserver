#include "server_connection.h"

error join_game(int client_fd, char *game_name);
error create_game(int client_fd, char *game_name, unsigned int timer_length);
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

void *client_worker(void *args){
    worker_args *data = (worker_args*)args;
    int client_fd = data->client_fd;
    
    char input_buffer[BUFFER_LEN];
    char game_name[BUFFER_LEN]; 
    unsigned int timer_length;
    client_choice choice;
    ssize_t bytes_read;
    error error_code = 0;

    while(1){
        bytes_read = recv(client_fd, &choice, sizeof(client_choice), 0);
        if(bytes_read){
            switch(choice){
                case CREATE_GAME:
                    recvline(client_fd, input_buffer, sizeof(input_buffer), 0);
                    //Stops the routine if an empty string has been sent 
                    // (can happen in case the client closes the connection)
                    if(*input_buffer == 0) break;
                    
                    char *split_token;
                    split_token = strtok(input_buffer, ":");
                    strncpy(game_name, split_token, GAME_NAME_MAX_LENGTH);
                        
                    split_token = strtok(NULL, "");
                    if(split_token == NULL || !(is_number(split_token))){
                        error_code = INVALID_INPUT;
                        if(DEBUG) printf("Input in creating room is not valid, split_token: %s\n", split_token);
                        
                        break;
                    }
                    else{
                        timer_length = strtoul(split_token,0,0);
                    }

                    error_code = create_game(client_fd, game_name, timer_length);
                    break;
                    
                case JOIN_GAME:
                    recvline(client_fd, game_name, sizeof(game_name), 0);
                    if(*game_name == 0) break;
                    error_code = join_game(client_fd, game_name);
                    break;

                case EXIT:
                    pthread_exit(0);
                    break;
            }
            send(client_fd, &error_code, 1, 0);
        }
        else{
            //Exit from thread
            if (DEBUG) printf("Exiting Thread\n");
            pthread_exit(0);
        }
    }
    return NULL;
}

error create_game(int client_fd, char *game_name, unsigned int timer_length){
    error error_code;
    
    error_code = insert_game(game_name, timer_length);

    if(error_code != NO_ERROR){
        return error_code;
    }
    else{
        if (DEBUG) printf("Game created. Game name: %s", game_name);
        return join_game(client_fd, game_name);
    }
}

error join_game(int client_fd, char *game_name){
    error error_code;
    game *game = get_game(game_name);
    //Checks first whether a game with the given name exists
    if(game == NULL){
        if(DEBUG){
            printf("Game %s does not exist\n", game_name);
        }        
        error_code = GAME_DOESNT_EXIST;
    }
    else{
        pthread_rwlock_wrlock(&game->rwlock);

        //if the room is not full add the new player
        //else the room is full
        if(game->match_data->connected_players < MAX_PLAYERS){
            player* new_player = malloc(sizeof(player));
            new_player->socket_fd = client_fd;
            new_player->timer = game->match_data->timer_length;
            new_player->next_player = NULL;

            //If game is empty, player has to be added to head, else after the last player
            if(game->match_data->connected_players == 0){
                    game->match_data->players = new_player;
                    if(DEBUG) printf("Game is empty, joined player\n");
            }
            else{
                    player *curr= game->match_data->players;
                    while(curr->next_player != NULL) curr=curr->next_player;
                    curr->next_player = new_player;
                    if(DEBUG) printf("Game is not full, joined player\n");
            }

            //Increments counter of connected players, and signals game that a new player has joined
            game->match_data->connected_players++;
            error_code = NO_ERROR;
            pthread_cond_signal(&game->new_player_cond);    
        }//If the game is full and the player is not playing
        else{

            error_code = GAME_FULL;
            if(DEBUG) printf("Game is full, player cannot join\n");
        }

        pthread_rwlock_unlock(&game->rwlock);
    }
    return error_code;
}
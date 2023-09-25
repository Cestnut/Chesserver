#include "game_handling.h"
#include "server_connection.h"


games_struct *games;

void clean_game(game *game);

void init_games(){
    games = malloc(sizeof(games_struct));
    games->hashmap = NULL;
    pthread_rwlock_init(&games->rwlock, NULL);
}

error insert_game(char *name){
    error error_code;
    pthread_rwlock_wrlock(&games->rwlock);
    if(get_game(name) != NULL){
        error_code = GAME_NAME_TAKEN;
    }
    else{
        game* game_entry = malloc(sizeof(game));
        strcpy(game_entry->name, name);
        game_entry->log_file = create_game_log(name);
        match_data* data = malloc(sizeof(match_data));
        data->board = NULL;
        data->players = NULL;
        data->connected_players = 0;
        game_entry->match_data = data;

        pthread_rwlock_init(&game_entry->rwlock, NULL);
        pthread_mutex_init(&game_entry->new_player_mutex, NULL);
        pthread_cond_init(&game_entry->new_player_cond, NULL);

        pthread_t tid;
        pthread_create(&tid, NULL, run_game, (void *)game_entry);    
        game_entry->tid = tid;

        HASH_ADD_STR(games->hashmap, name, game_entry);
        error_code = NO_ERROR;
    }
    pthread_rwlock_unlock(&games->rwlock);
    return error_code;
}

game *get_game(char *name){
    game* game_entry;

    pthread_rwlock_rdlock(&games->rwlock);
    HASH_FIND_STR(games->hashmap, name, game_entry);
    pthread_rwlock_unlock(&games->rwlock);

    return game_entry;
}

void delete_game(char *name){
    pthread_rwlock_wrlock(&games->rwlock);
    game* game_entry = get_game(name);
    if(game_entry) {
        clean_game(game_entry);
        HASH_DEL(games->hashmap, game_entry);
    }
    pthread_rwlock_unlock(&games->rwlock);
}

void clean_game(game *game){
    match_data *data = game->match_data;

    player *curr_player = data->players, *tmp_player;
    board_struct *board = data->board;
    
    clean_board(board);
    
    for(int i=0; i<data->connected_players; i++){
        tmp_player = curr_player;
        curr_player = curr_player->next_player;
        free(tmp_player);
    }
    free(data);

    if(game->log_file != NULL) fclose(game->log_file);
    free(game);
}

void *run_game(void *args){
    game* current_game = (game*)args;
    player *current_player;
    //Iterates until game room is full
    while(current_game->match_data->connected_players < MAX_PLAYERS){
        if (DEBUG) printf("There are %d players out of %d in the game. Waiting...\n",   current_game->match_data->connected_players, 
                                                                                        MAX_PLAYERS);        
        pthread_cond_wait(&current_game->new_player_cond, &current_game->new_player_mutex);
    }

    current_player = current_game->match_data->players;
    if(DEBUG){
        printf("Game room %s is full\n", current_game->name);
    }

    //Connects the last player to the first, making the linked list circular
    while(current_player->next_player != NULL) current_player = current_player->next_player;
    current_player->next_player = current_game->match_data->players;     

    //Randomizes who's the first player
    if(rand()%2){
        current_player->player_color = WHITE;
        current_player->next_player->player_color = BLACK;
    }
    else{
        current_player = current_player->next_player;
        current_player->player_color = WHITE;
        current_player->next_player->player_color = BLACK;
    }

    //Communicate to each player their color
    for(int i=0; i<current_game->match_data->connected_players; i++){
        send(current_player->socket_fd, &current_player->player_color, sizeof(piece_color), 0);
        current_player = current_player->next_player;
    }

    char input_buffer[BUFFER_LEN];
    Position *positions = malloc(sizeof(Position)*2);
    int error;
    move_validation_result server_response_move;
    game_status status = RUNNING;

    board_struct *board = init_board();
    current_game->match_data->board = board;
    while(status == RUNNING){
        error=1;
        if (DEBUG) printf("Game running\n");
        while(error){
            //Receive move
            if(DEBUG) render_board(board, WHITE);
            if(DEBUG) printf("Waiting for move\n");
            memset(input_buffer, 0, sizeof(input_buffer));
            if(recvline(current_player->socket_fd, input_buffer, sizeof(input_buffer), 0)){

                if(DEBUG) printf("Received move: %s\n", input_buffer);
                //Validate and make move
                if(parse_move(positions, input_buffer) == NULL){
                    if (DEBUG) printf("Invalid input\n");
                    server_response_move = INVALID_MOVE;
                }
                else if(!is_move_valid(board, current_player->player_color, positions[0], positions[1])){
                    if (DEBUG) printf("Invalid move\n");
                    server_response_move = INVALID_MOVE;
                }
                else{
                    move_piece(board, positions[0], positions[1]);
                    log_move(current_game->log_file, input_buffer, current_player->player_color);
                    server_response_move = VALID_MOVE;
                    error = 0;
                }
                send(current_player->socket_fd, &server_response_move, sizeof(move_validation_result), 0);
            }
            else{
                if (DEBUG) printf("Player disconnected\nThe other player won.\n");
                error=0;
                status = TIMEOUT;
            }
        }

        //Changes player here, because the next messages will be sent to them.
        current_player = current_player->next_player;

        if(status != TIMEOUT){
            //Send move
            while(send(current_player->socket_fd, input_buffer, strlen(input_buffer), 0)==-1) if (DEBUG) printf("Error sending move: retrying");
            if(DEBUG) printf("Sent move to other player\n");

            //Sets new game status and sends it to each player
            if(!has_valid_moves(board, current_player->player_color)){
                if(is_in_check(board, current_player->player_color)){
                    if (DEBUG) printf("Checkmate\n");
                    status = CHECKMATE;
                }
                else{
                    if (DEBUG) printf("Stalemate\n");
                    status = STALEMATE;
                }
            }
            else status = RUNNING;
            if(DEBUG) printf("Calculated new status\n");

            for(int i=0; i<current_game->match_data->connected_players; i++){
                while (send(current_player->socket_fd, &status, sizeof(game_status), 0) == -1) if(DEBUG) printf("Error sending status: retrying");
                current_player = current_player->next_player;
            }

            if(DEBUG)printf("Sent new status to both players\n");
        }
        else{
            //TIMEOUT. Send no move and new status only to other player
            char no_move[] = "a1-a1";
            send(current_player->socket_fd, no_move, strlen(no_move), 0);
            send(current_player->socket_fd, &status, sizeof(game_status), 0);
        }
    }
        
    log_end(current_game->log_file, status, current_player->player_color);
    switch(status){
        case RUNNING:
            printf("ERROR, shouldn't have left game loop\n");
            break;
        case CHECKMATE:
            printf("Checkmate!\n");
            break;
        case STALEMATE:
            printf("Stalemate!\n");
            break;
        case TIMEOUT:
            printf("Timeout!\n");
            break;
        default:
            printf("Something weird happened. Status: %d\n", status);
            break;
    }

    worker_args **workers_args = malloc(sizeof(worker_args *) *current_game->match_data->connected_players);
    pthread_t tid;
    for(int i=0; i<current_game->match_data->connected_players; i++){
        workers_args[i] = malloc(sizeof(worker_args));
        workers_args[i]->client_fd = current_player->socket_fd;
        pthread_create(&tid, NULL, client_worker, (void *)workers_args[i]);
        current_player = current_player->next_player;
    }
    free(workers_args);
    clean_game(current_game);
    return NULL;
}

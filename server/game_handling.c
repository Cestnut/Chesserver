#include "game_handling.h"

games_struct *games;

void init_games(){
    games = malloc(sizeof(games_struct));
    games->hashmap = NULL;
    pthread_rwlock_init(&games->rwlock, NULL);
}

error insert_game(char *name, unsigned int timer_length){
    error error_code;
    pthread_rwlock_wrlock(&games->rwlock);
    if(get_game(name) != NULL){
        error_code = GAME_NAME_TAKEN;
    }
    else{
        game* game_entry = malloc(sizeof(game));
        strcpy(game_entry->name, name);
        match_data* data = malloc(sizeof(match_data));
        data->players = NULL;
        data->timer_length = timer_length;
        data->max_players = MAX_PLAYERS;
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
        free(game_entry->match_data);
        HASH_DEL(games->hashmap, game_entry);
        free(game_entry);
    }
    pthread_rwlock_unlock(&games->rwlock);
}


void *run_game(void *args){
    game* current_game = (game*)args;
    player *curr = current_game->match_data->players;

    //Iterates until game room is full
    while(current_game->match_data->connected_players < current_game->match_data->max_players){
        if (DEBUG) printf("There are %d players out of %d in the game. Waiting...\n",   current_game->match_data->connected_players, 
                                                                                        current_game->match_data->max_players);        
        pthread_cond_wait(&current_game->new_player_cond, &current_game->new_player_mutex);
    }
    
    if(DEBUG){
        printf("Game room %s is full\n", current_game->name);
    }

    //Connects the last player to the first, making the linked list circular
    while(curr->next_player != NULL) curr = curr->next_player;
    curr->next_player = current_game->match_data->players;     

    while(1){
        printf("%d\n", curr->socket_fd);
        curr = curr->next_player;
        sleep(2);
    }
}
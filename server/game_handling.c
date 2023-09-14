#include "game_handling.h"
#include <pthread.h>

void init_games(){
    games->hashmap = NULL;
    pthread_rwlock_init(&games->rwlock, NULL);
}

error insert_game(char *name, unsigned int timer_length){
    error error_code;
    pthread_rwlock_wrlock(&games->rwlock);
    if(get_game(name) == NULL){
        error_code = GAME_NAME_TAKEN;
    }
    else{
        game* game_entry = malloc(sizeof(game));
        strcpy(game_entry->name, name);
        match_data* data = malloc(sizeof(match_data));
        data->players = NULL;
        data->timer_length = timer_length;
        data->num_players = 2;
        game_entry->match_data = data;

        pthread_rwlock_init(&game_entry->rwlock, NULL);
        
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
    while(1){
        ;
    }
}
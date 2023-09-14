#define _GNU_SOURCE
#include <pthread.h>
#include "../common/common.h"
#include "../common/uthash.h"

typedef struct player{
    char *token;
    int timer;
    int socket_fd;
    struct player *next_player;
} player;


typedef struct match_data{
    player *players;
    unsigned int timer_length;

    //usati per controllare se la lobby e piena e aggiungere nuovi giocatori
    unsigned short int max_players;
    unsigned short int connected_players;
} match_data;

typedef struct game{
    char name[GAME_NAME_MAX_LENGTH];
    pthread_t tid;
    pthread_rwlock_t rwlock;
    match_data *match_data;
    UT_hash_handle hh;
} game;

typedef struct games_struct{
    game *hashmap;
    pthread_rwlock_t rwlock;
} games_struct;

void init_games();
error insert_game(char *name, unsigned int timer_length);
game *get_game(char *name);
void delete_game(char *name);

void *run_game(void *args);
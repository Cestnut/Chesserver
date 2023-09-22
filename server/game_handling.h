#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "../common/chess.h"
#include "../common/common.h"
#include "../common/uthash.h"
#include "../common/common_conf.h"
#include "server_conf.h"
typedef struct player{
    int socket_fd;
    piece_color player_color;
    struct player *next_player;
} player;


typedef struct match_data{
    player *players;
    board_struct *board;
    //usato per controllare se la lobby e piena e aggiungere nuovi giocatori
    unsigned short int connected_players;
} match_data;

typedef struct game{
    char name[GAME_NAME_MAX_LENGTH];
    pthread_t tid;
    pthread_rwlock_t rwlock;
    pthread_cond_t new_player_cond;
    pthread_mutex_t new_player_mutex;
    match_data *match_data;
    UT_hash_handle hh;
} game;

typedef struct games_struct{
    game *hashmap;
    pthread_rwlock_t rwlock;
} games_struct;

void init_games();
error insert_game(char *name);
game *get_game(char *name);
void delete_game(char *name);

void *run_game(void *args);
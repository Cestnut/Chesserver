#include "../common/common.h"
#include "../common/uthash.h"
#include <pthread.h>

typedef struct player{
    char *token;
    int timer;
    int socket_fd;
    struct player *next_player;
} player;

typedef struct spectator{
    int socket_fd;
} spectator;

typedef struct match_data{
    player *players;
    spectator *spectators;
    unsigned int timer_lenght;
    unsigned short int num_players; //usato per scorrere la lista dei giocatori
    unsigned int max_spectators;
} match_data;

typedef struct game{
    char name[GAME_NAME_MAX_LENGHT];
    pthread_t tid;
    pthread_rwlock_t rwlock;
    match_data *match_data;
    UT_hash_handle hh;
} game;

typedef struct games_struct{
    game *hashmap;
    pthread_rwlock_t rwlock;
} games_struct;

games_struct *games;

void init_games();
error insert_game(char *name, unsigned int timer_lenght, unsigned int max_spectators);
game *get_game(char *name);
void delete_game(char *name);

void *run_game(void *args);
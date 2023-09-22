#include "../common/common.h"
#include "../common/chess.h"


FILE *create_game_log(char *game_name);
void log_move(FILE *log_file, char *move, piece_color player_color);
void log_end(FILE *log_file, game_status status, piece_color winner);
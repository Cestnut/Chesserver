//header declaring all data structures for playing chess and methods for parsing, validating, rendering and making moves
#include <stdio.h>
#include <stdlib.h>
#define BOARD_SIZE 8
#define WHITE_CHARSET ".PRNBQK"
#define BLACK_CHARSET ".prnbqk"

typedef enum {NO_COLOR=0, WHITE, BLACK} piece_color;
typedef enum {NO_TYPE=0, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING} piece_type;

typedef struct Position{
    int row, col;
} Position;

typedef struct piece{
    piece_color color;
    piece_type type;
    int moved_flag;
} piece_struct;

//board coordinates are to be considered [col][row], to be coherent with how cells are addressed in chess.
typedef struct board{
    piece_struct *board[BOARD_SIZE][BOARD_SIZE];
} board_struct;

Position *parse_move(Position *points, char *move_string);

board_struct *init_board();
void render_board(board_struct *board, piece_color player_color);
void clean_board(board_struct *board);


//Returns the position of the king of the given point
Position get_king_position(board_struct *board, piece_color player_color);

void move_piece(board_struct *board, Position src_position, Position dst_position);
int is_move_safe(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_in_check(board_struct *board, piece_color color);

int has_valid_moves(board_struct *board, piece_color player_color);
int is_move_valid(board_struct *board, piece_color player_color, Position src_position, Position dst_position);

int is_pattern_valid(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
//Each of this functions check if pattern is respected and path is clear
int is_pattern_valid_pawn(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_pattern_valid_rook(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_pattern_valid_bishop(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_pattern_valid_knight(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_pattern_valid_queen(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
int is_pattern_valid_king(board_struct *board, piece_color player_color, Position src_position, Position dst_position);
//header declaring all data structures for playing chess and methods for parsing, validating, rendering and making moves
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#define BOARD_SIZE 8
#define WHITE_CHARSET ".PRNBQK"
#define BLACK_CHARSET ".prnbqk"

typedef enum {NO_COLOR=0, WHITE, BLACK} piece_color;
typedef enum {NO_TYPE=0, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING} piece_type;

typedef struct Point{
    int row, col;
} Point;

typedef struct piece{
    piece_color color;
    piece_type type;
    int moved_flag;
} piece_struct;

//board coordinates are to be considered [col][row], to be coherent with how cells are addressed in chess.
typedef struct board{
    piece_struct *board[BOARD_SIZE][BOARD_SIZE];
} board_struct;

board_struct *init_board();
void render_board(board_struct *board);

//Returns the position of the king of the given point
Point get_king_position(board_struct *board, piece_color player_color);

int is_move_valid(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);

int is_pattern_valid(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
//Each of this functions check if pattern is respected and path is clear
int is_pattern_valid_pawn(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int is_pattern_valid_rook(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int is_pattern_valid_bishop(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int is_pattern_valid_knight(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int is_pattern_valid_queen(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int is_pattern_valid_king(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);

int has_valid_moves_pawn(board_struct *board, piece_color player_color, int col_src, int row_src);
int has_valid_moves_rook(board_struct *board, piece_color player_color, int col_src, int row_src);
int has_valid_moves_bishop(board_struct *board, piece_color player_color, int col_src, int row_src);
int has_valid_moves_knight(board_struct *board, piece_color player_color, int col_src, int row_src);
int has_valid_moves_queen(board_struct *board, piece_color player_color, int col_src, int row_src);
int has_valid_moves_king(board_struct *board, piece_color player_color, int col_src, int row_src);


int is_in_check(board_struct *board, piece_color color);
int is_stalemate(board_struct *board, piece_color color);
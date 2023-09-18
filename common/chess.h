//header declaring all data structures for playing chess and methods for parsing, validating, rendering and making moves
#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#define BOARD_SIZE 8

typedef enum {NO_COLOR=0, WHITE, BLACK} piece_color;
typedef enum {NO_TYPE=0, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING} piece_type;

typedef struct piece{
    piece_color color;
    piece_type type;
    int moved_flag;
} piece_struct;

//board coordinates are to be considered [col][row], to be coherent with how cells are addressed in chess.
typedef struct board{
    piece_struct board[BOARD_SIZE][BOARD_SIZE];
} board_struct;

board_struct *init_board();
void render_board(board_struct *board);

int validate_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
//Each of this functions check if pattern is respected and path is clear
int validate_pawn_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int validate_rook_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int validate_bishop_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int validate_knight_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int validate_queen_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);
int validate_king_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst);

int is_in_check(board_struct *board, piece_color color);
int can_move(board_struct *board, piece_color color);
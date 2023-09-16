//header declaring all data structures for playing chess and methods for parsing, validating, rendering and making moves
#include <stdio.h>
#include <stdlib.h>
#define BOARD_SIZE 8

typedef enum {NO_COLOR=0, WHITE, BLACK} piece_color;
typedef enum {NO_TYPE=0, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING} piece_type;



typedef struct piece{
    piece_color color;
    piece_type type;
} piece_struct;

//board coordinates are to be considered [col][row], to be coherent with how cells are addressed in chess.
typedef struct board{
    piece_struct board[BOARD_SIZE][BOARD_SIZE];
} board_struct;

board_struct *init_board();
void render_board(board_struct *board);
#include "chess.h"

board_struct *init_board(){

    char clean_board[BOARD_SIZE][BOARD_SIZE][3] = { 
    { "wR", "wP", "..", "..", "..", "..", "bP", "bR" },
    { "wN", "wP", "..", "..","..", "..", "bP", "bN" },
    { "wB", "wP", "..", "..","..", "..", "bP", "bB" },
    { "wQ", "wP", "..", "..","..", "..", "bP", "bQ" },
    { "wK", "wP", "..", "..","..", "..", "bP", "bK" },
    { "wB", "wP", "..", "..","..", "..", "bP", "bB" },
    { "wN", "wP", "..", "..","..", "..", "bP", "bN" },
    { "wR", "wP", "..", "..", "..", "..", "bP", "bR" }
    };

    board_struct *board = malloc(sizeof(board_struct));
    for(int col=0; col<BOARD_SIZE; col++){
        for(int row=0; row<BOARD_SIZE; row++){
            piece_struct piece;

            char color_char, type_char;
            color_char = clean_board[col][row][0];
            type_char = clean_board[col][row][1];

            switch(color_char){
                case 'w':
                    piece.color = WHITE;
                    break;
                case 'b':
                    piece.color = BLACK;
                    break;
                case '.':
                    piece.color = NO_COLOR;
            }
            switch(type_char){
                case '.':
                    piece.type = NO_TYPE;
                    break;
                case 'P':
                    piece.type = PAWN;
                    break;
                case 'R':
                    piece.type = ROOK;
                    break;
                case 'B':
                    piece.type = BISHOP;
                    break;
                case 'N':
                    piece.type = KNIGHT;
                    break;
                case 'K':
                    piece.type = KING;
                    break;
                case 'Q':
                    piece.type = QUEEN;
                    break;
            }
            board->board[col][row] = piece;
        }
    }
}

void render_board(board_struct *board){
    char symbols[3][7] = {"......", ".PRNBQK", ".prnbqk"};
    for(int row=0; row<BOARD_SIZE; row++){
        printf("%d ", row+1);
        for(int col=0; col<BOARD_SIZE; col++){
            piece_struct piece = board->board[col][row];
            printf("%3c ", symbols[piece.color][piece.type]);
        }
        printf("\n");
    }

    char columns[] = "ABCDEFGH";
    printf("  ");
    for(int i=0; i<sizeof(columns); i++){
        printf("%3c ", columns[i]);
    }
    printf("\n\n");
}
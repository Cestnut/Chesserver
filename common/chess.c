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

int validate_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    piece_struct src_piece = board->board[col_src][row_src];
    piece_struct dst_piece = board->board[col_dst][row_dst];
    
    piece_color opponent_color;
    if(player_color == WHITE) opponent_color=BLACK;
    else if(player_color == BLACK) opponent_color=WHITE;

    //if destination is outside board
    if(col_dst >= BOARD_SIZE || row_dst >= BOARD_SIZE || col_dst < 0 || row_dst < 0) return FALSE;

    //if piece is not being moved
    if(col_src==col_dst && row_src == row_dst) return FALSE;

    //if moved piece is not owned by player
    if(src_piece.color != player_color) return FALSE;

    //if destination piece is owned by player
    if(dst_piece.color == player_color) return FALSE;


    switch(src_piece.type){
        case PAWN:
            validate_pawn_move(board, player_color, col_src, row_src, col_dst, row_dst);
            break;
        case ROOK:
            validate_rook_move(board, player_color, col_src, row_src, col_dst, row_dst);            
            break;
        case KNIGHT:
            validate_knight_move(board, player_color, col_src, row_src, col_dst, row_dst);
            break;
        case BISHOP:
            validate_bishop_move(board, player_color, col_src, row_src, col_dst, row_dst);
            break;
        case KING:
            validate_king_move(board, player_color, col_src, row_src, col_dst, row_dst);
            break;
        case QUEEN:
            validate_queen_move(board, player_color, col_src, row_src, col_dst, row_dst);
            break;
    }

}


int validate_pawn_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    piece_struct src_piece = board->board[col_src][row_src];
    piece_struct dst_piece = board->board[col_dst][row_dst];    
    
    piece_color opponent_color;
    if(player_color == WHITE) opponent_color=BLACK;
    else if(player_color == BLACK) opponent_color=WHITE;
    
    int step; //Used because white and black pawns go forward in opposing directions
    if(player_color == WHITE) step=1;
    else step = -1;

    //Checks if destination row is 1 row space forward and square is empty
    //else if destination row is 2 spaces forward and piece hasn't moved yet and square is empty
    if(row_src == row_dst+step){
        //If destination column is the same and there are no pieces there 
        //else if destination column is shifted of one and there is an opponent's piece there
        if(col_src == col_dst && dst_piece.type==NO_TYPE){
            return TRUE;
        }
        else if((col_src == col_dst + 1 || col_src == col_dst - 1) && dst_piece.color==opponent_color){
            return TRUE;
        }
    }
    else if(row_src == row_dst+(2*step) && src_piece.moved_flag == 0 && dst_piece.type==NO_TYPE){
        return TRUE;
    }
    else{
        return FALSE;
    } 
}
int validate_rook_move(board_struct *board, piece_color piece_color, int col_src, int row_src, int col_dst, int row_dst){
    int step; //Represents the direction of movement (-1 or +1)
    //If movement is straight (only one coordinate is changing)
    if(row_src==row_dst){
        //Here we control if the path to destination is clear
        if(col_src > col_dst) step = -1;
        else step = +1;

        //current position to analyze, since movement happens along column
        col_src += step;
        while(col_src != col_dst){
            //If in the current cell there is already a piece, return false
            if(board->board[col_src][row_dst].type != NO_TYPE) return FALSE;
            col_src += step;
        }
        return TRUE;
    }
    else if(col_src==col_dst){
        if(row_src > row_dst) step = -1;
        else step = +1;

        row_src += step;
        while(row_src != row_dst){
            if(board->board[col_dst][row_src].type != NO_TYPE) return FALSE;
            row_src += step;
        }
        return TRUE;

    }
    else{
        return FALSE;
    }    
}

int validate_knight_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    //Movement is valid if one of the two coordinates changes by 1 and the other by 2.
    //Knight is the only piece that doesn't care if the path is obstructed
    if(col_dst == col_src + 2 || col_dst == col_src - 2){
        if(row_dst == row_src +1 || row_dst == row_src -1) return TRUE;
        else return FALSE;
    }
    else if(col_dst == col_src +1 || col_dst == col_src -1){
        if(row_dst == row_src +2 || row_dst == row_src -2) return TRUE;
        else return FALSE;
    }
    else{
        return FALSE;
    }
}

int validate_bishop_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    //Movement is valid if change of row and column are of the same magnitude (absolute value of variation)
    int row_variation, col_variation, row_step, column_step;
    row_variation = row_dst - row_src;
    col_variation = col_dst - col_src;
    if(row_variation * row_variation == col_variation * col_variation){
        //Direction of movement on row
        if(row_dst > row_src) row_step = +1;
        else row_step = -1;
        //Direction of movement on column
        if(col_dst > col_src) column_step = +1;
        else column_step = -1;

        row_src += row_step;
        col_src += column_step;
        //We check step by step if the path is occupied
        while(row_src != row_dst){
            if(board->board[col_dst][row_dst].type != NO_TYPE) return FALSE;
            row_src += row_step;
            col_src += column_step;
        }
        return TRUE;
    }
    else return FALSE;

}


int validate_queen_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    return validate_rook_move(board, player_color, col_src, row_src, col_dst, row_dst) || validate_bishop_move(board, player_color, col_src, row_src, col_dst, row_dst);
}

int validate_king_move(board_struct *board, piece_color player_color, int col_src, int row_src, int col_dst, int row_dst){
    //Movement is valid only if row or column change by one
    int row_variation, col_variation;
    row_variation = row_dst - row_src;
    col_variation = col_dst - col_src;

    if(row_variation>=-1 && row_variation <=1 && col_variation>= -1 && col_variation <= 1) return TRUE;
    else return FALSE;
}

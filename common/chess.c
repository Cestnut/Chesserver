#include "chess.h"
#include "common.h"

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
            piece_struct *piece = (piece_struct*)malloc(sizeof(piece_struct));

            char color_char, type_char;
            color_char = clean_board[col][row][0];
            type_char = clean_board[col][row][1];

            switch(color_char){
                case 'w':
                    piece->color = WHITE;
                    break;
                case 'b':
                    piece->color = BLACK;
                    break;
                case '.':
                    piece->color = NO_COLOR;
            }
            switch(type_char){
                case '.':
                    piece->type = NO_TYPE;
                    break;
                case 'P':
                    piece->type = PAWN;
                    break;
                case 'R':
                    piece->type = ROOK;
                    break;
                case 'B':
                    piece->type = BISHOP;
                    break;
                case 'N':
                    piece->type = KNIGHT;
                    break;
                case 'K':
                    piece->type = KING;
                    break;
                case 'Q':
                    piece->type = QUEEN;
                    break;
            }
            piece->moved_flag = FALSE;
            board->board[col][row] = piece;

        }
    }
    return board;
}

void render_board(board_struct *board){
    piece_struct *piece;
    for(int row=0; row<BOARD_SIZE; row++){
        printf("%d ", row+1);
        for(int col=0; col<BOARD_SIZE; col++){
            piece = board->board[col][row];
            char symbol;
            if(piece->color == NO_COLOR) symbol = '.';
            else if(piece->color == WHITE) symbol = WHITE_CHARSET[piece->type];
            else if(piece->color == BLACK) symbol = BLACK_CHARSET[piece->type];

            printf("%3c ", symbol);
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

Position get_king_position(board_struct *board, piece_color player_color){
    piece_struct *piece;
    Position point;
    for(int row=0; row<BOARD_SIZE; row++){
        for(int col=0; col<BOARD_SIZE; col++){
            piece = board->board[col][row];
            if(piece->color == player_color && piece->type == KING){
                point.col = col;
                point.row = row;
                return point;
            }
        }
    }
}

int is_move_valid(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    piece_struct tmp_piece, *moving_piece;
    if(is_pattern_valid(board, player_color, src_position, dst_postion)){
        //Make the move but save the piece in destination, since we may have to rollback
        tmp_piece = *board->board[dst_postion.col][dst_postion.row];
        moving_piece = board->board[src_position.col][src_position.row];
        *board->board[dst_postion.col][dst_postion.row] = *moving_piece;
        moving_piece->color = NO_COLOR;
        moving_piece->type = NO_TYPE;
        
        if(is_in_check(board, player_color)){
            //Rollback the movement, because it would lead to being in check
            *moving_piece = *board->board[dst_postion.col][dst_postion.row];
            *board->board[dst_postion.col][dst_postion.row] = tmp_piece;
            return FALSE;
        }
        else{
            //If move happened, set moved flag to true
            board->board[dst_postion.col][dst_postion.row]->moved_flag = TRUE;
            //If moved piece was pawn, promote to queen in case it reached the end of the board
            int last_row;
            if(board->board[dst_postion.col][dst_postion.row]->type == PAWN){
                if(board->board[dst_postion.col][dst_postion.row]->color == WHITE) last_row = 7;
                else if(board->board[dst_postion.col][dst_postion.row]->color == BLACK) last_row = 0;
                if(dst_postion.row == last_row) board->board[dst_postion.col][dst_postion.row]->type = QUEEN;
            }

            return TRUE;
        }
    }
    else{
        if(DEBUG) printf("Invalid pattern\n");
        return FALSE;
    }
}

int is_in_check(board_struct *board, piece_color player_color){
    piece_color opponent_color;
    piece_struct *piece;
    Position king_position = get_king_position(board, player_color), src_position;
    if(player_color == WHITE) opponent_color=BLACK;
    else if(player_color == BLACK) opponent_color = WHITE;

    for(int row=0; row<BOARD_SIZE; row++){
        for(int col=0; col<BOARD_SIZE; col++){
            piece = board->board[col][row];
            if(piece->color == opponent_color){
                src_position.col = col;
                src_position.row = row;
                if(is_pattern_valid(board, opponent_color, src_position, king_position)){
                    if(DEBUG) printf("This would put the king in check from %d%d that could go to %d%d\n", col, row, king_position.col, king_position.row);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;

}

int is_pattern_valid(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    piece_struct *src_piece = board->board[src_position.col][src_position.row];
    piece_struct *dst_piece = board->board[dst_postion.col][dst_postion.row];
    
    piece_color opponent_color;
    if(player_color == WHITE) opponent_color=BLACK;
    else if(player_color == BLACK) opponent_color=WHITE;

    //if destination is outside board
    if(dst_postion.col >= BOARD_SIZE || dst_postion.row >= BOARD_SIZE || dst_postion.col < 0 || dst_postion.row < 0) return FALSE;

    //if piece is not being moved
    if(src_position.col==dst_postion.col && src_position.row == dst_postion.row) return FALSE;

    //if moved piece is not owned by player
    if(src_piece->color != player_color) return FALSE;

    //if destination piece is owned by player
    if(dst_piece->color == player_color) return FALSE;

    switch(src_piece->type){
        case PAWN:
            return is_pattern_valid_pawn(board, player_color, src_position, dst_postion);
            break;
        case ROOK:
            return is_pattern_valid_rook(board, player_color, src_position, dst_postion);            
            break;
        case KNIGHT:
            return is_pattern_valid_knight(board, player_color, src_position, dst_postion);
            break;
        case BISHOP:
            return is_pattern_valid_bishop(board, player_color, src_position, dst_postion);
            break;
        case KING:
            return is_pattern_valid_king(board, player_color, src_position, dst_postion);
            break;
        case QUEEN:
            return is_pattern_valid_queen(board, player_color, src_position, dst_postion);
            break;
    }

}


int is_pattern_valid_pawn(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    piece_struct *src_piece = board->board[src_position.col][src_position.row];
    piece_struct *dst_piece = board->board[dst_postion.col][dst_postion.row];    
    
    piece_color opponent_color;
    if(player_color == WHITE) opponent_color=BLACK;
    else if(player_color == BLACK) opponent_color=WHITE;
    
    int step; //Used because white and black pawns go forward in opposing directions
    if(player_color == WHITE) step=1;
    else step = -1;

    //Checks if destination row is 1 row space forward and square is empty
    //else if destination row is 2 spaces forward and piece hasn't moved yet and square is empty
    if(dst_postion.row == src_position.row+step){
        //If destination column is the same and there are no pieces there 
        //else if destination column is shifted of one and there is an opponent's piece there
        if(src_position.col == dst_postion.col && dst_piece->type==NO_TYPE){
            return TRUE;
        }
        else if((src_position.col == dst_postion.col + 1 || src_position.col == dst_postion.col - 1) && dst_piece->color==opponent_color){
            return TRUE;
        }
        else return FALSE;
    }
    else if(dst_postion.row == src_position.row+(2*step) && src_piece->moved_flag == 0 && dst_piece->type==NO_TYPE){
        return TRUE;
    }
    else{
        return FALSE;
    } 
}
int is_pattern_valid_rook(board_struct *board, piece_color piece_color, Position src_position, Position dst_postion){
    int step; //Represents the direction of movement (-1 or +1)
    //If movement is straight (only one coordinate is changing)
    if(src_position.row==dst_postion.row){
        //Here we control if the path to destination is clear
        if(src_position.col > dst_postion.col) step = -1;
        else step = +1;

        //current position to analyze, since movement happens along column
        src_position.col += step;
        while(src_position.col != dst_postion.col){
            //If in the current cell there is already a piece, return false
            if(board->board[src_position.col][src_position.row]->type != NO_TYPE) return FALSE;
            src_position.col += step;
        }
        return TRUE;
    }
    else if(src_position.col==dst_postion.col){
        if(src_position.row > dst_postion.row) step = -1;
        else step = +1;

        src_position.row += step;
        while(src_position.row != dst_postion.row){
            if(board->board[src_position.col][src_position.row]->type != NO_TYPE) return FALSE;
            src_position.row += step;
        }
        return TRUE;

    }
    else{
        return FALSE;
    }    
}

int is_pattern_valid_knight(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    //Movement is valid if one of the two coordinates changes by 1 and the other by 2.
    //Knight is the only piece that doesn't care if the path is obstructed
    if(dst_postion.col == src_position.col + 2 || dst_postion.col == src_position.col - 2){
        if(dst_postion.row == src_position.row +1 || dst_postion.row == src_position.row -1) return TRUE;
        else return FALSE;
    }
    else if(dst_postion.col == src_position.col +1 || dst_postion.col == src_position.col -1){
        if(dst_postion.row == src_position.row +2 || dst_postion.row == src_position.row -2) return TRUE;
        else return FALSE;
    }
    else{
        return FALSE;
    }
}

int is_pattern_valid_bishop(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    //Movement is valid if change of row and column are of the same magnitude (absolute value of variation)
    int row_variation, col_variation, row_step, column_step;
    row_variation = dst_postion.row - src_position.row;
    col_variation = dst_postion.col - src_position.col;
    if(row_variation * row_variation == col_variation * col_variation){
        //Direction of movement on row
        if(dst_postion.row > src_position.row) row_step = +1;
        else row_step = -1;
        //Direction of movement on column
        if(dst_postion.col > src_position.col) column_step = +1;
        else column_step = -1;

        src_position.row += row_step;
        src_position.col += column_step;
        //We check step by step if the path is occupied
        while(src_position.row != dst_postion.row){
            if(board->board[src_position.col][src_position.row]->type != NO_TYPE) return FALSE;
            src_position.row += row_step;
            src_position.col += column_step;
        }
        return TRUE;
    }
    else return FALSE;

}


int is_pattern_valid_queen(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    return is_pattern_valid_rook(board, player_color, src_position, dst_postion) || is_pattern_valid_bishop(board, player_color, src_position, dst_postion);
}

int is_pattern_valid_king(board_struct *board, piece_color player_color, Position src_position, Position dst_postion){
    //Movement is valid only if row or column change by one
    int row_variation, col_variation;
    row_variation = dst_postion.row - src_position.row;
    col_variation = dst_postion.col - src_position.col;

    if(row_variation>=-1 && row_variation <=1 && col_variation>= -1 && col_variation <= 1) return TRUE;
    else return FALSE;
}

#include "../common/chess.h"

int main(){
    board_struct *board = init_board();
    render_board(board);

    piece_struct tmp = board->board[5][5];
    board->board[5][5] = board->board[6][7];
    board->board[6][7] = tmp;

    render_board(board);
}
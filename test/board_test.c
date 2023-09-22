#include "../server/game_handling.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

player *init_players(){
    player *player1, *player2;
    player1 = malloc(sizeof(player));
    player2 = malloc(sizeof(player));

    player1->player_color = WHITE;
    player1->next_player = player2;

    player2->player_color = BLACK;
    player2->next_player = player1;

    return player1;
}

int main(){

    char input[10];
    player *curr_player = init_players(60);
    board_struct *board = init_board();
        
    Position *points = malloc(sizeof(Position)*2);
    while(TRUE){
        /*if(!has_valid_moves(board, curr_player->player_color)){
            if(is_in_check(board, curr_player->player_color)){
                printf("Checkmate\n");
            }
            else{
                printf("Stalemate\n");
            }
            break;
        }*/
        
        render_board(board, curr_player->player_color);        
        fgets(input, sizeof(input), stdin);
        if(parse_move(points, input) == NULL){
            printf("Invalid input\n");
        }
        else{
            printf("%d%d to %d%d\n", points[0].col, points[0].row, points[1].col, points[1].row);
            if(is_move_valid(board, curr_player->player_color, points[0], points[1])){
                move_piece(board, points[0], points[1]);
                curr_player = curr_player->next_player;
            }
            else{
                printf("Mossa invalida\n");
            }
        }
    }
}
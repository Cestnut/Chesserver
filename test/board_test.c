#include "../server/game_handling.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int parse(char *charset, int charset_len, char letter){
    for(int i=0; i<charset_len; i++){
        if(toupper(charset[i]) == toupper(letter)){
            return i;
        }
    }
    return -1;
}

int parse_column(char letter){
    return parse("ABCDEFGH", 8, letter);
}

int parse_row(char letter){
    return parse("12345678", 8, letter);
}

Position *parse_move(Position *points, char *move_string){
    int parsed_letter;
    char *token;

    //PARSE FIRST POINT
    token = strtok(move_string, "-");
    if(token==NULL) return NULL;

    parsed_letter = parse_column(token[0]);
    if(parsed_letter == -1) return NULL;
    points[0].col = parsed_letter;
    
    parsed_letter = parse_row(token[1]);
    if(parsed_letter == -1) return NULL;
    points[0].row = parsed_letter;
    
    //PARSE SECOND POINT
    token = strtok(NULL, "");
    if(token==NULL) return NULL;

    parsed_letter = parse_column(token[0]);
    if(parsed_letter == -1) return NULL;
    points[1].col = parsed_letter;
    
    parsed_letter = parse_row(token[1]);
    if(parsed_letter == -1) return NULL;
    points[1].row = parsed_letter;


    return points;
}

player *init_players(int timer){
    player *player1, *player2;
    player1 = malloc(sizeof(player));
    player2 = malloc(sizeof(player));

    player1->player_color = WHITE;
    player1->timer = timer;
    player1->next_player = player2;

    player2->player_color = BLACK;
    player2->timer = timer;
    player2->next_player = player1;

    return player1;
}

void print_timer(){
    int timer = 100;

}

int main(){

    char input[10];
    player *curr_player = init_players(60);
    board_struct *board = init_board();
    
    render_board(board);
    piece_color color = WHITE;
    
    Position *points = malloc(sizeof(Position)*2);
    while(TRUE){
        if(!has_valid_moves(board, curr_player->player_color)){
            if(is_in_check(board, curr_player->player_color)){
                printf("Checkmate\n");
            }
            else{
                printf("Stalemate\n");
            }
            break;
        }

        fgets(input, sizeof(input), stdin);
        if(parse_move(points, input) == NULL){
            printf("Invalid input\n");
        }
        else{
            printf("%d%d to %d%d\n", points[0].col, points[0].row, points[1].col, points[1].row);
            if(is_move_valid(board, curr_player->player_color, points[0], points[1])){
                move_piece(board, points[0], points[1]);
                render_board(board);
                curr_player = curr_player->next_player;
            }
            else{
                printf("Mossa invalida\n");
            }
        }
    }
}
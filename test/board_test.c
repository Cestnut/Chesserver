#include "../common/chess.h"
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

Point *parse_move(Point *points, char *move_string){
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

int main(){

    char input[10];


    board_struct *board = init_board();
    render_board(board);
    piece_color color = WHITE;
    Point *points = malloc(sizeof(Point)*2);
    while(TRUE){
        fgets(input, sizeof(input), stdin);
        if(parse_move(points, input) == NULL){
            printf("Invalid input\n");
        }
        else{
            printf("%d%d to %d%d\n", points[0].col, points[0].row, points[1].col, points[1].row);
            if(is_move_valid(board, color, points[0].col, points[0].row, points[1].col, points[1].row)){
                render_board(board);
            if(color == WHITE) color = BLACK;
            else color = WHITE;
            }
            else{
                printf("Mossa invalida\n");
            }
        }
        
    }
    free(points);
}
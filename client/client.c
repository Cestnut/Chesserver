#include "connection.h"
#include <string.h>
#include <unistd.h>
#include "../common/chess.h"

error create_game(int client_fd);
error join_game(int client_fd);
void game_menu();

int main(int argc, char **argv){

    int client_fd;
    char server_addr[32], *hostname; 
    int port, error_flag;

    //Parses hostname and port number from user input
    char *token;
    do{
        error_flag = 0;
        puts("Benvenuto a Chesserver! Scrivi l'indirizzo del server nella forma <HOSTNAME>:<PORTA>\n");
        fgets(server_addr, sizeof(server_addr), stdin);
        token = strtok(server_addr, ":");
        hostname = token;
        
        token = strtok(NULL, "");
        if(token == NULL){
            puts("Anche la porta deve essere fornita\n");
            error_flag = 1;
        }
        else{
            port = atoi(token);
        }

        for(int attempt=1; attempt<=MAX_CONNECTION_ATTEMPTS && error_flag == 0;attempt++){   
            client_fd = connect_to_server(hostname, port);        
            if(client_fd != -1){
                printf("Connected succesfully\n");
                break;
            }

            if(attempt == MAX_CONNECTION_ATTEMPTS){
                printf("Couldn't connect to server.\n");
                error_flag = 1;
                break;
            }

            printf("Could not connect to server. Retrying.\n");
            sleep(1);
        }

    }while(error_flag);
    
    char input[10];
    client_choice choice;
    char command_prompt[] = "Insert a command:\n"
                            "> 1: Create game\n"
                            "> 2: Join game\n"
                            "> 3: Exit\n";
    error error_code;
    
    //Manda input
    send_token(client_fd);
    while(1){
        printf("%s\n", command_prompt);    
        fgets(input, sizeof(input), stdin);
        flush_stdin();
        choice = (client_choice)atoi(input);
        send(client_fd, &choice, 1, 0);
        switch(choice){
            case CREATE_GAME:
                error_code = create_game(client_fd);
                if(error_code==NO_ERROR){
                    printf("Game created and joined\n");
                    game_menu(client_fd);
                }
                break;
                    
            case JOIN_GAME:
                error_code = join_game(client_fd);
                if(error_code==NO_ERROR){
                    printf("Game joined\n");
                    game_menu(client_fd);
                }
                break;

            case EXIT:
                puts("GAME EXITED\n");        
                close(client_fd);
                exit(0);
                break;
            }

        switch(error_code){
                case NO_ERROR:
                    break;
                case INVALID_INPUT:
                    puts("The input you sent is not valid");
                    break;
                case GAME_NAME_TAKEN:
                    puts("A game with that name already exists");
                    break;
                case GAME_DOESNT_EXIST:
                    puts("There isn't a game with such name");
                    break;
                case GAME_FULL:
                    puts("This game is full");
                    break;
            }
    }
    
    close(client_fd);

    return 0;
}

error create_game(int client_fd){
    char input_buffer[BUFFER_LEN];
    error error_code;
    puts("Insert game name, timer length in the form: <GAME_NAME>:<TIMER>");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recv(client_fd, &error_code, sizeof(error_code), 0);
    return error_code;
}

error join_game(int client_fd){
    char input_buffer[GAME_NAME_MAX_LENGTH];
    error error_code;
    puts("Insert game name: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recv(client_fd, &error_code, sizeof(error_code), 0);
    return error_code;
}

void game_menu(int client_fd){
    char input_buffer[GAME_NAME_MAX_LENGTH];
    char player_color_str[10];
    Position positions[2];
    piece_color player_color, curr_color=WHITE;
    game_status status = RUNNING;
    board_struct *board;
    move_validation_result server_response_move;

    printf("Waiting for game to begin\n");
    recv(client_fd, &player_color, sizeof(player_color), 0);
    if(player_color == WHITE){
        strncpy(player_color_str, "WHITE", sizeof(player_color_str));
    }
    else if(player_color == BLACK){
        strncpy(player_color_str, "BLACK", sizeof(player_color_str));
    }

    board = init_board();

    printf("The game has begun! You're %s\n", player_color_str);
    while(status == RUNNING){
        render_board(board, player_color);
        if(curr_color == player_color){
            printf("It's your turn!\n");
            while(TRUE){
                fgets(input_buffer, sizeof(input_buffer), stdin);
                if(parse_move(positions, input_buffer) == NULL){
                    printf("Invalid input\n");
                }
                else if(!is_move_valid(board, player_color, positions[0], positions[1])){
                    printf("Mossa invalida\n");
                }
                else{
                    //Sends the move and waits for feedback
                    send(client_fd, input_buffer, strlen(input_buffer), 0);
                    recv(client_fd, &server_response_move, sizeof(server_response_move), 0);
                    if(server_response_move == VALID_MOVE){
                        printf("%d%d to %d%d\n", positions[0].col, positions[0].row, positions[1].col, positions[1].row);
                        move_piece(board, positions[0], positions[1]);
                        break;
                    }
                }
            }   
        }
        else{
            printf("It's your opponent's turn!\n");
            recv(client_fd, &input_buffer, sizeof(input_buffer), 0);
            parse_move(positions, input_buffer);
            move_piece(board, positions[0], positions[1]);
        }
        

        //Change current color
        if(curr_color == WHITE) curr_color = BLACK;
        else if(curr_color == BLACK) curr_color = WHITE;
        
        recv(client_fd, &status, sizeof(status), 0);

    }
    switch(status){
        case RUNNING:
            break;
        case CHECKMATE:
            break;
        case STALEMATE:
            break;
        case TIMEOUT:
            break;
        default:
            break;
    }
}
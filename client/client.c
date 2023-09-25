#include "connection.h"
#include <string.h>
#include <unistd.h>
#include "../common/chess.h"
#define DEBUG TRUE

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
    while(1){
        printf("%s\n", command_prompt);    
        fgets(input, sizeof(input), stdin);
        //flush_stdin();
        choice = (client_choice)atoi(input);
        send(client_fd, &choice, 1, 0);
        switch(choice){
            case CREATE_GAME:
                error_code = create_game(client_fd);
                if(error_code==NO_ERROR){
                    printf("Game created and joined\n");
                    game_menu(client_fd);
                    printf("Game ended\n");
                }
                break;
                    
            case JOIN_GAME:
                error_code = join_game(client_fd);
                if(error_code==NO_ERROR){
                    printf("Game joined\n");
                    game_menu(client_fd);
                    printf("Game ended\n");
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
                default:
                    printf("Something went wrong. Error: %d", error_code);
            }
    }
    
    close(client_fd);

    return 0;
}

error create_game(int client_fd){
    char input_buffer[BUFFER_LEN];
    error error_code;
    puts("Insert game name: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    //flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recvline(client_fd, &error_code, sizeof(error_code), 0);
    return error_code;
}

error join_game(int client_fd){
    char input_buffer[BUFFER_LEN];
    error error_code;
    puts("Insert game name: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    //flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recvline(client_fd, &error_code, sizeof(error_code), 0);
    return error_code;
}

void game_menu(int client_fd){
    char input_buffer[BUFFER_LEN];
    char player_color_str[10];
    Position *positions = malloc(sizeof(Position)*2);
    piece_color player_color, curr_color=WHITE;
    game_status status = RUNNING;
    board_struct *board;
    move_validation_result server_response_move;
    int error;

    printf("Waiting for game to begin\n");
    if(recvline(client_fd, &player_color, sizeof(player_color), 0) == 0){
        printf("Server closed the connection\nExiting...");
        exit(0);
    }

    if(player_color == WHITE){
        strncpy(player_color_str, "WHITE", sizeof(player_color_str));
    }
    else if(player_color == BLACK){
        strncpy(player_color_str, "BLACK", sizeof(player_color_str));
    }

    board = init_board();

    printf("The game has begun! You're %s\n", player_color_str);
    while(status == RUNNING){
        printf("Game running\n");
        memset(input_buffer, 0, sizeof(input_buffer));
        error = 1;
        render_board(board, player_color);
        if(curr_color == player_color){
            memset(input_buffer, 0, sizeof(input_buffer));
            fflush(stdin);
            printf("It's your turn!\n");
            while(error){
                printf("Your move: \n");
                memset(input_buffer, 0, sizeof(input_buffer));
                fgets(input_buffer, sizeof(input_buffer), stdin);
                //flush_stdin();
                //Sends the move and waits for feedback
                strip_newlines(input_buffer, strlen(input_buffer));
                if(parse_move(positions, input_buffer) == NULL){
                    error = 1;
                    if (DEBUG) printf("Invalid pattern\n");
                }
                else if(!is_move_valid(board, player_color, positions[0], positions[1])){
                    error = 1;
                    if (DEBUG) printf("Invalid move\n");
                }
                else if(send(client_fd, input_buffer, strlen(input_buffer), 0) == -1){
                    if(DEBUG) printf("Error sending move. Errno: %d", errno);
                    error = 1;
                }
                else{
                    if (DEBUG) printf("Move sent: %s of length %ld\n", input_buffer, strlen(input_buffer));
                    if(recvline(client_fd, &server_response_move, sizeof(move_validation_result), 0)== 0){
                        printf("Server closed the connection\nExiting...");
                        exit(0);
                    }

                    if(server_response_move == INVALID_MOVE) printf("Invalid move\n");
                    else{
                        parse_move(positions, input_buffer);
                        if (DEBUG) printf("%d%d to %d%d\n", positions[0].col, positions[0].row, positions[1].col, positions[1].row);
                        move_piece(board, positions[0], positions[1]);
                        error = 0;
                    }
                }
            }
        }   
        else{
            printf("It's your opponent's turn!\n");
            if(recvline(client_fd, input_buffer, sizeof(input_buffer), 0)== 0){
                printf("Server closed the connection\nExiting...");
                exit(0);
            }
            if (DEBUG) printf("Received move %s\n", input_buffer);
            parse_move(positions, input_buffer);
            move_piece(board, positions[0], positions[1]);
        }
        

        //Change current color
        if(curr_color == WHITE) curr_color = BLACK;
        else if(curr_color == BLACK) curr_color = WHITE;
        
        if(DEBUG) printf("Waiting for new status\n");
        if(recvline(client_fd, &status, sizeof(game_status), 0)== 0){
            printf("Server closed the connection\nExiting...\n");
            exit(0);
        }
        if(DEBUG) printf("New status received: %d\n", status);
    }

    free(positions);
    clean_board(board);

    switch(status){
        case RUNNING:
            printf("ERROR, shouldn't have left game loop\n");
            break;
        case CHECKMATE:
            printf("Checkmate! ");
            if(curr_color != player_color) printf("You won :D!!\n");
            else printf("You lost :(\n");
            break;
        case STALEMATE:
            printf("Stalemate!\n");
            break;
        case TIMEOUT:
            printf("The other player disconnected, you won!\n");
            break;
        default:
            printf("Something weird happened. Status: %d\n", status);
            break;
    }
}
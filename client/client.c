#include "connection.h"
#include <string.h>
#include <unistd.h>

error create_game(int client_fd);
error join_game(int client_fd);

int main(int argc, char **argv){

    int client_fd;
    char server_addr[32], *hostname; 
    int port, error_flag;

    //Parses hostname and port number from user input
    do{
        error_flag = 0;
        puts("Benvenuto a Chesserver! Scrivi l'indirizzo del server nella forma <HOSTNAME>:<PORTA>\n");
        fgets(server_addr, sizeof(server_addr), stdin);
        char *token;
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
    }while(error_flag);
    
    int max_attempts = 3, attempt = 1;
    while(1){   
        client_fd = connect_to_server(hostname, port);        
        if(client_fd != -1){
            printf("Connected succesfully\n");
            break;
        }

        if(attempt == max_attempts){
            printf("Couldn't connect to server. Exiting process.");
            exit(0);
        }

        printf("Could not connect to server. Retrying.\n");
        attempt++;
        sleep(1);
    }
        
    char input[10];
    client_choice choice;
    char command_prompt[] = "Insert a command:\n"
                            "> 1: Create game\n"
                            "> 2: Join game\n"
                            "> 3: Exit\n";
    error error_code;
    
    //Manda input
    while(1){
        send_token(client_fd);
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
                }
                break;
                    
            case JOIN_GAME:
                error_code = join_game(client_fd);
                if(error_code==NO_ERROR){
                    printf("Game joined\n");
                }
                break;

            case EXIT:
                puts("GAME EXITED\n");        
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
    char input_buffer[64];
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
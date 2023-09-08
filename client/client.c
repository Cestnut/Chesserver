#include "connection.h"
#include <string.h>
#include "../common/common.h"

error create_game(int client_fd);
error join_game(int client_fd);
error spectate_game(int client_fd);

int main(int argc, char **argv){

    int client_fd;
    char server_addr[32], *hostname; 
    int port, error_flag;

    char output_buffer[BUFFER_LEN], input_buffer[BUFFER_LEN];
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
    
    memset(output_buffer, 0, BUFFER_LEN);
    client_fd = connect_to_server(hostname, port);
    
    char input[10];
    client_choice choice;
    char command_prompt[] = "Insert a command:\n"
                            "> 1: Create game\n"
                            "> 2: Join game\n"
                            "> 3: Spectate game\n"
                            "> 4: Exit\n";
    error error_code;
    while(1){
        //Manda input\n
        printf("%s\n", command_prompt);    
        fgets(input, sizeof(input), stdin);
        flush_stdin();
        choice = (client_choice)atoi(input);
        send(client_fd, &choice, 1, 0);
        switch(choice){
            case CREATE_GAME:
                error_code = create_game(client_fd);
                break;
                    
            case JOIN_GAME:
                error_code = join_game(client_fd);
                break;

            case SPECTATE_GAME:
                error_code = spectate_game(client_fd);
                break;

            case EXIT:
                puts("GAME EXITED\n");        
                exit(0);
                break;
            }

        switch(error_code){
                case NO_ERROR:
                    break;
                case GAME_NAME_TAKEN:
                    puts("A game with that name already exists");
                case GAME_DOESNT_EXIST:
                    puts("There isn't a game with such name");
                case GAME_FULL:
                    puts("This game is full");
            }
    }

    fgets(output_buffer, sizeof(output_buffer), stdin);
    output_buffer[strlen(output_buffer) - 1] = '\0';
    printf("%lu\n", strlen(output_buffer));
    if(send(client_fd, output_buffer, strlen(output_buffer), 0) == -1){
        printf("Error sending message: errno %d\n", errno);
        exit(0);
    }
    puts("Message sent\n");

    if(recv(client_fd, input_buffer, sizeof(input_buffer), 0) == -1){
        printf("Error receiving message: errno %d\n", errno);
        exit(0);
    }
    puts("Message received\n");

    printf("client message: %s\n", output_buffer);
    printf("server message: %s\n", input_buffer);

    close(client_fd);


    return 0;
}

error create_game(int client_fd){
    char input_buffer[64];
    error error_code;
    puts("Insert game name, timer lenght in the form: <GAME_NAME>:<TIMER>");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recv(client_fd, &error_code, sizeof(client_choice), 0);
    return error_code;
}
error join_game(int client_fd){
    char input_buffer[GAME_NAME_MAX_LENGHT];
    error error_code;
    puts("Insert game name: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recv(client_fd, &error_code, sizeof(client_choice), 0);
    return error_code;
}

//This function is the same as join_game. It exists because it may behave differently in the future.
error spectate_game(int client_fd){
    char input_buffer[GAME_NAME_MAX_LENGHT];
    error error_code;
    puts("Insert game name: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    flush_stdin();
    send(client_fd, input_buffer, strlen(input_buffer), 0);

    recv(client_fd, &error_code, sizeof(client_choice), 0);
    return error_code;
}

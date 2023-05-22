#include "connection.h"
#include <string.h>
#include "../common/common.h"

int main(int argc, char **argv){

    int socket_fd;
    char server_addr[32], *hostname; 
    int port, error_flag;

    char output_buffer[BUFFER_LEN], input_buffer[BUFFER_LEN];
    do{
        error_flag = 0;
        printf("Benvenuto a Chesserver! Scrivi l'indirizzo del server nella forma <HOSTNAME>:<PORTA>\n");
        fgets(server_addr, sizeof(server_addr), stdin);
        char *token;
        token = strtok(server_addr, ":");
        hostname = token;
        
        token = strtok(NULL, "");
        if(token == NULL){
            printf("Anche la porta deve essere fornita\n");
            error_flag = 1;
        }
        else{
            port = atoi(token);
        }
    }while(error_flag);
    

    memset(output_buffer, 0, BUFFER_LEN);
    socket_fd = connect_to_server(hostname, port);
    
    char input;
    client_choice choice;
    char command_prompt[] = "Insert a command:\n"
                            "> 1: Create game\n"
                            "> 2: Join game\n"
                            "> 3: Spectate game\n"
                            "> 4: Exit\n";
    while(1){
        //Manda input\n
        printf("%s\n", command_prompt);    
        input = (char)fgetc(stdin);
        send(socket_fd, &input, 1, 0);
        choice = atoi(&input);
        switch(choice){
            case CREATE_GAME:
                printf("GAME CREATED\n");
                break;
                    
            case JOIN_GAME:
                printf("GAME JOINED\n");
                break;

            case SPECTATE_GAME:
                printf("SPECTATING GAME\n");
                break;

            case EXIT:
                printf("GAME EXITED\n");        
                exit(0);
                break;
            }        
        flush_stdin();
    }

    fgets(output_buffer, sizeof(output_buffer), stdin);
    output_buffer[strlen(output_buffer) - 1] = '\0';
    printf("%lu\n", strlen(output_buffer));
    if(send(socket_fd, output_buffer, strlen(output_buffer), 0) == -1){
        printf("Error sending message: errno %d\n", errno);
        exit(0);
    }
    printf("Message sent\n");

    if(recv(socket_fd, input_buffer, sizeof(input_buffer), 0) == -1){
        printf("Error receiving message: errno %d\n", errno);
        exit(0);
    }
    printf("Message received\n");

    printf("client message: %s\n", output_buffer);
    printf("server message: %s\n", input_buffer);

    close(socket_fd);


    return 0;
}

/*
1: create socket
2: set port and IP
3: bind to the set port and ip
4: listen for clients
5: accept incoming connection
6: Receive client's message and send response
7: close socket


sockaddr_in.sin_family
sockaddr_in.sin_port
sockaddr_in.sin_addr.s_addr
listen
accept
recv
send
*/
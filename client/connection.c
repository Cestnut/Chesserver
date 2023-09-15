#include "connection.h"

//returns a fd on success, -1 on failure
int connect_to_server(char* hostname, int port){
    int socket_fd;
    struct sockaddr_in server_addr;
    char input_buffer[BUFFER_LEN], output_buffer[BUFFER_LEN];
    
    memset(input_buffer, 0, BUFFER_LEN);
    memset(output_buffer, 0, BUFFER_LEN);


    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        printf("Error opening socket: errno %d\n", errno);
        exit(0);
    }
    printf("Socket created\n");


    struct hostent *server;
    server = gethostbyname(hostname);
    if(server == NULL){
        printf("Error resolving hostname\n");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)server->h_addr);


    if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        return -1;
    }

    return socket_fd;
}

void send_token(int client_fd){
    char filename[] = "token.txt";
    //Checks if file exists. If it doesn't it's created.
    FILE *file = fopen(filename, "r");
    if(!file){
        file = fopen(filename, "w");
        fclose(file);
    }
    file = fopen(filename, "r");
    char token[65];
    
    //Checks if file is empty. If it is, the new token is saved
    fgets(token, sizeof(token), file);
    send(client_fd, token, strlen(token), 0);

    printf("%s\n", token);
    printf("%lu\n", strlen(token));
    if (strlen(token)!=TOKEN_LENGTH){
        fclose(file);
        file = fopen(filename, "w");
        recv(client_fd, token, sizeof(token), 0);

        fprintf(file, "%s", token);
    }

    printf("%s\n", token);
    fclose(file);
}
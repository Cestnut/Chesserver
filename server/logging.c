#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int directory_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

FILE *create_game_log(char *name) {
    
    // Create directory path "/var/log/ELO/games"
    char directory_path[256];
    sprintf(directory_path, "/var/log/ELO/games");
    if(mkdir(directory_path, 0755)) perror(mkdir); // Create directory with permissions 0755 (rwxr-xr-x)

    // Get current time
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_str[20];
    sprintf(time_str, "%02d_%02d_%04d:%02d-%02d",
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
            tm_info->tm_hour, tm_info->tm_min);

    // Create file name as combination of 'name' and current time
    char file_name[300];
    sprintf(file_name, "%s/%s_%s.txt", directory_path, name, time_str);

    // Open the file
    FILE *file = fopen(file_name, "w");
    if(file != NULL) printf("Succesfully created file %s\n", file_name);
    else printf("Error creating file %s\n", file_name);

    return file;
}

void log_move(FILE *log_file, char *move, piece_color player_color){
    if(log_file != NULL){
        char new_move[10];
        if(player_color == WHITE) sprintf(new_move, "%s", move);
        else sprintf(new_move, " %s\n", move);
    
        fprintf(log_file,"%s",new_move);
    }
}

void log_end(FILE *log_file, game_status status, piece_color winner){
    if(log_file != NULL){
        char winner_str[10];
        if(status == CHECKMATE){
            if(winner == WHITE) sprintf(winner_str, "%s", "WHITE");
            else if(winner == BLACK) sprintf(winner_str, "%s", "BLACK");
            fprintf(log_file,"CHECKMATE %s",winner_str);
            }
        else if(status == STALEMATE) fprintf(log_file, "%s", "STALEMATE");
    }
}

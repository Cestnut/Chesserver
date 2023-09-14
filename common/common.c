#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void flush_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Discard characters until newline or end of file
    }
}

int is_number(char *string){
    for (char *chr = string ; *chr ; chr++){
        if (!isdigit(*chr))
        return 0;
    }
    return 1;
}

char *random_string(int bytes_length){
    
    char *output_string = malloc(bytes_length * sizeof(char));

    for(int i = 0; i < bytes_length; i++){
        //Prints to every character and hex value between 0x0 and 0xF
        sprintf(&output_string[i], "%hhX", rand()%16);
    }

    return output_string;
}
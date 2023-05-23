#include <stdio.h>
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
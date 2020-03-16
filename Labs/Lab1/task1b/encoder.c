#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32

int main(int argc, char **argv){
    int i;
    char c;
    FILE *errOutput = stderr;
    int isInDebug = 0;
    for(i = 1; i < argc; i++){
        if (strcmp(argv[i], "-D") == 0){       
            isInDebug = 1;
        }
    }
    if(isInDebug){
        for(i = 1; i < argc; i++){
            fprintf(errOutput, "%s\n", argv[i]);
        }
        while(!feof(stdin)){
            c = getc(stdin);
            if (c >= LOWER_CASE_LOW_BOUND && c <= LOWER_CASE_HIGH_BOUND){
                fprintf(errOutput, "%d\t%d\n", c, c-LOWER_CASE_TO_HIGHER_CASE_DIFF);
                printf("%c", c-LOWER_CASE_TO_HIGHER_CASE_DIFF);
            }
            else if(c == '\n'){   
                fprintf(errOutput, "\n");
                printf("%s", "\n");
            }
            else if(c == -1){
                printf("%c", c);
            }
            else{
                fprintf(errOutput, "%d\t%d\n", c, c);
                printf("%c", c);
            }
        }   
    }
    else{
        while(!feof(stdin)){
            c = getc(stdin);
            if (c >= LOWER_CASE_LOW_BOUND && c <= LOWER_CASE_HIGH_BOUND){
                printf("%c", c-LOWER_CASE_TO_HIGHER_CASE_DIFF);
            }
            else{
                printf("%c", c);
            }
        }
    }
    printf("%s", "\n");
}
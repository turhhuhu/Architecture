#include <stdio.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32

int main(int argc, char **argv){
    char c;
    while(!feof(stdin)){
        c = getc(stdin);
        if (c >= LOWER_CASE_LOW_BOUND && c <= LOWER_CASE_HIGH_BOUND){
            printf("%c", c-LOWER_CASE_TO_HIGHER_CASE_DIFF);
        }
        else{
            printf("%c", c);
        }
    }
    printf("%s", "\n");
}
#include <stdio.h>
#define MAX_LIMIT 20

int main(int argc, char **argv){
    for (int i = 1; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("%s", "\n");
}
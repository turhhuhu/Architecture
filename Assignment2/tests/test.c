#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE* output = fopen("output.txt", "r");
    FILE* expected_output = fopen("expected_output.txt", "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * line2 = NULL;
    size_t len2 = 0;
    ssize_t read2;
    char success = 1;

    if (output == NULL || expected_output == NULL){
        exit(1);
    }

    while ((read = getline(&line, &len, output)) != -1) {
        if ((read2 = getline(&line2, &len2, expected_output)) != -1){
            if (read != read2 || strcmp(line, line2) != 0)
            {
                printf("test failed.\nexpected: %sbut got: %s", line2, line);
                success = 0;
            }
            
        }
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
    }
    if (success){
        printf("test has passed successfully\n");
    }

    fclose(output);
    if (line)
        free(line);
    exit(0);
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ESTIMATED_MAX_MENU_SIZE 1024
#define MAX_INPUT_SIZE 1024
#define MAX_FILE_NAME 100
#define MAX_FILE_SIZE 10000

int isInDebugMode = 0;
int displayFlag = 0;
typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

struct fun_desc
{
    char *name;
    void (*fun)(state *s);
};

void debugStr(char *msg)
{
    if (isInDebugMode)
    {
        printf("%s", msg);
    }
}

void debugInt(int msg)
{
    if (isInDebugMode)
    {
        printf("%d", msg);
    }
}

void debugHex(int msg)
{
    if (isInDebugMode)
    {
        printf("%X", msg);
    }
}

void quit(state *s)
{
    debugStr("quitting\n");
    exit(0);
}

void setUnitSize(state *s)
{
    char inputAsString[MAX_INPUT_SIZE];
    int input = 0;
    printf("enter unit size: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%d", &input);
    if(input >= 1 && input <= 4 && input != 3)
    {
        s -> unit_size = input;
        debugStr("Debug: set size to ");
        debugInt(input);
        debugStr("\n");
    }
}

void setFileName(state* s)
{
    printf("enter file name: ");
    fgets(s->file_name, MAX_FILE_NAME, stdin);
    sscanf(s->file_name, "%s", s->file_name);
    debugStr("Debug: file name set to ");
    debugStr(s->file_name);
    debugStr("\n");
}

void toggleDebug(state *s)
{
    if (isInDebugMode)
    {
        isInDebugMode = 0;
        printf("Debug flag now off\n");
        return;
    }
    isInDebugMode = 1;
    printf("Debug flag now on\n");
}

void toggleDisplay(state *s)
{
    if(displayFlag)
    {
        displayFlag = 0;
        printf("Display flag now off, decimal representation");
        return;
    }
    displayFlag = 1;
    printf("Display flag now on, hexadecimal representation");
}

void loadIntoMem(state* s)
{
    if(s -> file_name[0] == '\0')
    {
       printf("No file name\n");
       return; 
    }
    FILE* fileOpen = fopen(s -> file_name, "r");
    if(fileOpen == NULL)
    {
        printf("An error occurred while opening file\n");
        return;
    }
    char inputAsString[MAX_INPUT_SIZE];
    int location = 0;
    int length = 0;
    printf("Enter location: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &location);
    printf("Enter length: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%d", &length);
    debugStr(s -> file_name);
    debugStr("\n");
    debugHex(location);
    debugStr("\n");
    debugInt(length);
    debugStr("\n");
    fseek(fileOpen, location, SEEK_SET);
    fread(s -> mem_buf, s->unit_size, length, fileOpen);
    printf("Loaded %d units into memory", length);
}

int getBoundary(struct fun_desc *menu)
{
    int i;
    for (i = 0; i < ESTIMATED_MAX_MENU_SIZE; i++)
    {
        if (menu[i].name == NULL || menu[i].fun == NULL)
        {
            return i;
        }
    }
    return 0; // for the compiler warning
}

void print_menu(struct fun_desc *menu)
{
    int i;
    for (i = 0; i < ESTIMATED_MAX_MENU_SIZE; i++)
    {
        if (menu[i].name != NULL && menu[i].fun != NULL)
        {
            printf("%d-%s\n", i, menu[i].name);
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int boundary, input = -1;
    char inputAsString[MAX_INPUT_SIZE];
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebug}, {"Set File Name", setFileName}, {"Set Unit Size", setUnitSize},
            {"Load Into Memory", loadIntoMem}, {"Toggle Display Mode", toggleDisplay}, {"Quit", quit}, {NULL, NULL}};
    boundary = getBoundary(menu);
    state s;
    s.unit_size = 1;
    while (1)
    {
        debugInt(s.unit_size);
        debugStr("\n");
        debugStr(s.file_name);
        debugStr("\n");
        debugInt(s.mem_count);
        debugStr("\n");
        printf("Choose action: \n");
        print_menu(menu);
        fgets(inputAsString, MAX_INPUT_SIZE, stdin);
        sscanf(inputAsString, "%d", &input);
        if (input < 0 && input > boundary)
        {
            printf("Not within bounds\n");
        }
        menu[input].fun(&s);
        printf("\n");
    }

    return 0;
}
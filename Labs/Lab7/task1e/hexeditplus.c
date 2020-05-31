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
    if (input >= 1 && input <= 4 && input != 3)
    {
        s->unit_size = input;
        debugStr("Debug: set size to ");
        debugInt(input);
        debugStr("\n");
    }
}

void setFileName(state *s)
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
    if (displayFlag)
    {
        displayFlag = 0;
        printf("Display flag now off, decimal representation");
        return;
    }
    displayFlag = 1;
    printf("Display flag now on, hexadecimal representation");
}

void saveIntoFile(state *s)
{
    char inputAsString[MAX_INPUT_SIZE];
    unsigned int addr;
    int offset;
    int length;
    unsigned char *start;
    int fileSize;
    FILE *fileOpen = NULL;
    printf("Enter source-address: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &addr);
    printf("Enter target-location: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &offset);
    printf("Enter length: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%d", &length);
    debugHex(addr);
    debugStr("\n");
    debugHex(offset);
    debugStr("\n");
    debugInt(length);
    debugStr("\n");

    fileOpen = fopen(s->file_name, "r+");
    if (fileOpen == NULL)
    {
        printf("An error occurred while opening file\n");
        return;
    }
    fseek(fileOpen, 0, SEEK_END);
    fileSize = ftell(fileOpen);
    fseek(fileOpen, 0, SEEK_SET);
    if(offset > fileSize)
    {
        printf("offset is greater than file size");
        return;
    }
    fseek(fileOpen, offset, SEEK_SET);
    if(addr == 0)
    {
        start = s -> mem_buf;
        start = start + addr;
    }
    else
    {
        start = (unsigned char*)addr;
    }
    fwrite(start, s -> unit_size, length, fileOpen);
    fclose(fileOpen);
}

void memoryModify(state *s)
{
    char inputAsString[MAX_INPUT_SIZE];
    unsigned int location;
    int val;
    unsigned char *start;
    printf("Enter location: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &location);
    printf("Enter val: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &val);
    debugHex(location);
    debugStr("\n");
    debugHex(val);
    debugStr("\n");
    if(location > MAX_FILE_SIZE)
    {
        printf("offset is greater than file size");
        return;
    }
    start = s -> mem_buf;
    start = start + location;
    *((int*)(start)) = val;
}

void memoryDisplay(state *s)
{
    char inputAsString[MAX_INPUT_SIZE];
    int u = 0;
    unsigned int addr;
    unsigned char* end;
    unsigned char* start;
    printf("Enter u: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%d", &u);
    printf("Enter addr: ");
    fgets(inputAsString, MAX_INPUT_SIZE, stdin);
    sscanf(inputAsString, "%X", &addr);
    debugInt(u);
    debugStr("\n");
    debugHex(addr);
    debugStr("\n");
    if(addr == 0)
    {
        start = s -> mem_buf;
        start = start + addr;
    }
    else
    {
        start = (unsigned char*)addr;
    }
    end = start + s -> unit_size * u;
    while(start < end)
    {
        int var = *((int*)(start));
        if(displayFlag)
        {
            printf("%X\n", var);
        }
        else
        {
            printf("%d\n", var);
        }
        start += s -> unit_size;
    }
}

void loadIntoMem(state *s)
{
    if (s->file_name[0] == '\0')
    {
        printf("No file name\n");
        return;
    }
    FILE *fileOpen = fopen(s->file_name, "r + b");
    if (fileOpen == NULL)
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
    debugStr(s->file_name);
    debugStr("\n");
    debugHex(location);
    debugStr("\n");
    debugInt(length);
    debugStr("\n");
    fseek(fileOpen, location, SEEK_SET);
    fread(s->mem_buf, s->unit_size, length, fileOpen);
    printf("Loaded %d units into memory", length);
    fclose(fileOpen);
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
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebug}, {"Set File Name", setFileName},
     {"Set Unit Size", setUnitSize}, {"Load Into Memory", loadIntoMem}, {"Toggle Display Mode", toggleDisplay},
      {"Memory Display", memoryDisplay}, {"Save Into File", saveIntoFile}, {"Memory Modify", memoryModify}, {"Quit", quit}, {NULL, NULL}};
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
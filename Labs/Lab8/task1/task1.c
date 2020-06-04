#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#define ESTIMATED_MAX_MENU_SIZE 1024
#define MAX_INPUT_SIZE 1024
#define MAX_FILE_NAME 100
#define MAX_FILE_SIZE 10000

typedef struct
{
    int currentFd;
    struct stat fd_stat;
    char debug_mode;
    void *map_start;
    Elf32_Ehdr *header;
} state;

struct fun_desc
{
    char *name;
    void (*fun)(state *s);
};

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

void toggleDebug(state *s)
{
    s->debug_mode = !(s->debug_mode);
    char *mode = s->debug_mode ? "on" : "off";
    printf("Debug mode is now %s\n", mode);
}

void examineElf(state *s)
{
    s -> currentFd = -1;
    char fileName[MAX_FILE_NAME];
    printf("enter ELF file name: ");
    fgets(fileName, MAX_FILE_NAME, stdin);
    sscanf(fileName, "%s", fileName);
    if ((s -> currentFd = open(fileName, O_RDWR)) < 0)
    {
        perror("error in open");
        return;
    }
    if (fstat(s->currentFd, &s->fd_stat) != 0)
    {
        perror("stat failed");
        return;
    }
    if ((s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, s->currentFd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        return;
    }
    s->header = (Elf32_Ehdr *)s->map_start;
    char magic1 = s->header->e_ident[EI_MAG1];
    char magic2 = s->header->e_ident[EI_MAG2];
    char magic3 = s->header->e_ident[EI_MAG3];
    if (magic1 != 'E' || magic2 != 'L' || magic3 != 'F')
    {
        printf("given file is not in the form on an ELF, canceling operation...");
        if (close(s->currentFd) < 0)
        {
            perror("error in close");
            exit(-1);
        }
        s -> currentFd = -1;
        if (munmap(s->map_start, s->fd_stat.st_size) < 0)
        {
            perror("error in unmap");
            exit(-1);
        }
        return;
    }
    printf("Magic: %28c, %c, %c\n", s->header->e_ident[EI_MAG1],
           s->header->e_ident[EI_MAG2], s->header->e_ident[EI_MAG3]);
    printf("Data: %57s\n", s->header->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian"
                : s->header->e_ident[EI_DATA] == ELFDATA2MSB ? "2's complement, big endian" : "Invalid data encoding");
    printf("Entry point: %#30x\n", s->header->e_entry);
    printf("Start of section headers: %12d (bytes into the file)\n", s -> header -> e_shoff);
    printf("Number of section headers: %9d\n", s -> header -> e_shnum);
    printf("Size of section headers: %11d (bytes)\n", s -> header -> e_shentsize);
    printf("Start of program headers: %10d (bytes into the file)\n", s -> header -> e_phoff);
    printf("Number of program headers: %8d\n", s -> header -> e_phnum);
    printf("Size of program headers: %11d (bytes)\n", s -> header -> e_phentsize);
}

void quit(state *s)
{
    if(s -> currentFd != -1)
    {
        if (close(s->currentFd) < 0)
        {
            perror("current fd has been already closed");
        }
        if (munmap(s->map_start, s->fd_stat.st_size) < 0)
        {
            perror("error in unmap");
        }
    }
    printf("exiting...\n");
    exit(0);
}

void printSectionNames(state *s)
{
    if(s -> currentFd == -1)
    {
        printf("invalid fd\n");
        return;
    }
    unsigned int section_header_size = s -> header -> e_shentsize;
    unsigned int section_headers_offset = s -> header -> e_shoff;
    Elf32_Shdr *section_headers_str_table_header = (Elf32_Shdr*)((s -> map_start) + section_headers_offset + (s -> header -> e_shstrndx * section_header_size));
    int section_headers_str_table_offset = section_headers_str_table_header -> sh_offset;
    char *shstrtab = (char*)(s -> map_start + section_headers_str_table_offset);
    int section_headers_table_size = s -> header -> e_shnum;
    printf("%-6s%-24s%-12s%-12s%-12s%-12s\n", "[Nr]", "Name", "Addr", "Off", "Size", "Type");
    for(int i = 0; i < section_headers_table_size; i ++)
    {
        Elf32_Shdr *current_header = (Elf32_Shdr*)((s -> map_start) + section_headers_offset + section_header_size*i);
        char *name = (char*)(shstrtab + current_header -> sh_name);
        printf("%-6d%-24s%#-12x%#-12x%#-12x%#-12x\n", i, name,
         current_header -> sh_addr, current_header -> sh_offset, current_header -> sh_size, current_header -> sh_type);
    }
}

int main(int argc, char **argv)
{
    int boundary, input = -1;
    char inputAsString[MAX_INPUT_SIZE];
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebug}, {"Examine ELF File", examineElf}, {"Print Section Names", printSectionNames},
                             {"Quit", quit}, {NULL, NULL}};
    boundary = getBoundary(menu);
    state s;
    s.currentFd = -1;
    s.debug_mode = 0;
    while (1)
    {
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
}
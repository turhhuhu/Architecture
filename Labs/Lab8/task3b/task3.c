#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
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
    if(s -> currentFd != -1)
    {
        if (close(s->currentFd) < 0)
        {
            perror("error in close");
        }
    }
    s->currentFd = -1;
    char fileName[MAX_FILE_NAME];
    printf("enter ELF file name: ");
    fgets(fileName, MAX_FILE_NAME, stdin);
    sscanf(fileName, "%s", fileName);
    if ((s->currentFd = open(fileName, O_RDWR)) < 0)
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
        }
        s->currentFd = -1;
        if (munmap(s->map_start, s->fd_stat.st_size) < 0)
        {
            perror("error in unmap");
        }
        return;
    }
    printf("Magic: %28c, %c, %c\n", s->header->e_ident[EI_MAG1],
           s->header->e_ident[EI_MAG2], s->header->e_ident[EI_MAG3]);
    printf("Data: %57s\n", s->header->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian"
                                                                      : s->header->e_ident[EI_DATA] == ELFDATA2MSB ? "2's complement, big endian" : "Invalid data encoding");
    printf("Entry point: %#30x\n", s->header->e_entry);
    printf("Start of section headers: %12d (bytes into the file)\n", s->header->e_shoff);
    printf("Number of section headers: %9d\n", s->header->e_shnum);
    printf("Size of section headers: %11d (bytes)\n", s->header->e_shentsize);
    printf("Start of program headers: %10d (bytes into the file)\n", s->header->e_phoff);
    printf("Number of program headers: %8d\n", s->header->e_phnum);
    printf("Size of program headers: %11d (bytes)\n", s->header->e_phentsize);
}

void quit(state *s)
{
    if (s->currentFd != -1)
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
    if (s->currentFd == -1)
    {
        printf("invalid fd\n");
        return;
    }
    unsigned int section_header_size = s->header->e_shentsize;
    unsigned int section_headers_offset = s->header->e_shoff;
    Elf32_Shdr *section_headers_str_table_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + (s->header->e_shstrndx * section_header_size));
    int section_headers_str_table_offset = section_headers_str_table_header->sh_offset;
    char *shstrtab = (char *)(s->map_start + section_headers_str_table_offset);
    int section_headers_table_size = s->header->e_shnum;
    printf("%-6s%-24s%-12s%-12s%-12s%-12s\n", "[Nr]", "Name", "Addr", "Off", "Size", "Type");
    for (int i = 0; i < section_headers_table_size; i++)
    {
        Elf32_Shdr *current_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * i);
        char *name = (char *)(shstrtab + current_header->sh_name);
        printf("%-6d%-24s%#-12x%#-12x%#-12x%#-12x\n", i, name,
               current_header->sh_addr, current_header->sh_offset, current_header->sh_size, current_header->sh_type);
    }
}

void printSymbols(state *s)
{
    if (s->currentFd == -1)
    {
        printf("invalid fd\n");
        return;
    }
    char has_symbols = 0;
    unsigned int section_header_size = s->header->e_shentsize;
    unsigned int section_headers_offset = s->header->e_shoff;
    Elf32_Shdr *section_headers_str_table_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + (s->header->e_shstrndx * section_header_size));
    int section_headers_str_table_offset = section_headers_str_table_header->sh_offset;
    char *shstrtab = (char *)(s->map_start + section_headers_str_table_offset);
    int section_headers_table_size = s->header->e_shnum;
    for (int i = 0; i < section_headers_table_size; i++)
    {
        Elf32_Shdr *current_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * i);
        int header_type = current_header -> sh_type;
        char *name = (char *)(shstrtab + current_header->sh_name);
        if (header_type == SHT_DYNSYM || header_type == SHT_SYMTAB)
        {
            has_symbols = 1;
            Elf32_Shdr *symtab_header = current_header;
            int symtab_offset = symtab_header->sh_offset;
            int symtab_size = symtab_header->sh_size;
            int associated_str_tab_index = symtab_header->sh_link;
            Elf32_Shdr *associated_str_tab_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * associated_str_tab_index);
            int associated_str_tab_offset = associated_str_tab_header->sh_offset;
            char *strtab = (char *)(s->map_start + associated_str_tab_offset);
            int number_of_symbols = symtab_size / sizeof(Elf32_Sym);
            printf("Symbol table '%s' contains %d entries:\n", name, number_of_symbols);
            printf("  %-6s%-12s%-12s%-12s%-12s\n", "[Nr]", "Value", "Ndx Name", "Sec Name", "Name");
            for (int i = 0; i < number_of_symbols; i++)
            {
                Elf32_Sym *current_symbol = (Elf32_Sym *)((s->map_start) + symtab_offset + sizeof(Elf32_Sym) * i);
                int sym_index_in_strtab = current_symbol->st_name;
                char *name = (char *)(strtab + sym_index_in_strtab);
                int associated_section_header_index = current_symbol->st_shndx;
                if (associated_section_header_index == SHN_UNDEF)
                {
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", i, current_symbol->st_value, associated_section_header_index, "UND", name);
                }
                else if (associated_section_header_index > section_headers_table_size)
                {
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", i, current_symbol->st_value, associated_section_header_index, "ABS", name);
                }
                else
                {
                    Elf32_Shdr *associated_section_header = ((s->map_start) + section_headers_offset + section_header_size * associated_section_header_index);
                    char *section_name = (char *)(shstrtab + associated_section_header->sh_name);
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", i, current_symbol->st_value, associated_section_header_index, section_name, name);
                }
            }
            printf("\n\n");
        }
    }
    if (has_symbols == 0)
    {
        printf("no symbols in given elf file\n");
        return;
    }
}

void printReloc(state *s)
{
    if (s->currentFd == -1)
    {
        printf("invalid fd\n");
        return;
    }
    unsigned int section_header_size = s->header->e_shentsize;
    unsigned int section_headers_offset = s->header->e_shoff;
    Elf32_Shdr *section_headers_str_table_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + (s->header->e_shstrndx * section_header_size));
    int section_headers_str_table_offset = section_headers_str_table_header->sh_offset;
    char *shstrtab = (char *)(s->map_start + section_headers_str_table_offset);
    int section_headers_table_size = s->header->e_shnum;
    for (int i = 0; i < section_headers_table_size; i++)
    {
        Elf32_Shdr *current_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * i);
        int header_type = current_header -> sh_type;
        char *name = (char *)(shstrtab + current_header->sh_name);
        if (header_type == SHT_REL)
        {
            Elf32_Shdr *rel_header = current_header;
            int rel_offset = rel_header->sh_offset;
            int rel_size = rel_header->sh_size;
            int number_of_rel = rel_size / sizeof(Elf32_Rel);
            int dynsym_header_index = rel_header -> sh_link;
            Elf32_Shdr *dynsym_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * dynsym_header_index);
            int dynsyn_table_offset = dynsym_header -> sh_offset;
            int associated_str_tab_index = dynsym_header -> sh_link;
            Elf32_Shdr *associated_str_tab_header = (Elf32_Shdr *)((s->map_start) + section_headers_offset + section_header_size * associated_str_tab_index);
            int associated_str_tab_offset = associated_str_tab_header->sh_offset;
            char *strtab = (char *)(s->map_start + associated_str_tab_offset);
            printf("Relocation section '%s' at offset 0x340 contains 8 entries:\n", name);
            printf(" %-12s%-11s%-9s%-12s%-12s\n", "Offset", "Info", "Type", "Sym.value", "Sym. name");
            for(int i = 0; i < number_of_rel; i++)
            {
                Elf32_Rel *current_rel = (Elf32_Rel *)((s->map_start) + rel_offset + sizeof(Elf32_Rel) * i);
                int offset = current_rel -> r_offset;
                int info = current_rel -> r_info;
                int symbol_offset = ELF32_R_SYM(info);
                Elf32_Sym *current_symbol = (Elf32_Sym *)((s->map_start) + dynsyn_table_offset + sizeof(Elf32_Sym)*symbol_offset);
                int sym_index_in_strtab = current_symbol -> st_name;
                char *name = (char *)(strtab + sym_index_in_strtab);
                int value = current_symbol -> st_value;
                int type = ELF32_R_TYPE(info);
                printf("%#-12x%#-12x%-9d%#-12x%-12s\n", offset, info, type, value, name);
            }
            printf("\n");
        }
    }
}

int main(int argc, char **argv)
{
    int boundary, input = -1;
    char inputAsString[MAX_INPUT_SIZE];
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebug}, {"Examine ELF File", examineElf}, {"Print Section Names", printSectionNames},
     {"Print Symbols", printSymbols}, {"Relocation Tables", printReloc}, {"Quit", quit}, {NULL, NULL}};
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
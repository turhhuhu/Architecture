#include <elf.h>
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
    if ((s->currentFd = open(fileName, O_RDONLY)) < 0)
    {
        perror("error in open");
        return;
    }
    if (fstat(s->currentFd, &s->fd_stat) != 0)
    {
        perror("stat failed");
        return;
    }
    if ((s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ, MAP_PRIVATE, s->currentFd, 0)) == MAP_FAILED)
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
    if (s->currentFd == -1)
    {
        printf("invalid fd\n");
        return;
    }
    unsigned int section_headers_offset = s -> header -> e_shoff;
    Elf32_Shdr *section_headers_str_table_header = &(((Elf32_Shdr *) (s->map_start + section_headers_offset))[s->header->e_shstrndx]);
    Elf32_Shdr *shdr_start = (Elf32_Shdr *)(s->map_start + section_headers_offset);
    int section_headers_str_table_offset = section_headers_str_table_header -> sh_offset;
    char *shstrtab = (char*)(s -> map_start + section_headers_str_table_offset);
    int section_headers_table_size = s -> header -> e_shnum;
    printf("%-6s%-24s%-12s%-12s%-12s%-12s\n", "[Nr]", "Name", "Addr", "Off", "Size", "Type");
    for(int i = 0; i < section_headers_table_size; i++)
    {
        Elf32_Shdr *current_header = &(shdr_start[i]);
        char *name = &(shstrtab[current_header->sh_name]);
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
    unsigned int section_headers_offset = s -> header -> e_shoff;
    Elf32_Shdr *section_headers_str_table_header = &(((Elf32_Shdr *) (s->map_start + section_headers_offset))[s->header->e_shstrndx]);
    Elf32_Shdr *shdr_start = (Elf32_Shdr *)(s->map_start + section_headers_offset);
    int section_headers_str_table_offset = section_headers_str_table_header -> sh_offset;
    char *shstrtab = (char*)(s -> map_start + section_headers_str_table_offset);
    int section_headers_table_size = s -> header -> e_shnum;
    for(int i = 0; i < section_headers_table_size; i++)
    {
        Elf32_Shdr *current_header = &(shdr_start[i]);
        int header_type = current_header -> sh_type;
        char *header_name = &(shstrtab[current_header->sh_name]);
        if(header_type == SHT_DYNSYM || header_type == SHT_SYMTAB)
        {
            has_symbols = 1;
            Elf32_Shdr *symtab_header = current_header;
            int symtab_offset = symtab_header -> sh_offset;
            Elf32_Sym *sym_start = (Elf32_Sym *)(s->map_start + symtab_offset);
            int symtab_size = symtab_header -> sh_size;
            int associated_str_tab_index = symtab_header -> sh_link;
            Elf32_Shdr *associated_str_tab_header = &(shdr_start[associated_str_tab_index]);
            int associated_str_tab_offset = associated_str_tab_header -> sh_offset;
            char *strtab = (char*)(s->map_start + associated_str_tab_offset);
            int number_of_symbols = symtab_size / sizeof(Elf32_Sym);
            printf("Symbol table '%s' contains %d entries:\n", header_name, number_of_symbols);
            printf("  %-6s%-12s%-12s%-12s%-12s\n", "Num", "Value", "Ndx Name", "Sec Name", "Name");
            for(int j = 0; j < number_of_symbols; j++)
            {
                Elf32_Sym *current_symbol = &(sym_start[j]);
                int sym_index_in_strtab = current_symbol -> st_name;
                char *sym_name = &(strtab[sym_index_in_strtab]);
                int associated_section_header_index = current_symbol -> st_shndx;
                if(associated_section_header_index == SHN_UNDEF)
                {
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", j, current_symbol->st_value, associated_section_header_index, "UND", sym_name);
                }
                else if(associated_section_header_index > section_headers_table_size)
                {
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", j, current_symbol->st_value, associated_section_header_index, "ABS", sym_name);
                }
                else
                {
                    Elf32_Shdr *associated_section_header = &(shdr_start[associated_section_header_index]);
                    char *section_name = &(shstrtab[associated_section_header -> sh_name]);
                    printf("  %-6d%#-12x%-12d%-12s%-12s\n", j, current_symbol->st_value, associated_section_header_index, section_name, sym_name);
                }
            }
            printf("\n");
        }
    }
    if (has_symbols == 0)
    {
        printf("no symbols in given elf file\n");
        return;
    }
}


int main(int argc, char **argv)
{
    int boundary, input = -1;
    char inputAsString[MAX_INPUT_SIZE];
    struct fun_desc menu[] = {
        {"Toggle Debug Mode", toggleDebug},
        {"Examine ELF File", examineElf},
        {"Print Section Names", printSectionNames},
        {"Print Symbols", printSymbols},  
        {"Quit", quit},
        {NULL, NULL}
    };
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
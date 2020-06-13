#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int currentFd;
    void *map_start;
    struct stat fd_stat;
    if ((currentFd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("error in open");
        return 1;
    }
    if (fstat(currentFd, &fd_stat) != 0)
    {
        perror("stat failed");
        return 1;
    }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ, MAP_PRIVATE, currentFd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        return 1;
    }
    Elf32_Ehdr* header = (Elf32_Ehdr *)map_start;
    char magic1 = header->e_ident[EI_MAG1];
    char magic2 = header->e_ident[EI_MAG2];
    char magic3 = header->e_ident[EI_MAG3];
    if (magic1 != 'E' || magic2 != 'L' || magic3 != 'F')
    {
        printf("given file is not in the form on an ELF, canceling operation...");
        if (close(currentFd) < 0)
        {
            perror("error in close");
        }
        if (munmap(map_start, fd_stat.st_size) < 0)
        {
            perror("error in unmap");
        }
        return 1;
    }
    int e_phoff = header ->e_phoff;
    Elf32_Phdr *ph_start = (Elf32_Phdr *)(map_start + e_phoff);
    int num_of_ph = header -> e_phnum;
    printf("  %-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n", "Type", "Offset", "VirtAddr", "PhysAddr", "FileSiz", "MemSiz", "Flg", "Align");
    for(int i = 0; i < num_of_ph; i++)
    {
        Elf32_Phdr *curr_ph = &(ph_start[i]);
        int type = curr_ph -> p_type;
        int offset = curr_ph -> p_offset;
        int virtAddr = curr_ph -> p_vaddr;
        int physAddr = curr_ph -> p_paddr;
        int fileSiz = curr_ph -> p_filesz;
        int memSiz = curr_ph -> p_memsz;
        int flag = curr_ph -> p_flags;
        int align = curr_ph -> p_align;
        printf("  %#-12x%#-12x%#-12x%#-12x%#-12x%#-12x%#-12x%#-12x\n", type, offset, virtAddr, physAddr, fileSiz, memSiz, flag, align);
    }
    return 0;
}
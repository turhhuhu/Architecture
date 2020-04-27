
#include "lab4_util.h"

#define SYS_WRITE 4
#define STDOUT 1

int main(int argc, char **argv)
{
    system_call(SYS_WRITE, STDOUT, "hello_world \n", 14);
    system_call(1, 0x55);
}
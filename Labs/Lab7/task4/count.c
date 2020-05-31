#include <stdio.h>
int digit_count (char* str)
{
    int count = 0;
    while(*str != '\0')
    {
        if(*str > 47 && *str < 58)
        {
            count ++;
        }
        str += 1;
    }
    return count;
}

int main(int argc, char**argv)
{
    printf("%d\n", digit_count(argv[1]));
    return 0;
}
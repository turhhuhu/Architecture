#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ENCRYPT_LOWER_BOUND 0x20
#define ENCRYPT_UPPER_BOUND 0x7E
#define ESTIMATED_MENU_BOUND 1000
struct fun_desc
{
    char *name;
    char (*fun)(char);
};

char censor(char c)
{
    if (c == '!')
        return '.';
    else
        return c;
}

char encrypt(char c)
{
    if (c > ENCRYPT_LOWER_BOUND && c < ENCRYPT_UPPER_BOUND)
    {
        return c + 3;
    }
    else
    {
        return c;
    }
}

char decrypt(char c)
{
    if (c > ENCRYPT_LOWER_BOUND && c < ENCRYPT_UPPER_BOUND)
    {
        return c - 3;
    }
    else
    {
        return c;
    }
}

char dprt(char c)
{
    printf("%d\n", c);
    return c;
}

char cprt(char c)
{
    if (c > ENCRYPT_LOWER_BOUND && c < ENCRYPT_UPPER_BOUND)
    {
        printf("%c\n", c);
    }
    else
    {
        printf(".\n");
    }
    return c;
}

char my_get(char c)
{
    return fgetc(stdin);
}

char quit(char c)
{
    if (c == 'q')
    {
        exit(0);
    }
    return c;
}

char *map(char *array, int array_length, char (*f)(char))
{
    int i;
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    for (i = 0; i < array_length; i++)
    {
        mapped_array[i] = (*f)(array[i]);
    }
    return mapped_array;
}

void print_menu(struct fun_desc *menu)
{
    int i;
    for (i = 0; 1; i++)
    {
        if (menu[i].name != NULL && menu[i].fun != NULL)
        {
            printf("%d) %s\n", i, menu[i].name);
        }
        else
        {
            break;
        }
    }
}

int getBoundary(struct fun_desc *menu)
{
    int i;
    for (i = 0; 1; i++)
    {
        if (menu[i].name == NULL || menu[i].fun == NULL || i > ESTIMATED_MENU_BOUND)
        {
            return i;
        }
    }
}

int main(int argc, char **argv)
{
    int boundary, input = -1, arrSize = 5;
    char a[arrSize];
    a[0] = '\0';
    char *carray = a;
    char *pointerHolder;
    struct fun_desc menu[] = {{"Censor", censor}, {"Encrypt", encrypt}, {"Decrypt", decrypt}, {"Print dec", dprt},\
     {"Print string", cprt}, {"Get string", my_get}, {"Quit", quit}, {NULL, NULL}};
    boundary = getBoundary(menu);
    while (1)
    {
        printf("Please choose a function: \n");
        print_menu(menu);
        scanf("%d", &input);
        getchar(); //flushing new line right after getting the int.
        if (input >= 0 && input <= boundary)
        {
            printf("Within bounds\n");
        }
        else
        {
            printf("Not within bounds\n");
            break;
        }
        pointerHolder = map(carray, arrSize, menu[input].fun);
        if (carray != a)
        {
            free(carray);
        }
        carray = pointerHolder;
        printf("DONE.\n\n");
    }
    if (carray != a)
    {
        free(carray);
    }
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct link {
    unsigned char value;
    struct link* next;
} link;

link** stack;
int stack_size;
int max_stack_size;

link* append(link* list, char value){
    link* newlink = (link*)malloc(sizeof(link));
    newlink->value = value;
    newlink->next = list;
    return newlink;
}

void free_list(link* list){
    if (list == NULL) {
        return;
    }
    free_list(list->next);
    free(list);
}

void push(link* list){
    if (stack_size == max_stack_size){
        printf("%s\n", "Error: Operand Stack Overflow");
        free_list(list);
        return;
    }
    stack[stack_size] = list;
    stack_size++;
}

link* pop(){
    if (stack_size == 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return NULL;
    }
    stack_size--;
    link* list = stack[stack_size];
    return list;
}

link* peek(){
    return stack[stack_size-1];
}

char charToNum(char x){
    if (x >= 65){
        x = x - 7;
    }
    int y = x - 48;
    return y;
}

void unsignedAddition(){
    if (stack_size - 2 < 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    link* op1 = pop(); 
    link* op2 = pop();
    link* temp1 = op1;
    link* prev1 = op1;
    link* temp2 = op2;
    link* prev2 = op2;
    char overflow = 0;
    while (temp1 != NULL && temp2 != NULL) {
        int x = temp1->value;
        x = x + temp2->value;
        x = x + overflow;
        temp1->value = temp1->value + temp2->value + overflow;
        if (x > 255){
            overflow = 1;
        } else {
            overflow = 0;
        }
        prev1 = temp1;
        prev2 = temp2;
        temp1 = temp1->next;
        temp2 = temp2->next;
    }
    if (temp1 == NULL){
        prev1->next = prev2->next;
        prev2->next = NULL;
        temp1 = prev1->next;
    }
    while (temp1 != NULL)
    {
        int x = temp1->value;
        x = x + overflow;
        temp1->value = temp1->value + overflow;
        if (x > 255){
            overflow = 1;
        } else {
            overflow = 0;
        }
        prev1 = temp1;
        temp1 = temp1->next; 
    }
    if (overflow == 1){
        prev1->next = append(NULL, 1);
    }
    free_list(op2);
    push(op1);    
    
}

void print_list_recursive(link* list){
    int x = list->value;
    if (list->next == NULL){
        printf("%X", x);
        return;
    }
    print_list_recursive(list->next);
    printf("%02X", x);
}

void print_list(link* list){
    print_list_recursive(list);
    printf("\n");
}

void popAndPrint(){
    if (stack_size - 1 < 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    link* op = pop();
    if (op == NULL){
        return;
    }
    print_list(op); 
    free_list(op);
}

link* duplicate_recursive(link* list){
    if (list == NULL){
        return NULL;
    }
    return append(duplicate_recursive(list->next), list->value);
}

void duplicate(){
    if (stack_size == 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    if (stack_size + 1 > max_stack_size){
        printf("%s\n", "Error: Operand Stack Overflow");
        return;
    }
    link* op = peek();
    push(duplicate_recursive(op));
}

void bitwiseAnd(){
    if (stack_size - 2 < 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    link* op1 = pop(); 
    link* op2 = pop();
    link* temp1 = op1;
    link* temp2 = op2;
    while (temp1->next != NULL && temp2->next != NULL){
        temp1->value = temp1->value & temp2->value;
        temp1 = temp1->next;
        temp2 = temp2->next;
    }
    temp1->value = temp1->value & temp2->value;
    if (temp1->next != NULL){
        free_list(temp1->next);
        temp1->next = NULL;
    }
    free_list(op2);
    push(op1);
}

void bitwiseOr() {
    if (stack_size - 2 < 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    link* op1 = pop(); 
    link* op2 = pop();
    link* temp1 = op1;
    link* temp2 = op2;
    while (temp1->next != NULL && temp2->next != NULL){
        temp1->value = temp1->value | temp2->value;
        temp1 = temp1->next;
        temp2 = temp2->next;
    }
    temp1->value = temp1->value | temp2->value;
    if (temp1->next == NULL){
        if (temp2->next != NULL){
            temp1->next = temp2->next;
            temp2->next = NULL;
        }
    }
    free_list(op2);
    push(op1);
}

void countNumOfHexDigits(){
    if (stack_size - 1 < 0){
        printf("%s\n", "Error: Insufficient Number of Arguments on Stack");
        return;
    }
    link* op = pop();
    link* temp = op;
    link* counter_list = NULL;
    char counter = 0;
    while (temp != NULL)
    {
        if (temp->next == NULL && temp->value < 16){
            counter++;
        } else {
            counter = counter + 2;
        }
        temp = temp->next;
    }
    push(append(counter_list, counter));
    
}

int myCalc(){
    int numOfActions = 0;
    int i = 0;
    link* list = NULL;
    char buffer[80], cont = 0;
    while (1){
        cont = 0;   
        printf("%s", "calc: ");
        fgets(buffer, 80, stdin);
        switch (buffer[0])
        {
        case '+':
            unsignedAddition();
            cont = 1;
            break;
        case 'p':
            popAndPrint();
            cont = 1;
            break;
        case 'd':
            duplicate();
            cont = 1;
            break;
        case '&':
            bitwiseAnd();
            cont = 1;
            break;
        case '|':
            bitwiseOr();
            cont = 1;
            break;
        case 'n':
            countNumOfHexDigits();
            cont = 1;
            break;
        case 'q':
            return numOfActions;
        }
        if (cont){
            numOfActions++;
            continue;
        }
        list = NULL;
        i = 0; 
        if ((strlen(buffer)-1) % 2 == 1){
            list = append(list, charToNum(buffer[0]));
            i++;
        }
        while (!(buffer[i] == '\n' || buffer[i] == '\0')){
            int x = charToNum(buffer[i])*16 + charToNum(buffer[i+1]);
            list = append(list, x);
            i = i + 2;
        }
        push(list);
    }

}

int main(int argc, char* argv[]){
    max_stack_size = 5;
    if (argc > 1){
        if (strncmp(argv[1], "-d", 2) == 0){
            printf("%s\n", "debug");
        } else {
            max_stack_size = atol(argv[1]);
        }
    }
    stack = (link**) calloc(max_stack_size, sizeof(link*));
    stack_size = 0;  
    printf("%X\n", myCalc());
}
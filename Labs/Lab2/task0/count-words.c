/* $Id: count-words.c 858 2010-02-21 10:26:22Z tolpin $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* return string "word" if the count is 1 or "words" otherwise */
char *words(int count) {
  char *words = malloc(256);
  strcpy(words, "words");
  if(count==1) 
    strcpy(words, "word");
  return words;
}

/* print a message reportint the number of words */
int print_word_count(char **argv) {
  int count = 0;
  char **a = argv;
  while(*(a++))
    ++count;
  char* word = words(count);
  printf("The sentence contains %d %s.\n", count, word);
  free(word);
  return count;
}

/* print the number of words in the command line and return the number as the exit code */
int main(int argc, char **argv) {
  return print_word_count(argv+1);
}

   

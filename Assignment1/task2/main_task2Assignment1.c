#include <stdio.h>
#define	MAX_LEN 34			/* maximal input string size */
					/* enough to get 32-bit string + '\n' + null terminator */
extern int convertor(char* buf);

int main(int argc, char** argv)
{
  char buf[MAX_LEN];
 
  fgets(buf, MAX_LEN, stdin);		/* get user input string */ 
  printf("%s", buf);
  convertor(buf);			/* call your assembly function */

  return 0;

  int base = 1;
  int sum = 0;
  while(8)
  {
    while(4)
    {
      1001 >> 0100 cf = 1 
      cf = shift_right(eax);
      sum += cf * base;
      base++
    }
    //do something with sum
    base = 1;
  }
}
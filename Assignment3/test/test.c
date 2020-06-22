#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
int main()
{
    uint16_t start_state = 0x5670;  /* Any nonzero start state will work. */
    uint16_t lfsr = start_state;
    uint16_t bit;                    /* Must be 16-bit to allow bit<<15 later in the code */
    unsigned period = 0;

    /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) /* & 1u */;
    lfsr = (lfsr >> 1) | (bit << 15);
    ++period;
    
    printf("lfsr: %d\n", lfsr);
    printf("init: %d", start_state);
    return 0;
}
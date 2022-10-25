#include <stdio.h>
#include <stdint.h>

#ifndef N
#define N 3
#endif

// Interfejs między C a Asemblerem
uint64_t notec(uint32_t n, char const *calc);
int64_t debug(uint32_t n, uint64_t *stack_pointer);

static int errors = 0;
static int debug_calls = 0;

int64_t debug(uint32_t n, uint64_t *stack_pointer) {
    ++debug_calls;
    if(n == 0) {
        if (*stack_pointer != 0x0) {
            printf("DEBUG_Y_FAIL\n");
            errors++;
        }
        return 1;
    } else if (n == 1) {
        if (*(stack_pointer + 1) != 0x4) {
            printf("DEBUG_X_FAIL\n");
            errors++;
        }
        return 0;
    }
    printf("DEBUG_FAIL\n");
    errors++;
    return 0;
}


int main()  {
    
    printf("Starting unit tests\n");

    if(notec(0, "3=4+") != 0x7) {
        printf("Failed addition\n");
        errors++;
    }
    if(notec(0, "3=4*") != 0xc) {
        printf("Failed multiplication\n");
        errors++;
    }
    if(notec(0, "1-") != (uint64_t) -0x1) {
        printf("Failed arithmetic negation\n");
        errors++;
    }
    if(notec(0, "5=7&") != 0x5) {
        printf("Failed AND\n");
        errors++;
    }
    if(notec(0, "5=7|") != 0x7) {
        printf("Failed OR\n");
        errors++;
    }
    if(notec(0, "5=7^") != 0x2) {
        printf("Failed XOR\n");
        errors++;
    }
    if(notec(0, "1~") != (uint64_t) -0x2) {
        printf("Failed bitwise negation\n");
        errors++;
    }
    if(notec(0, "3=4Z") != 0x3) {
        printf("Failed Z\n");
        errors++;
    }
    if(notec(0, "0Yg") != 0x0) {
        printf("Failed Y\n");
        errors++;
    }
    if(notec(1, "3=4Xg") != 0x3) {
        printf("Failed X\n");
        errors++;
    }
    if(notec(0, "N") != N) {
        printf("Failed N\n");
        errors++;
    }
    if(notec(0, "n") != 0) {
        printf("Failed n\n");
        errors++;
    }
    if(debug_calls != 2) {
        printf("Failed number of debug calls, equal to %d\n", debug_calls);
        errors++;
    }
    if (errors == 0) {
        printf("Unit passed!\n");
    } else {
        printf("Unit failed with %d errors\n", errors);
    }
  //printf("%lu\n", notec(1, "13432=3132=4*53Y-32n+33&123|33^gNZ~"));
    return 0;
}


#include <stdio.h>

static volatile int b_ = 0;

int test_1()
{
    int a = 100;
    int result;

    result = a / b_;

    if (b_ == 0) {
        printf("A: b is 0 (result=%d)\n", result);
    } else {
        printf("A: b is NOT 0\n");
    }

    return 0;
}

int test_2()
{
    int a = 100;
    int b = b_;

    int result = a / b;

    if (b == 0) {
        printf("B: b is 0 (result=%d)\n", result);
    } else {
        printf("B: b is NOT 0(result=%d)\n", result);
    }

    return 0;
}
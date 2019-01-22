#include <stdio.h>
#include <stdlib.h>
#include "expect.h"

void expect(int line, int expected, int actual) {
    if (expected == actual)
        return;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

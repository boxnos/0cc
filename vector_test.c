#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void expect(int line, int expected, int actual) {
    if (expected == actual)
        return;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

int main() {
    vector *v = new_vector();
    expect(__LINE__, 0, v->size);

    for (int i = 0; i < 100; i++)
        vector_push(v, (void *)(long) i);
    expect(__LINE__, 100, v->size);
    expect(__LINE__, 50, (long) v->data[50]);
    expect(__LINE__, 99, (long) v->data[99]);

    printf("OK\n");

    return 0;
}

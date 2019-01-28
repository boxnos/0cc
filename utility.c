#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

void dump(char *expect, char *got) {
    fprintf(stderr, "ERROR: expected %s, but got %s\n", expect, got);
    exit(1);
}

void error(char *s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
}

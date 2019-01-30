#include "utility.h"
#include "generate.h"

int main(int argc, char **argv) {
    if (argc != 2)
        error("arg : wrong arguments.");

    generate(argv[1]);

    return 0;
}

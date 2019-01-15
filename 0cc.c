#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum { TK_NUM, TK_EOF };

typedef struct {
    int type;
    int value;
    char *input;
} Token;

Token tokens[100];

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].type = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }
        if (isdigit(*p)) {
            tokens[i].type = TK_NUM;
            tokens[i].value = strtol(p, &p, 10);
            tokens[i].input = p;
            i++;
            continue;
        }
        fprintf(stderr, "tokenize : error unexpexted input.\n");
        exit(1);
    }
    tokens[i].type = TK_EOF;
    tokens[i].input = p;
}

void error(int i, char *s) {
    fprintf(stderr, "ERROR: expected %s, but got %s\n", s, tokens[i].input);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: wrong arguments.");
        return 1;
    }

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    if (tokens[0].type != TK_NUM)
        error(0, "number");
    printf("\tmov rax, %d\n", tokens[0].value);
    
    int i = 1;
    while (tokens[i].type != TK_EOF) {
        if (tokens[i].type == '+') {
            i++;
            if (tokens[i].type != TK_NUM)
                error(i, "number");
            printf("\tadd rax, %d\n", tokens[i].value);
            i++;
            continue;
        }
        if (tokens[i].type == '-') {
            i++;
            if (tokens[i].type != TK_NUM)
                error(i, "number");
            printf("\tsub rax, %d\n", tokens[i].value);
            i++;
            continue;
        }
        fprintf(stderr, "main : error unexpexted input.\n");
        exit(1);
    }

    /*
    while (*p) {
        if (*p == '+') {
            p++;
            printf("\tadd rax, %ld\n", strtol(p, &p, 10));
            continue;
        }
        if (*p == '-') {
            p++;
            printf("\tsub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }
        fprintf(stderr, "ERROR: expected '+' , '-', but got '%c'\n", *p);
        return 1;
    }
    */

    printf("\tret\n");

    return 0;
}

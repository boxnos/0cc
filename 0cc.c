#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "map.h"
#include "node.h"
#include "utility.h"

void gen_lvalue(node *node) {
    if (node->type != ND_IDENT)
        error("syntax error : expected lvalue.");
    puts("\tmov rax, rbp");
    int i = (long) map_get(env, node->string);
    printf("\tsub rax, %d\n", i * 8);
    puts("\tpush rax");
}

void gen(node *n) {
    puts("");
    if (n->type == ND_NUM) {
        printf("\tpush %d\n", n->value);
        return;
    }
    if (n->type == ND_IDENT) {
        gen_lvalue(n);
        puts("\tpop rax");
        puts("\tmov rax, [rax]");
        puts("\tpush rax");
        return;
    }
    if (n->type == '=') {
        gen_lvalue(n->lhs);
        gen(n->rhs);
        puts("\tpop rdi");
        puts("\tpop rax");
        puts("\tmov [rax], rdi");
        puts("\tpush rdi");
        return;
    }

    gen(n->lhs);
    gen(n->rhs);

    puts("\tpop rdi");
    puts("\tpop rax");

    switch (n->type) {
    case '+' :
        puts("\tadd rax, rdi");
        break;
    case '-' :
        puts("\tsub rax, rdi");
        break;
    case '*' :
        puts("\tmul rdi");
        break;
    case '/' :
        puts("\tmov rdx, 0");
        puts("\tdiv rdi");
        break;
    }
    printf("\tpush rax\n");
}

int main(int argc, char **argv) {
    if (argc != 2)
        error("arg : wrong arguments.");

    program(argv[1]);
    //display();

    puts(".intel_syntax noprefix");
    puts(".global main");
    puts("main:");

    puts("\tpush rbp");
    puts("\tmov rbp, rsp");
    printf("\tsub rsp, %d\n", env->keys->size * 8);

    for (int i = 0; code[i] != NULL; i++) {
        gen(code[i]);
        puts("");
        puts("\tpop rax");
    }

    puts("\tmov rsp, rbp");
    puts("\tpop rbp");
    puts("\tret");

    return 0;
}

#include <stdio.h>
#include "map.h"
#include "node.h"
#include "utility.h"

void gen_lvalue(node *node) {
    if (node->type != ND_IDENT)
        error("syntax error : expected lvalue.");
    puts("\tmovq %rbp, %rax");
    int i = (long) map_get(env, node->string);
    printf("\tsubq $%d, %%rax\n", i * 8);
    puts("\tpushq %rax");
}

void gen(node *n) {
    puts("");
    if (n->type == ND_NUM) {
        printf("\tpushq $%d\n", n->value);
        return;
    }
    if (n->type == ND_IDENT) {
        gen_lvalue(n);
        puts("\tpopq %rax");
        puts("\tmovq (%rax), %rax");
        puts("\tpushq %rax");
        return;
    }
    if (n->type == '=') {
        gen_lvalue(n->lhs);
        gen(n->rhs);
        puts("\tpopq %rdi");
        puts("\tpopq %rax");
        puts("\tmovq %rdi, (%rax)");
        puts("\tpushq %rdi");
        return;
    }

    gen(n->lhs);
    gen(n->rhs);

    puts("\tpopq %rdi");
    puts("\tpopq %rax");

    switch (n->type) {
    case '+' :
        puts("\taddq %rdi, %rax");
        break;
    case '-' :
        puts("\tsubq %rdi, %rax");
        break;
    case '*' :
        puts("\tmulq %rdi");
        break;
    case '/' :
        puts("\tmovq $0, %rdx");
        puts("\tdivq %rdi");
        break;
    }
    puts("\tpushq %rax");
}

void generate(char *input) {
    program(input);
    display();

    puts(".global main");
    puts("main:");

    puts("\tpushq %rbp");
    puts("\tmovq %rsp, %rbp");
    printf("\tsubq $%d, %%rsp\n", env->keys->size * 8);

    for (int i = 0; code[i] != NULL; i++) {
        gen(code[i]);
        puts("");
        puts("\tpopq %rax");
    }

    puts("\tmovq %rbp, %rsp");
    puts("\tpopq %rbp");
    puts("\tret");
}

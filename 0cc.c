#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "utility.h"
#include "token.h"

enum {ND_NUM, ND_IDENT};

token *get_token(int i) {
    return (token *) tokens->data[i];
}

map *env;

typedef struct node {
    int type;
    struct node *lhs;
    struct node *rhs;
    union {
        int value;
        char *string;
    };
} node;

node *new_node(int type, node *lhs, node *rhs) {
    node *n = malloc(sizeof(node));
    n->type = type;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

node *new_number(int value) {
    node *n = malloc(sizeof(node));
    n->type = ND_NUM;
    n->value = value;
    return n;
}

node *new_ident(char *string) {
    node *n = malloc(sizeof(node));
    n->type = ND_IDENT;
    n->string = string;
    return n;
}

int consume(int type, int *pos) {
    if (get_token(*pos)->type != type)
        return 0;
    (*pos)++;
    return 1;
}

node *add(int *);

node *term(int *pos) {
    if (consume('(', pos)) {
        node *n = add(pos);
        if (!consume(')', pos))
            dump(")", get_token(*pos)->input);
        return n;
    }
    if (get_token(*pos)->type == TK_IDENT)
        return new_ident(get_token((*pos)++)->string);
    if (get_token(*pos)->type == TK_NUM)
        return new_number(get_token((*pos)++)->value);
    dump("Term", get_token(*pos)->input);
    return NULL;
}

node *mul(int *pos) {
    node *n = term(pos);
    for (;;) {
        if (consume('*', pos))
            n = new_node('*', n, term(pos));
        else if (consume('/', pos))
            n = new_node('/', n, term(pos));
        else
            return n;
    }
}

node *add(int *pos) {
    node *n = mul(pos);
    for (;;) {
        if (consume('+', pos))
            n = new_node('+', n, mul(pos));
        else if (consume('-', pos))
            n = new_node('-', n, mul(pos));
        else
            return n;
    }
}

node *assign(int *pos) {
    node *n = add(pos);
    if (consume('=', pos)) {
        map_put(env, n->string, (void *) (long) env->keys->size);
        return new_node('=', n, assign(pos));
    }
    return n;
}

node *statement(int *pos) {
    node *n = assign(pos);
    if (!consume(';', pos))
        dump(";", get_token(*pos)->input);
    return n;
}

node *code[100];

void program() {
    int i = 0;
    int p = 0;
    while (get_token(p)->type != TK_EOF)
        code[i++] = statement(&p);
    code[i] = NULL;
}

void display_node(node *n) {
    if (n->type == ND_NUM)
        fprintf(stderr, "%d", n->value);
    else if (n->type == ND_IDENT) {
        fprintf(stderr, "%s", n->string);
    } else {
        fprintf(stderr, "(%c ", n->type);
        display_node(n->lhs);
        fprintf(stderr, " ");
        display_node(n->rhs);
        fprintf(stderr, ")");
    }
}

void display() {
    for (int i = 0; code[i] != NULL; i++) {
        display_node(code[i]);
        fprintf(stderr, "\n");
    }
}

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

    tokens = new_vector();
    env = new_map();
    tokenize(argv[1]);

    program();
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

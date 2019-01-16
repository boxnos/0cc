#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum { ND_NUM, TK_NUM, TK_EOF };

typedef struct {
    int type;
    int value;
    char *input;
} Token;

Token tokens[100];

void error(int i, char *s) {
    fprintf(stderr, "ERROR: expected %s, but got %s\n", s, tokens[i].input);
    exit(1);
}

typedef struct Node {
    int type;
    struct Node *lhs;
    struct Node *rhs;
    int value;
} Node;

Node *new_node(int type, Node *lhs, Node *rhs) {
    Node *n = malloc(sizeof(Node));
    n->type = type;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node *new_number(int value) {
    Node *n = malloc(sizeof(Node));
    n->type = ND_NUM;
    n->value = value;
    return n;
}

int consume(int type, int *pos) {
    if (tokens[*pos].type != type)
        return 0;
    (*pos)++;
    return 1;
}

Node *add(int *);

Node *term(int *pos) {
    if (consume('(', pos)) {
        Node *n = add(pos);
        if (!consume(')', pos))
            error(*pos, ")");
        return n;
    }
    if (tokens[*pos].type == TK_NUM)
        return new_number(tokens[(*pos)++].value);
    error(*pos, "Term");
}

Node *mul(int *pos) {
    Node *n = term(pos);
    for (;;) {
        if (consume('*', pos))
            n = new_node('*', n, term(pos));
        else if (consume('/', pos))
            n = new_node('/', n, term(pos));
        else
            return n;
    }
}

Node *add(int *pos) {
    Node *n = mul(pos);
    for (;;) {
        if (consume('+', pos))
            n = new_node('+', n, mul(pos));
        else if (consume('-', pos))
            n = new_node('-', n, mul(pos));
        else
            return n;
    }
}

void display(Node *n) {
    if (n->type == ND_NUM)
        fprintf(stderr, "%d", n->value);
    else {
        fprintf(stderr, "(%c ", n->type);
        display(n->lhs);
        fprintf(stderr, " ");
        display(n->rhs);
        fprintf(stderr, ")");
    }
}

void gen(Node *n) {
    if (n->type == ND_NUM) {
        printf("\tpush %d\n", n->value);
        return;
    }
    gen(n->lhs);
    gen(n->rhs);

    printf("\tpop rdi\n");
    printf("\tpop rax\n");

    switch (n->type) {
    case '+' :
        printf("\tadd rax, rdi\n");
        break;
    case '-' :
        printf("\tsub rax, rdi\n");
        break;
    case '*' :
        printf("\tmul rdi\n");
        break;
    case '/' :
        printf("\tmov rdx, 0\n");
        printf("\tdiv rdi\n");
        break;
    }
    printf("\tpush rax\n");
}

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: wrong arguments.");
        return 1;
    }

    tokenize(argv[1]);

    int pos = 0;
    Node *n = add(&pos);
//    display(n);
//    fprintf(stderr, "\n");

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(n);
    printf("\tpop rax\n");
    printf("\tret\n");

    return 0;
}

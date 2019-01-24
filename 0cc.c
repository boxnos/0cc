#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "vector.h"

enum { ND_NUM, ND_IDENT, TK_NUM, TK_IDENT, TK_EOF };

typedef struct {
    int type;
    char *input;
    union {
        int value;
        vector *string;
    };
} token;

token *new_token(int type, char *input) {
    token *t = malloc(sizeof (token));
    t->type = type;
    t->input = input;
    return t;
}

vector *tokens;

token *get_token(int i) {
    return (token *) tokens->data[i];
}

void dump(int i, char *s) {
    fprintf(stderr, "ERROR: expected %s, but got %s\n", s, get_token(i)->input);
    exit(1);
}

void error(char *s) {
    fprintf(stderr, "%s\n");
    exit(1);
}

typedef struct Node {
    int type;
    struct Node *lhs;
    struct Node *rhs;
    union {
        int value;
        vector *string;
    };
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

Node *new_ident(vector *string) {
    Node *n = malloc(sizeof(Node));
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

Node *add(int *);

Node *term(int *pos) {
    if (consume('(', pos)) {
        Node *n = add(pos);
        if (!consume(')', pos))
            dump(*pos, ")");
        return n;
    }
    if (get_token(*pos)->type == TK_IDENT)
        return new_ident(get_token((*pos)++)->string);
    if (get_token(*pos)->type == TK_NUM)
        return new_number(get_token((*pos)++)->value);
    dump(*pos, "Term");
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

Node *assign(int *pos) {
    Node *n = add(pos);
    if (consume('=', pos))
        return new_node('=', n, assign(pos));
    return n;
}

Node *statement(int *pos) {
    Node *n = assign(pos);
    if (!consume(';', pos))
        dump(*pos, ";");
    return n;
}

Node *code[100];

void program() {
    int i = 0;
    int p = 0;
    while (get_token(p)->type != TK_EOF)
        code[i++] = statement(&p);
    code[i] = NULL;
}

void display_node(Node *n) {
    if (n->type == ND_NUM)
        fprintf(stderr, "%d", n->value);
    else if (n->type == ND_IDENT) {
        for (void **i = n->string->data; *i != NULL; i++)
            fprintf(stderr, "%c", *i);
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

void tokenize(char *p) {
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' || *p == '=' || *p == ';') {
            vector_push(tokens, (void *) new_token(*p, p));
            p++;
            continue;
        }
        if (islower(*p)) {
            vector *s = new_vector();
            do {
                vector_push(s, (void *) (long) *p++);
            } while (islower(*p));
            vector_push(s, (long) '\0');
            token *t = new_token(TK_IDENT, p);
            t->string = s;
            vector_push(tokens, (void *) t);
            continue;
        }
        if (isdigit(*p)) {
            int value = strtol(p, &p, 10);
            token *t = new_token(TK_NUM, p);
            t->value = value;
            vector_push(tokens, (void *) t);
            continue;
        }

        error("tokenize : error unexpexted input.");
    }
    vector_push(tokens, (void *) new_token(TK_EOF, p));
}

void gen_lvalue(Node *node) {
    if (node->type != ND_IDENT)
        error("syntax error : expected lvalue.");
    puts("\tmov rax, rbp");
    printf("\tsub rax, %d\n", ('z' - (long) node->string->data[0] + 1) * 8);
    puts("\tpush rax");
}

void gen(Node *n) {
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
    tokenize(argv[1]);

    program();
    display();

    puts(".intel_syntax noprefix");
    puts(".global main");
    puts("main:");

    puts("\tpush rbp");
    puts("\tmov rbp, rsp");
    puts("\tsub rsp, 208");

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

#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "token.h"
#include "node.h"
#include "utility.h"

token *get_token(int i) {
    return (token *) tokens->data[i];
}

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

node *eq(int *pos) {
    node *n = add(pos);
    for (;;) {
        if (consume(TK_EQ, pos))
            n = new_node(ND_EQ, n, add(pos));
        else if (consume(TK_NEQ, pos))
            n = new_node(ND_NEQ, n, add(pos));
        else
            return n;
    }
}

node *assign(int *pos) {
    node *n = eq(pos);
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


void program(char *s) {
    tokens = new_vector();
    env = new_map();
    tokenize(s);

    int i = 0;
    int p = 0;
    while (get_token(p)->type != TK_EOF)
        code[i++] = statement(&p);
    code[i] = NULL;
}

void display_node(node *n) {
    switch (n->type) {
    case ND_NUM:
        fprintf(stderr, "%d", n->value);
        return;
    case ND_IDENT:
        fprintf(stderr, "%s", n->string);
        return;
    case ND_EQ:
        fprintf(stderr, "(%s ", "==");
        break;
    case ND_NEQ:
        fprintf(stderr, "(%s ", "!=");
        break;
    default:
        fprintf(stderr, "(%c ", n->type);

    }
    display_node(n->lhs);
    fprintf(stderr, " ");
    display_node(n->rhs);
    fprintf(stderr, ")");
}

void display() {
    for (int i = 0; code[i] != NULL; i++) {
        display_node(code[i]);
        fprintf(stderr, "\n");
    }
}

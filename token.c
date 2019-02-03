#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "map.h"
#include "token.h"
#include "utility.h"

token *new_token(int type, char *input) {
    token *t = malloc(sizeof (token));
    t->type = type;
    t->input = input;
    return t;
}

void tokenize(char *p) {
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        switch (*p) {
        case '+' : case '-' : case '*' : case '/' :
        case '(' : case ')' : case ';' :
            vector_push(tokens, (void *) new_token(*p, p));
            p++;
            continue;
        case '=':
            p++;
            if (*p == '=') {
                vector_push(tokens, (void *) new_token(TK_EQ, p));
                p++;
            } else {
                p--;
                vector_push(tokens, (void *) new_token(*p, p));
                p++;
            }
            continue;
        }
        if (islower(*p)) {
            char tmp[256];
            int i = 0;
            do {
                tmp[i++] = *p++;
            } while (islower(*p));
            tmp[i] = '\0';
            token *t = new_token(TK_IDENT, p);
            char *s = malloc(sizeof (char) * i);
            strcpy(s, tmp);
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

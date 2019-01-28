#include "map.h"

enum {TK_NUM, TK_IDENT, TK_EOF};

typedef struct {
    int type;
    char *input;
    union {
        int value;
        char *string;
    };
} token;

vector *tokens;

void tokenize(char *);

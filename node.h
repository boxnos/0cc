enum {ND_NUM, ND_IDENT};

typedef struct node {
    int type;
    struct node *lhs;
    struct node *rhs;
    union {
        int value;
        char *string;
    };
} node;

map *env;
node *code[100];

void program();

enum {ND_NUM, ND_IDENT, ND_CALL, ND_EQ, ND_NEQ};

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
void display();

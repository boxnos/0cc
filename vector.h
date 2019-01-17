typedef struct {
    void **data;
    int capacity;
    int size;
} vector;

vector *new_vector();
void vector_push(vector *, void *);

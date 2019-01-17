#include <stdlib.h>
#include "vector.h"

vector *new_vector() {
    vector *v = malloc(sizeof(vector));
    v->capacity = 16;
    v->data = malloc(sizeof(void *) * v->capacity);
    v->size = 0;
    return v;
}

void vector_push(vector *v, void *obj) {
    if (v->capacity == v->size) {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(void *) * v->capacity);
    }
    v->data[v->size++] = obj;
}


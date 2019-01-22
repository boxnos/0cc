#include <stdlib.h>
#include <string.h>
#include "map.h"

map *new_map() {
    map *m = malloc(sizeof(map));
    m->keys = new_vector();
    m->values = new_vector();
    return m;
}

void map_put(map *m, char *key, void *value) {
    vector_push(m->keys, key);
    vector_push(m->values, value);
}

void *map_get(map *m, char *key) {
    for (int i = m->keys->size - 1; i >= 0; i--)
        if (strcmp(m->keys->data[i], key) == 0)
            return m->values->data[i];
    return NULL;
}

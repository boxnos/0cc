#include "vector.h"

typedef struct {
    vector *keys;
    vector *values;
} map;

map *new_map();
void map_put(map *, char *, void *);
void *map_get(map *, char *);

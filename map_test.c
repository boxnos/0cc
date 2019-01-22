#include <stdio.h>
#include "map.h"
#include "expect.h"

int main() {
    map *m = new_map();
    expect(__LINE__, 0, (long)map_get(m, "foo"));
    map_put(m, "foo", (void *) 3);
    expect(__LINE__, 3, (long)map_get(m, "foo"));
    map_put(m, "bar", (void *) 4);
    expect(__LINE__, 4, (long)map_get(m, "bar"));
    map_put(m, "foo", (void *) 6);
    expect(__LINE__, 6, (long)map_get(m, "foo"));
    puts("OK");
}

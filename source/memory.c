#include <stdlib.h>
#include "polysync/memory.h"
#include "polysync/error.h"

void *polysync_memory_allocate(u32 bytes) {
    void *address = malloc(bytes);

    if (!address) {
        polysync_error("Failed to allocate memory\n");
    }

    return address;
}

void *polysync_memory_reallocate(void *address, u32 bytes) {
    void *new_address = realloc(address, bytes);

    if (!new_address) {
        polysync_error("Failed to reallocate memory\n");
    }

    return new_address;
}

void polysync_memory_free(void *address) {
    free(address);
}

#ifndef POLYSYNC_MEMORY_H
#define POLYSYNC_MEMORY_H

#include "polysync/types.h"

void *polysync_memory_allocate(u32 bytes);
void *polysync_memory_reallocate(void *address, u32 bytes);
void polysync_memory_free(void *address);

#endif

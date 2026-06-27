#ifndef POLYSYNC_TREE_H
#define POLYSYNC_TREE_H

#include <cJSON.h>
#include "polysync/types.h"

typedef struct PolysyncTreeEntry {
    string *source_path;
    string *target_path;
} PolysyncTreeEntry;

typedef struct PolysyncTree {
    PolysyncTreeEntry *entries;
    u32 entry_count;
    u32 entry_capacity;
} PolysyncTree;

PolysyncTree *polysync_tree_create(void);
void polysync_tree_destroy(PolysyncTree *tree);
void polysync_tree_load(PolysyncTree *tree, cJSON *root_object, cstring *source_path);
void polysync_tree_add(PolysyncTree *tree, cstring *source_path, cstring *target_path);
PolysyncTreeEntry *polysync_tree_find(PolysyncTree *tree, cstring *source_path);

#endif

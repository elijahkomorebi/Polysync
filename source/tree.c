#include "polysync/tree.h"
#include "polysync/memory.h"
#include "polysync/string.h"
#include "polysync/error.h"

PolysyncTree *polysync_tree_create(void) {
    PolysyncTree *tree = polysync_memory_allocate(sizeof(PolysyncTree));
    tree->entries = polysync_memory_allocate(sizeof(PolysyncTreeEntry) * 8);
    tree->entry_count = 0;
    tree->entry_capacity = 8;

    return tree;
}

void polysync_tree_destroy(PolysyncTree *tree) {
    for (u32 entry_index = 0; entry_index < tree->entry_count; entry_index++) {
        PolysyncTreeEntry entry = tree->entries[entry_index];

        polysync_memory_free(entry.source_path);
        polysync_memory_free(entry.target_path);
    }

    polysync_memory_free(tree->entries);
    polysync_memory_free(tree);
}

void polysync_tree_load(PolysyncTree *tree, cJSON *root_object, cstring *source_path) {
    cJSON *object = null;

    cJSON_ArrayForEach(object, root_object) {
        string *path = null;

        if (source_path) {
            path = polysync_string_concatenate(source_path, "/", object->string, null);
        } else {
            path = polysync_string_copy(object->string);
        }

        if (cJSON_IsObject(object)) {
            polysync_tree_load(tree, object, path);
        } else if (cJSON_IsString(object)) {
            polysync_tree_add(tree, path, object->valuestring);
        } else {
            polysync_error("Failed to load tree");
        }

        polysync_memory_free(path);
    }
}

void polysync_tree_add(PolysyncTree *tree, cstring *source_path, cstring *target_path) {
    if (tree->entry_count >= tree->entry_capacity) {
        tree->entry_capacity *= 2;
        tree->entries = polysync_memory_reallocate(tree->entries, sizeof(PolysyncTreeEntry) * tree->entry_capacity);
    }

    PolysyncTreeEntry *tree_entry = &tree->entries[tree->entry_count++];
    tree_entry->source_path = polysync_string_copy(source_path);
    tree_entry->target_path = polysync_string_copy(target_path);
}

PolysyncTreeEntry *polysync_tree_find(PolysyncTree *tree, cstring *source_path) {
    PolysyncTreeEntry *best_tree_entry = null;
    u32 best_tree_entry_length = 0;

    for (u32 entry_index = 0; entry_index < tree->entry_count; entry_index++) {
        PolysyncTreeEntry *tree_entry = &tree->entries[entry_index];
        u32 tree_entry_length = polysync_string_character_count(tree_entry->source_path, '/');

        if (polysync_string_matches_dynamic(source_path, tree_entry->source_path, polysync_string_length(tree_entry->source_path)) && tree_entry_length > best_tree_entry_length) {
            best_tree_entry = tree_entry;
            best_tree_entry_length = tree_entry_length;
        }
    }

    return best_tree_entry;
}

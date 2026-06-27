#include "polysync/build.h"
#include "polysync/transform.h"
#include "polysync/io.h"
#include "polysync/string.h"
#include "polysync/memory.h"
#include "polysync/log.h"

void polysync_build(PolysyncTransformContext *transform_context, cstring *source_path, cstring *build_path) {
    PolysyncDirectory *current_directory = polysync_io_directory_open(source_path);
    PolysyncDirectoryEntry *directory_entry;

    while ((directory_entry = polysync_io_directory_read(current_directory))) {
        string *entry_source_path = polysync_string_concatenate(source_path, "/", directory_entry->name, null);
        string *entry_build_path = polysync_string_concatenate(build_path, "/", directory_entry->name, null);

        if (directory_entry->type == POLYSYNC_DIRECTORY_ENTRY_TYPE_FILE) {
            polysync_transform(transform_context, entry_source_path, entry_build_path);
            polysync_log_info("Built file %s -> %s", entry_source_path, entry_build_path);
        } else if (directory_entry->type == POLYSYNC_DIRECTORY_ENTRY_TYPE_DIRECTORY) {
            polysync_io_directory_create(entry_build_path);
            polysync_build(transform_context, entry_source_path, entry_build_path);
        }

        polysync_memory_free(entry_source_path);
        polysync_memory_free(entry_build_path);
    }
}

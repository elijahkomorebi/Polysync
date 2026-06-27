#ifndef POLYSYNC_IO_H
#define POLYSYNC_IO_H

#include "polysync/types.h"

typedef struct PolysyncDirectory PolysyncDirectory;

typedef enum PolysyncDirectoryEntryType {
    POLYSYNC_DIRECTORY_ENTRY_TYPE_FILE,
    POLYSYNC_DIRECTORY_ENTRY_TYPE_DIRECTORY,
    POLYSYNC_DIRECTORY_ENTRY_TYPE_OTHER
} PolysyncDirectoryEntryType;

typedef struct PolysyncDirectoryEntry {
    cstring *name;
    PolysyncDirectoryEntryType type;
} PolysyncDirectoryEntry;

bool polysync_io_file_exists(cstring *file_path);
string *polysync_io_file_read(cstring *file_path);
bool polysync_io_file_write(cstring *file_path, cstring *file_contents);
u32 polysync_io_file_get_modification_time(cstring *file_path);
bool polysync_io_directory_create(cstring *directory_path);
PolysyncDirectory *polysync_io_directory_open(cstring *directory_path);
void polysync_io_directory_close(PolysyncDirectory *directory);
PolysyncDirectoryEntry *polysync_io_directory_read(PolysyncDirectory *directory);

#endif

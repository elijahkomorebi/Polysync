#ifndef POLYSYNC_WATCHER_H
#define POLYSYNC_WATCHER_H

#include "polysync/types.h"

typedef enum PolysyncFileEventType {
    POLYSYNC_FILE_EVENT_TYPE_CREATED,
    POLYSYNC_FILE_EVENT_TYPE_MODIFIED,
    POLYSYNC_FILE_EVENT_TYPE_DELETED
} PolysyncFileEventType;

typedef struct PolysyncWatchedFile {
    string *file_path;
    u32 modification_time;
    bool seen;
} PolysyncWatchedFile;

typedef struct PolysyncFileEvent {
    PolysyncFileEventType type;
    string *file_path;
} PolysyncFileEvent;

typedef struct PolysyncWatcher {
    cstring *directory_path;
    PolysyncWatchedFile *watched_files;
    u32 watched_file_count;
    u32 watched_file_capacity;
    PolysyncFileEvent *file_events;
    u32 file_event_count;
    u32 file_event_capacity;
    u32 file_event_index;
} PolysyncWatcher;

PolysyncWatcher *polysync_watcher_create(cstring *directory_path);
void polysync_watcher_destroy(PolysyncWatcher *watcher);
void polysync_watcher_update(PolysyncWatcher *watcher);
void polysync_watcher_update_directory(PolysyncWatcher *watcher, cstring *directory_path);
void polysync_watcher_check_for_watched_file_removals(PolysyncWatcher *watcher);
void polysync_watcher_mark_all_watched_files_unseen(PolysyncWatcher *watcher);
PolysyncFileEvent *polysync_watcher_poll_file_event(PolysyncWatcher *watcher);
PolysyncWatchedFile *polysync_watcher_get_watched_file(PolysyncWatcher *watcher, cstring *file_path);
void polysync_watcher_add_watched_file(PolysyncWatcher *watcher, cstring *file_path);
void polysync_watcher_remove_watched_file(PolysyncWatcher *watcher, cstring *file_path);
void polysync_watcher_push_file_event(PolysyncWatcher *watcher, cstring *file_path, PolysyncFileEventType file_event_type);

#endif

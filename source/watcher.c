#include "polysync/watcher.h"
#include "polysync/memory.h"
#include "polysync/io.h"
#include "polysync/error.h"
#include "polysync/string.h"

PolysyncWatcher *polysync_watcher_create(cstring *directory_path) {
    PolysyncWatcher *watcher = polysync_memory_allocate(sizeof(PolysyncWatcher));
    watcher->directory_path = directory_path;
    watcher->watched_files = polysync_memory_allocate(sizeof(PolysyncWatchedFile) * 32);
    watcher->watched_file_count = 0;
    watcher->watched_file_capacity = 32;
    watcher->file_events = polysync_memory_allocate(sizeof(PolysyncFileEvent) * 32);
    watcher->file_event_count = 0;
    watcher->file_event_capacity = 32;
    watcher->file_event_index = 0;

    return watcher;
}

void polysync_watcher_destroy(PolysyncWatcher *watcher) {
    polysync_memory_free(watcher->watched_files);
    polysync_memory_free(watcher->file_events);
    polysync_memory_free(watcher);
}

void polysync_watcher_update(PolysyncWatcher *watcher) {
    polysync_watcher_mark_all_watched_files_unseen(watcher);
    polysync_watcher_update_directory(watcher, watcher->directory_path);
    polysync_watcher_check_for_watched_file_removals(watcher);
}

void polysync_watcher_update_directory(PolysyncWatcher *watcher, cstring *directory_path) {
    PolysyncDirectory *directory = polysync_io_directory_open(directory_path);

    if (!directory) {
        polysync_error("Failed to open directory '%s'", directory_path);
    }

    PolysyncDirectoryEntry *directory_entry;

    while ((directory_entry = polysync_io_directory_read(directory))) {
        string *path = polysync_string_concatenate(directory_path, "/", directory_entry->name, null);

        if (directory_entry->type == POLYSYNC_DIRECTORY_ENTRY_TYPE_DIRECTORY) {
            polysync_watcher_update_directory(watcher, path);
        } else if (directory_entry->type == POLYSYNC_DIRECTORY_ENTRY_TYPE_FILE) {
            PolysyncWatchedFile *watched_file = polysync_watcher_get_watched_file(watcher, path);

            if (!watched_file) {
                polysync_watcher_add_watched_file(watcher, path);
                polysync_watcher_push_file_event(watcher, path, POLYSYNC_FILE_EVENT_TYPE_CREATED);
            } else {
                u64 modification_time = polysync_io_file_get_modification_time(path);
                watched_file->seen = true;

                if (watched_file->modification_time != modification_time) {
                    watched_file->modification_time = modification_time;

                    polysync_watcher_push_file_event(watcher, path, POLYSYNC_FILE_EVENT_TYPE_MODIFIED);
                }
            }
        }

        polysync_memory_free(path);
    }

    polysync_io_directory_close(directory);
}

void polysync_watcher_check_for_watched_file_removals(PolysyncWatcher *watcher) {
    for (u32 watched_file_index = 0; watched_file_index < watcher->watched_file_count; watched_file_index++) {
        PolysyncWatchedFile *watched_file = &watcher->watched_files[watched_file_index];

        if (watched_file->seen) {
            continue;
        }

        polysync_watcher_push_file_event(watcher, watched_file->file_path, POLYSYNC_FILE_EVENT_TYPE_DELETED);
        polysync_watcher_remove_watched_file(watcher, watched_file->file_path);
    }
}

void polysync_watcher_mark_all_watched_files_unseen(PolysyncWatcher *watcher) {
    for (u32 watched_file_index = 0; watched_file_index < watcher->watched_file_count; watched_file_index++) {
        watcher->watched_files[watched_file_index].seen = false;
    }
}

PolysyncFileEvent *polysync_watcher_poll_file_event(PolysyncWatcher *watcher) {
    if (watcher->file_event_index >= watcher->file_event_count) {
        for (u32 file_event_index = 0; file_event_index < watcher->file_event_count; file_event_index++) {
            polysync_memory_free(watcher->file_events[file_event_index].file_path);
        }

        watcher->file_event_index = 0;
        watcher->file_event_count = 0;

        return null;
    }

    return &watcher->file_events[watcher->file_event_index++];
}

PolysyncWatchedFile *polysync_watcher_get_watched_file(PolysyncWatcher *watcher, cstring *file_path) {
    for (u32 watched_file_index = 0; watched_file_index < watcher->watched_file_count; watched_file_index++) {
        PolysyncWatchedFile *watched_file = &watcher->watched_files[watched_file_index];

        if (!polysync_string_matches(watched_file->file_path, file_path)) {
            continue;
        }

        return watched_file;
    }

    return null;
}

void polysync_watcher_add_watched_file(PolysyncWatcher *watcher, cstring *file_path) {
    if (watcher->watched_file_count >= watcher->watched_file_capacity) {
        watcher->watched_file_capacity *= 2;
        watcher->watched_files = polysync_memory_reallocate(watcher->watched_files, sizeof(PolysyncWatchedFile) * watcher->watched_file_capacity);
    }

    PolysyncWatchedFile *watched_file = &watcher->watched_files[watcher->watched_file_count++];
    watched_file->file_path = polysync_string_copy(file_path);
    watched_file->modification_time = polysync_io_file_get_modification_time(file_path);
    watched_file->seen = true;
}

void polysync_watcher_remove_watched_file(PolysyncWatcher *watcher, cstring *file_path) {
    for (u32 watched_file_index = 0; watched_file_index < watcher->watched_file_count; watched_file_index++) {
        PolysyncWatchedFile *watched_file = &watcher->watched_files[watched_file_index];

        if (!polysync_string_matches(watched_file->file_path, file_path)) {
            continue;
        }

        polysync_memory_free(watched_file->file_path);

        u32 last_watched_file_index = --watcher->watched_file_count;
        PolysyncWatchedFile *last_watched_file = &watcher->watched_files[last_watched_file_index];
        watched_file->file_path = last_watched_file->file_path;
        watched_file->seen = last_watched_file->seen;
        watched_file->modification_time = last_watched_file->modification_time;

        break;
    }
}

void polysync_watcher_push_file_event(PolysyncWatcher *watcher, cstring *file_path, PolysyncFileEventType file_event_type) {
    if (watcher->file_event_count >= watcher->file_event_capacity) {
        watcher->file_event_capacity *= 2;
        watcher->file_events = polysync_memory_reallocate(watcher->file_events, sizeof(PolysyncFileEvent) * watcher->file_event_capacity);
    }

    PolysyncFileEvent *file_event = &watcher->file_events[watcher->file_event_count++];
    file_event->type = file_event_type;
    file_event->file_path = polysync_string_copy(file_path);
}

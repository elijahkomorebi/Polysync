#include <signal.h>
#include "polysync/sync.h"
#include "polysync/watcher.h"
#include "polysync/sleep.h"
#include "polysync/transform.h"
#include "polysync/memory.h"
#include "polysync/string.h"
#include "polysync/server.h"
#include "polysync/io.h"
#include "polysync/error.h"
#include "polysync/log.h"

static u32 SYNC_UPDATE_RATE = 1000;
static bool running = true;

static void polysync_signal_handler(i32 _) {
    running = false;
}

void polysync_sync(void) {
    signal(SIGINT, polysync_signal_handler);
    polysync_socket_initialize();

    PolysyncServer *server = polysync_server_create();
    PolysyncTransformContext *transform_context = polysync_transform_context_create();
    PolysyncWatcher *watcher = polysync_watcher_create("build");

    while (running) {
        polysync_watcher_update(watcher);

        PolysyncFileEvent *file_event = null;

        while ((file_event = polysync_watcher_poll_file_event(watcher))) {
            if (polysync_string_ends_with(file_event->file_path, ".meta")) {
                continue;
            }

            PolysyncScriptType script_type = polysync_sync_get_script_type(file_event->file_path);
            string *stripped_file_path = polysync_sync_strip_suffix(file_event->file_path);
            string *tree_file_path = polysync_transform_path_tree(transform_context, stripped_file_path);
            string *module_file_path = polysync_transform_path_module(tree_file_path);

            if (file_event->type == POLYSYNC_FILE_EVENT_TYPE_CREATED) {
                string *file_contents = polysync_io_file_read(file_event->file_path);

                if (!file_contents) {
                    polysync_error("Failed to read file '%s'", file_event->file_path);
                }

                PolysyncServerEventCreateScript server_event_create_script;
                server_event_create_script.event.type = POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT;
                server_event_create_script.script_type = script_type;
                server_event_create_script.script_path = module_file_path;
                server_event_create_script.script_contents = file_contents;
                server_event_create_script.file_path = file_event->file_path;

                polysync_server_push_server_event(server, (PolysyncServerEvent *)&server_event_create_script);
                polysync_memory_free(file_contents);
                polysync_log_info("Create file '%s'", file_event->file_path);
            } else if (file_event->type == POLYSYNC_FILE_EVENT_TYPE_DELETED) {
                PolysyncServerEventDeleteScript server_event_delete_script;
                server_event_delete_script.event.type = POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT;
                server_event_delete_script.script_path = module_file_path;

                polysync_server_push_server_event(server, (PolysyncServerEvent *)&server_event_delete_script);
                polysync_log_info("Delete file '%s'", file_event->file_path);
            } else if (file_event->type == POLYSYNC_FILE_EVENT_TYPE_MODIFIED) {
                string *file_contents = polysync_io_file_read(file_event->file_path);

                if (!file_contents) {
                    polysync_error("Failed to read file '%s'", file_event->file_path);
                }

                PolysyncServerEventModifyScript server_event_modify_script;
                server_event_modify_script.event.type = POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT;
                server_event_modify_script.script_path = module_file_path;
                server_event_modify_script.script_contents = file_contents;

                polysync_server_push_server_event(server, (PolysyncServerEvent *)&server_event_modify_script);
                polysync_memory_free(file_contents);
                polysync_log_info("Modify file '%s'", file_event->file_path);
            }

            polysync_memory_free(stripped_file_path);
            polysync_memory_free(module_file_path);
            polysync_memory_free(tree_file_path);
        }

        polysync_server_update(server);
        polysync_sleep(SYNC_UPDATE_RATE);
    }

    polysync_watcher_destroy(watcher);
    polysync_transform_context_destroy(transform_context);
    polysync_server_destroy(server);
    polysync_socket_shutdown();
}

PolysyncScriptType polysync_sync_get_script_type(cstring *file_path) {
    if (polysync_string_ends_with(file_path, ".server.luau")) {
        return POLYSYNC_SCRIPT_TYPE_SERVER;
    } else if (polysync_string_ends_with(file_path, ".client.luau")) {
        return POLYSYNC_SCRIPT_TYPE_CLIENT;
    }

    return POLYSYNC_SCRIPT_TYPE_MODULE;
}

string *polysync_sync_strip_suffix(cstring *file_path) {
    if (polysync_string_ends_with(file_path, ".server.luau") || polysync_string_ends_with(file_path, ".client.luau")) {
        return polysync_string_copy_dynamic(file_path, polysync_string_length(file_path) - 12);
    }

    return polysync_string_copy_dynamic(file_path, polysync_string_length(file_path) - 5);
}

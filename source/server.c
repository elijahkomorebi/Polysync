#include <cJSON.h>
#include "polysync/server.h"
#include "polysync/memory.h"
#include "polysync/string.h"

static const u16 DEFAULT_SOCKET_PORT = 37630;

PolysyncServer *polysync_server_create() {
    PolysyncServer *server = polysync_memory_allocate(sizeof(PolysyncServer));
    server->socket = polysync_socket_create(DEFAULT_SOCKET_PORT);
    server->events = polysync_memory_allocate(sizeof(PolysyncServerEvent *) * 32);
    server->event_count = 0;
    server->event_capacity = 32;

    return server;
}

void polysync_server_destroy(PolysyncServer *server) {
    polysync_server_clear_server_events(server);
    polysync_socket_destroy(server->socket);
    polysync_memory_free(server->events);
    polysync_memory_free(server);
}

void polysync_server_update(PolysyncServer *server) {
    PolysyncSocket *socket = polysync_socket_accept(server->socket);

    if (!socket) {
        return;
    }

    char buffer[4096];
    i32 received = polysync_socket_receive(socket, buffer, sizeof(buffer) - 1);

    if (received <= 0) {
        polysync_socket_destroy(socket);

        return;
    }

    buffer[received] = 0;
    cJSON *body_array_object = cJSON_CreateArray();

    for (u32 server_event_index = 0; server_event_index < server->event_count; server_event_index++) {
        PolysyncServerEvent *server_event = server->events[server_event_index];
        cJSON *event_object = cJSON_CreateObject();

        if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT) {
            PolysyncServerEventCreateScript *server_event_create_script = (PolysyncServerEventCreateScript *)server_event;

            cJSON_AddStringToObject(event_object, "type", "createScript");
            cJSON_AddStringToObject(event_object, "scriptPath", server_event_create_script->script_path);
            cJSON_AddNumberToObject(event_object, "scriptType", server_event_create_script->script_type);
            cJSON_AddStringToObject(event_object, "scriptContents", server_event_create_script->script_contents);
            cJSON_AddStringToObject(event_object, "filePath", server_event_create_script->file_path);
        } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT) {
            PolysyncServerEventDeleteScript *server_event_delete_script = (PolysyncServerEventDeleteScript *)server_event;

            cJSON_AddStringToObject(event_object, "type", "deleteScript");
            cJSON_AddStringToObject(event_object, "scriptPath", server_event_delete_script->script_path);
        } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT) {
            PolysyncServerEventModifyScript *server_event_modify_script = (PolysyncServerEventModifyScript *)server_event;

            cJSON_AddStringToObject(event_object, "type", "modifyScript");
            cJSON_AddStringToObject(event_object, "scriptPath", server_event_modify_script->script_path);
            cJSON_AddStringToObject(event_object, "scriptContents", server_event_modify_script->script_contents);
        }

        cJSON_AddItemToArray(body_array_object, event_object);
    }

    PolysyncStringBuilder *header_string_builder = polysync_string_builder_create();
    string *body = cJSON_PrintUnformatted(body_array_object);

    polysync_string_builder_append_format(
        header_string_builder,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n",
        polysync_string_length(body)
    );

    string *header = polysync_string_builder_get(header_string_builder);

    polysync_socket_send(socket, header, polysync_string_length(header));
    polysync_socket_send(socket, body, polysync_string_length(body));
    cJSON_Delete(body_array_object);
    polysync_string_builder_destroy(header_string_builder);
    polysync_memory_free(body);
    polysync_memory_free(header);
    polysync_server_clear_server_events(server);
    polysync_socket_destroy(socket);
}

PolysyncServerEvent *polysync_server_poll_server_event(PolysyncServer *server) {
    if (server->event_count == 0) {
        return null;
    }

    PolysyncServerEvent *server_event = server->events[0];
    server->event_count--;

    for (u32 server_event_index = 0; server_event_index < server->event_count; server_event_index++) {
        server->events[server_event_index] = server->events[server_event_index + 1];
    }

    return server_event;
}

void polysync_server_push_server_event(PolysyncServer *server, PolysyncServerEvent *server_event) {
    if (server->event_count >= server->event_capacity) {
        server->event_capacity *= 2;
        server->events = polysync_memory_reallocate(server->events, sizeof(PolysyncServerEvent *) * server->event_capacity);
    }

    PolysyncServerEvent *new_server_event = null;

    if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT) {
        PolysyncServerEventCreateScript *server_event_create_script = (PolysyncServerEventCreateScript *)server_event;
        PolysyncServerEventCreateScript *new_server_event_create_script = polysync_memory_allocate(sizeof(PolysyncServerEventCreateScript));
        new_server_event_create_script->event.type = POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT;
        new_server_event_create_script->script_type = server_event_create_script->script_type;
        new_server_event_create_script->script_path = polysync_string_copy(server_event_create_script->script_path);
        new_server_event_create_script->script_contents = polysync_string_copy(server_event_create_script->script_contents);
        new_server_event_create_script->file_path = polysync_string_copy(server_event_create_script->file_path);
        new_server_event = (PolysyncServerEvent *)new_server_event_create_script;
    } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT) {
        PolysyncServerEventDeleteScript *server_event_delete_script = (PolysyncServerEventDeleteScript *)server_event;
        PolysyncServerEventDeleteScript *new_server_event_delete_script = polysync_memory_allocate(sizeof(PolysyncServerEventDeleteScript));
        new_server_event_delete_script->event.type = POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT;
        new_server_event_delete_script->script_path = polysync_string_copy(server_event_delete_script->script_path);
        new_server_event = (PolysyncServerEvent *)new_server_event_delete_script;
    } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT) {
        PolysyncServerEventModifyScript *server_event_modify_script = (PolysyncServerEventModifyScript *)server_event;
        PolysyncServerEventModifyScript *new_server_event_modify_script = polysync_memory_allocate(sizeof(PolysyncServerEventModifyScript));
        new_server_event_modify_script->event.type = POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT;
        new_server_event_modify_script->script_path = polysync_string_copy(server_event_modify_script->script_path);
        new_server_event_modify_script->script_contents = polysync_string_copy(server_event_modify_script->script_contents);
        new_server_event = (PolysyncServerEvent *)new_server_event_modify_script;
    }

    server->events[server->event_count++] = new_server_event;
}

void polysync_server_destroy_server_event(PolysyncServerEvent *server_event) {
    if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT) {
        PolysyncServerEventCreateScript *server_event_create_script = (PolysyncServerEventCreateScript *)server_event;

        polysync_memory_free(server_event_create_script->script_path);
        polysync_memory_free(server_event_create_script->script_contents);
        polysync_memory_free(server_event_create_script->file_path);
    } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT) {
        PolysyncServerEventDeleteScript *server_event_delete_script = (PolysyncServerEventDeleteScript *)server_event;

        polysync_memory_free(server_event_delete_script->script_path);
    } else if (server_event->type == POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT) {
        PolysyncServerEventModifyScript *server_event_modify_script = (PolysyncServerEventModifyScript *)server_event;

        polysync_memory_free(server_event_modify_script->script_path);
        polysync_memory_free(server_event_modify_script->script_contents);
    }

    polysync_memory_free(server_event);
}

void polysync_server_clear_server_events(PolysyncServer *server) {
    for (u32 server_event_index = 0; server_event_index < server->event_count; server_event_index++) {
        polysync_server_destroy_server_event(server->events[server_event_index]);
    }

    server->event_count = 0;
}

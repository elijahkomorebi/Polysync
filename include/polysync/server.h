#ifndef POLYSYNC_SERVER_H
#define POLYSYNC_SERVER_H

#include "polysync/types.h"
#include "polysync/socket.h"

typedef enum PolysyncServerEventType {
    POLYSYNC_SERVER_EVENT_TYPE_CREATE_SCRIPT,
    POLYSYNC_SERVER_EVENT_TYPE_DELETE_SCRIPT,
    POLYSYNC_SERVER_EVENT_TYPE_MODIFY_SCRIPT
} PolysyncServerEventType;

typedef struct PolysyncServerEvent {
    PolysyncServerEventType type;
} PolysyncServerEvent;

typedef struct PolysyncServerEventCreateScript {
    PolysyncServerEvent event;
    u32 script_type;
    string *script_path;
    string *script_contents;
    string *file_path;
} PolysyncServerEventCreateScript;

typedef struct PolysyncServerEventDeleteScript {
    PolysyncServerEvent event;
    string *script_path;
} PolysyncServerEventDeleteScript;

typedef struct PolysyncServerEventModifyScript {
    PolysyncServerEvent event;
    string *script_path;
    string *script_contents;
} PolysyncServerEventModifyScript;

typedef struct PolysyncServer {
    PolysyncSocket *socket;
    PolysyncServerEvent **events;
    u32 event_count;
    u32 event_capacity;
} PolysyncServer;

PolysyncServer *polysync_server_create(void);
void polysync_server_destroy(PolysyncServer *server);
void polysync_server_update(PolysyncServer *server);
PolysyncServerEvent *polysync_server_poll_server_event(PolysyncServer *server);
void polysync_server_push_server_event(PolysyncServer *server, PolysyncServerEvent *server_event);
void polysync_server_destroy_server_event(PolysyncServerEvent *server_event); 
void polysync_server_clear_server_events(PolysyncServer *server);

#endif

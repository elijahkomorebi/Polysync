#ifndef POLYSYNC_SOCKET_H
#define POLYSYNC_SOCKET_H

#include "polysync/types.h"

typedef struct PolysyncSocket PolysyncSocket;

void polysync_socket_initialize(void);
void polysync_socket_shutdown(void);
PolysyncSocket *polysync_socket_create(u16 port);
void polysync_socket_destroy(PolysyncSocket *p_socket);
PolysyncSocket *polysync_socket_accept(PolysyncSocket *p_socket);
i32 polysync_socket_send(PolysyncSocket *p_socket, const void *buffer, u32 buffer_size);
i32 polysync_socket_receive(PolysyncSocket *p_socket, void *buffer, u32 buffer_size);

#endif

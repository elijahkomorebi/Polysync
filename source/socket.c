#include "polysync/socket.h"
#include "polysync/memory.h"
#include "polysync/error.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>

struct PolysyncSocket {
    SOCKET w_handle;
};
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

struct PolysyncSocket {
    i32 l_handle;
};
#endif

void polysync_socket_initialize(void) {
#ifdef _WIN32
    WSADATA w_wsa_data;

    WSAStartup(MAKEWORD(2, 2), &w_wsa_data);
#endif
}

void polysync_socket_shutdown(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

PolysyncSocket *polysync_socket_create(u16 port) {
    PolysyncSocket *p_socket = polysync_memory_allocate(sizeof(PolysyncSocket));
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

#ifdef _WIN32
    p_socket->w_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (p_socket->w_handle == INVALID_SOCKET) {
        polysync_error("Failed to create socket");
    }

    if (bind(p_socket->w_handle, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        polysync_error("Failed to bind socket");
    }

    u_long w_mode = 1;

    if(listen(p_socket->w_handle, 16) == SOCKET_ERROR) {
        polysync_error("Failed to listen on socket");
    }

    if (ioctlsocket(p_socket->w_handle, FIONBIO, &w_mode) == SOCKET_ERROR) {
        polysync_error("Failed to set socket non-blocking");
    }
#else
    p_socket->l_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (p_socket->l_handle < 0) {
        polysync_error("Failed to create socket");
    }

    if (bind(p_socket->l_handle, (struct sockaddr *)&address, sizeof(address))) {
        polysync_error("Failed to bind socket");
    }

    if (listen(p_socket->l_handle, 16) < 0) {
        polysync_error("Failed to listn on socket");
    }

    if (fcntl(p_socket->l_handle, F_SETFL, O_NONBLOCK) < 0) {
        polysync_error("Failed to set socket non-blocking");
    }
#endif

    return p_socket;
}

void polysync_socket_destroy(PolysyncSocket *p_socket) {
#ifdef _WIN32
    closesocket(p_socket->w_handle);
#else
    close(p_socket->l_handle);
#endif

    polysync_memory_free(p_socket);
}

PolysyncSocket *polysync_socket_accept(PolysyncSocket *p_socket) {
    PolysyncSocket *p_new_socket = polysync_memory_allocate(sizeof(PolysyncSocket));
    
#ifdef _WIN32
    p_new_socket->w_handle = accept(p_socket->w_handle, null, null);

    if (p_new_socket->w_handle == INVALID_SOCKET) {
        return null;
    }
#else
    p_new_socket->l_handle = accept(p_socket->l_handle, null, null);

    if (p_new_socket->l_handle < 0) {
        return null;
    }
#endif

    return p_new_socket;
}

i32 polysync_socket_send(PolysyncSocket *p_socket, const void *buffer, u32 buffer_size) {
#ifdef _WIN32
    return send(p_socket->w_handle, buffer, buffer_size, 0);
#else
    return send(p_socket->l_handle, buffer, buffer_size, 0);
#endif
}

i32 polysync_socket_receive(PolysyncSocket *p_socket, void *buffer, u32 buffer_size) {
#ifdef _WIN32
    return recv(p_socket->w_handle, buffer, buffer_size, 0);
#else
    return recv(p_socket->l_handle, buffer, buffer_size, 0);
#endif
}

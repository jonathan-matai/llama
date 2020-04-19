/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llserversocket.h
 Network Socket for the Server
*/

#pragma once

#include "../llevent.h"

namespace llama
{
    class ServerSocket_T
    {
    public:

        virtual ~ServerSocket_T() = default;
    };

    typedef std::shared_ptr<ServerSocket_T> ServerSocket;

    // Create a new ClientSocket Object
    // @eventBus:   The Event Bus the socket connects to
    // @port:       The port that the server should use
    // @ipv4:       By default, servers are IPv6, when ipv4 is set to true, the Server uses IPv4 and only IPv4 clients can connect
    // @return:     The newly created ClientSocket Object
    LLAMA_API ServerSocket createServerSocket(EventBus eventBus, uint16_t port, bool ipv4 = false);
}
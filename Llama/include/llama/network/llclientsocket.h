/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llclientsocket.h
 Network Socket for the Client
*/

#pragma once

#include "../event/llevent.h"

namespace llama
{
    class ClientSocket_T
    {
    public:

        virtual ~ClientSocket_T() = default;

        virtual bool connectServer(std::string_view hostname, std::string_view servicename, bool useIPv4 = false) = 0;
    };

    typedef std::shared_ptr<ClientSocket_T> ClientSocket;


    // Create a new ClientSocket Object
    // @node:       The Event Bus or filter the socket connects to
    // @return:     The newly created ClientSocket Object
    LLAMA_API ClientSocket createClientSocket(EventNode eventBus);
}
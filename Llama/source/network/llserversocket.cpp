#include "llcore.h"
#include "network/llserversocket.h"

#include <asio.hpp>


namespace llama
{
    class ServerSocket_I : public ServerSocket_T
    {
    public:

        ServerSocket_I(EventNode node, uint16_t port, bool ipv4);

        ~ServerSocket_I() override;

        EventDispatchState dispatchEvent(Event* e);



    private:

        void receiveFromClients();

        std::unique_ptr<std::thread> m_receiveThread;

        EventNode m_node;
        EventDispatchFunction m_dispatcher;

        asio::ip::udp::endpoint t_client;


        asio::io_context m_context;
        bool m_ipv4;
        asio::ip::udp::socket m_socket;
    };
}

llama::ServerSocket_I::ServerSocket_I(EventNode node, uint16_t port, bool ipv4) :
    m_ipv4(ipv4),
    m_socket(m_context, asio::ip::udp::endpoint(ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), port)),
    m_dispatcher(node, this, &ServerSocket_I::dispatchEvent),
    m_node(node)
{
    std::vector<char> buffer(1024);
    asio::ip::udp::endpoint endpoint;

    asio::error_code error;
    m_socket.receive_from(asio::buffer(buffer, 1024), endpoint, 0, error);

    if (error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Reveiving error %x ocured: %s\n", error.value(), error.message().c_str());
        return;
    }


    if (memcmp(buffer.data(), "PING", 4) == 0)
        logfile()->print(Colors::GREEN, "Response from %s:%d : %s",
                         endpoint.address().to_string().c_str(),
                         endpoint.port(),
                         &buffer[4]);
    else
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Oops, what the server sent back doesn't look like a ping");

    std::string msg = "PINGHello Client!";

    m_socket.send_to(asio::buffer(msg.data(), msg.size() + 1), endpoint);

    t_client = endpoint;

    m_receiveThread = std::make_unique<std::thread>(&ServerSocket_I::receiveFromClients, this);
}

llama::ServerSocket_I::~ServerSocket_I()
{
    // Don't close socket --> wait for thread recieve close event
    m_receiveThread->join();
}

llama::EventDispatchState llama::ServerSocket_I::dispatchEvent(Event* e)
{
    asio::mutable_buffer buf(e, e->m_size);

    asio::error_code error;

    m_socket.send_to(buf, t_client, 0, error);

    if (error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Sending error %x ocured: %s\n", error.value(), error.message().c_str());
        return EventDispatchState::IGNORED;
    }

    return EventDispatchState::PROCESSED;
}

void llama::ServerSocket_I::receiveFromClients()
{
    while (true)
    {
        std::vector<char> buffer(1024);
        asio::ip::udp::endpoint endpoint;
        asio::error_code error;

        m_socket.receive_from(asio::buffer(buffer), endpoint, 0, error);

        if (error)
        {
            logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Receiving error %x ocured: %s\n", error.value(), error.message().c_str());
            return;
        }

        Event* event = reinterpret_cast<Event*>(buffer.data());

        logfile()->print(Colors::WHITE, "Recieved an Event of Type %d from %s:%d", event->m_type, endpoint.address().to_string().c_str(), endpoint.port());


        if (event->m_type == InternalEventType::CLOSE_APPLICATION)
            return;

        Event* copy = reinterpret_cast<Event*>(::operator new(event->m_size));
        memcpy(copy, event, event->m_size);
        copy->m_creator = this;

        m_node->handleEvent(std::unique_ptr<Event>(copy));
    }
}

llama::ServerSocket llama::createServerSocket(EventNode node, uint16_t port, bool ipv4)
{
    try
    {
        auto a = std::make_shared<ServerSocket_I>(node, port, ipv4);
        return a;
    }
    catch (asio::system_error error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "An error occured upon creating a Server Socket: %s\n", error.what());
        return nullptr;
    }
}

#include "llcore.h"
#include "network/llclientsocket.h"

#include <asio.hpp>

namespace llama
{
    class ClientSocket_I : public ClientSocket_T
    {
    public:

        ClientSocket_I(EventNode node);

        ~ClientSocket_I() override;

        bool connectServer(std::string_view hostname, std::string_view servicename, bool useIPv4) override;

        EventDispatchState dispatchEvent(Event* e);

    private:

        void receiveFromServer();

        std::unique_ptr<std::thread> m_receiveThread;

        EventNode m_node;
        EventDispatchFunction m_dispatcher;

        asio::io_context m_context;
        asio::ip::udp::resolver m_resolver;
        asio::ip::udp::socket m_socket;
        bool m_ipv4;

        asio::ip::udp::endpoint m_serverEndpoint;

    };
}

llama::ClientSocket_I::ClientSocket_I(EventNode node) :
    m_socket(m_context),
    m_resolver(m_context),
    m_dispatcher(node, this, &ClientSocket_I::dispatchEvent),
    m_node(node)
{
}

llama::ClientSocket_I::~ClientSocket_I()
{
    m_socket.close();
    m_receiveThread->join();
}

bool llama::ClientSocket_I::connectServer(std::string_view hostname, std::string_view servicename, bool useIPv4)
{
    m_ipv4 = useIPv4;

    asio::error_code error;
    auto results = m_resolver.resolve(m_ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), hostname, servicename, error);

    if (error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Resolving error %x ocured: %s\n", error.value(), error.message().c_str());
        return false;
    }

    m_serverEndpoint = *results.begin();

    if (m_socket.is_open())
    {
        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Disconnecting old server before connecting");
        m_socket.close();
    }

    m_socket.open(m_ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), error);

    std::string msg = "PINGHello Server!";

    m_socket.send_to(asio::buffer(msg.data(), msg.size() + 1), m_serverEndpoint, 0, error);

    if (error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Sending error %x ocured: %s\n", error.value(), error.message().c_str());
        return false;
    }

    std::future<void> future = std::async(std::launch::async, [&]()
    {
        std::vector<char> buffer(1024);
        asio::ip::udp::endpoint endpoint;

        asio::error_code error;
        m_socket.receive_from(asio::buffer(buffer, 1024), endpoint, 0, error);

        if(error)
            logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Reveiving error %x ocured: %s\n", error.value(), error.message().c_str());

        if (memcmp(buffer.data(), "PING", 4) == 0)
            logfile()->print(Colors::GREEN, "Response from %s:%d : %s",
                             endpoint.address().to_string().c_str(),
                             endpoint.port(),
                             &buffer[4]);
        else
            logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Oops, what the server sent back doesn't look like a ping");

    });

    auto status = future.wait_for(std::chrono::milliseconds(1000));

    if (status == std::future_status::ready)
    {
        m_receiveThread = std::make_unique<std::thread>(&ClientSocket_I::receiveFromServer, this);

        return true;
    }

    m_socket.close();
    printf("Timeout!\n");
    return false;
}

llama::EventDispatchState llama::ClientSocket_I::dispatchEvent(Event* e)
{
    asio::mutable_buffer buf(e, e->m_size);

    asio::error_code error;

    m_socket.send_to(buf, m_serverEndpoint, 0, error);

    if (error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Sending error %x ocured: %s\n", error.value(), error.message().c_str());
        return EventDispatchState::IGNORED;
    }

    return EventDispatchState::PROCESSED;
}

void llama::ClientSocket_I::receiveFromServer()
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

        if (event->m_type == InternalEventType::CLOSE_APPLICATION)
            return;

        Event* copy = reinterpret_cast<Event*>(::operator new(event->m_size));
        memcpy_s(copy, event->m_size, event, event->m_size);
        copy->m_creator = this;

        m_node->handleEvent(std::unique_ptr<Event>(copy));
    }
}

llama::ClientSocket llama::createClientSocket(EventNode node)
{
    try
    {
        auto a = std::make_shared<ClientSocket_I>(node);
        return a;
    }
    catch (asio::system_error error)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "An error occured upon creating a Client Socket: %s\n", error.what());
        return nullptr;
    }
}

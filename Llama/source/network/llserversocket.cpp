#include "llcore.h"
#include "network/llserversocket.h"

#include <asio.hpp>


namespace llama
{
    class ServerSocket_I : public ServerSocket_T
    {
    public:

        ServerSocket_I(uint16_t port, bool ipv4);

    private:

        asio::io_context m_context;
        bool m_ipv4;
        asio::ip::udp::socket m_socket;
    };

    ServerSocket_I::ServerSocket_I(uint16_t port, bool ipv4) :
        m_ipv4(ipv4),
        m_socket(m_context, asio::ip::udp::endpoint(ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), port))
    {
        std::vector<char> buffer(1024);
        asio::ip::udp::endpoint endpoint;

        asio::error_code error;
        m_socket.receive_from(asio::buffer(buffer, 1024), endpoint, 0, error);

        if (error)
        {
            printf("Reveiving error %x ocured: %s\n", error.value(), error.message().c_str());
            return;
        }
        

        printf("Message from %s:%d : %s", endpoint.address().to_string().c_str(), endpoint.port(), buffer.data());

        std::string msg = "Hello Client!";

        m_socket.send_to(asio::buffer(msg.data(), msg.size() + 1), endpoint);
    }
}

llama::ServerSocket llama::createServerSocket(EventNode node, uint16_t port, bool ipv4)
{
    try
    {
        auto a = std::make_shared<ServerSocket_I>(port, ipv4);
        /// eventBus->addDispatcher(a, &ServerSocket_I::function)
        return a;
    }
    catch (asio::system_error error)
    {
        printf("Error occured: %s\n", error.what());
        return nullptr;
    }
}

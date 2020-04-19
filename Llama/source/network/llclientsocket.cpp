#include "llcore.h"
#include "network/llclientsocket.h"

#include <asio.hpp>

namespace llama
{
    class ClientSocket_I : public ClientSocket_T
    {
    public:

        ClientSocket_I();

    private:

        bool connectServer(std::string_view hostname, std::string_view servicename, bool useIPv4) override;

    private:

        asio::io_context m_context;
        asio::ip::udp::resolver m_resolver;
        asio::ip::udp::socket m_socket;
        bool m_ipv4;

        asio::ip::udp::endpoint m_serverEndpoint;

    };


    ClientSocket_I::ClientSocket_I() :
        m_socket(m_context),
        m_resolver(m_context)
    {
    }
}

bool llama::ClientSocket_I::connectServer(std::string_view hostname, std::string_view servicename, bool useIPv4)
{
    m_ipv4 = useIPv4;

    asio::error_code error;
    auto results = m_resolver.resolve(m_ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), hostname, servicename, error);

    if (error)
    {
        printf("Resolving error %x ocured: %s\n", error.value(), error.message().c_str());
        return false;
    }

    m_serverEndpoint = *results.begin();

    if (m_socket.is_open())
    {
        printf("Disconnecting old server before connecting");
        m_socket.close();
    }

    m_socket.open(m_ipv4 ? asio::ip::udp::v4() : asio::ip::udp::v6(), error);

    std::string msg = "Hello Server!";

    m_socket.send_to(asio::buffer(msg.data(), msg.size() + 1), m_serverEndpoint, 0, error);

    if (error)
    {
        printf("Error %x ocured: %s\n", error.value(), error.message().c_str());
        return false;
    }

    std::future<void> future = std::async(std::launch::async, [&]()
    {
        std::vector<char> buffer(1024);
        asio::ip::udp::endpoint endpoint;

        asio::error_code error;
        m_socket.receive_from(asio::buffer(buffer, 1024), endpoint, 0, error);

        if(error)
            printf("Reveiving error %x ocured: %s\n", error.value(), error.message().c_str());

    });

    auto status = future.wait_for(std::chrono::milliseconds(1000));

    if(status == std::future_status::ready)
        return true;


    m_socket.close();
    printf("Timeout!\n");
    return false;
}

llama::ClientSocket llama::createClientSocket()
{
    try
    {
        return std::make_shared<ClientSocket_I>();
    }
    catch (asio::system_error error)
    {
        printf("Error occured: %s\n", error.what());
        return nullptr;
    }
}
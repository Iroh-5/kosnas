#include <boost/asio/ip/tcp.hpp>

void ProcessConnection(tcp::socket& sock)
{
    while (true)
    {
        std::string commandBuf;

        boost::system::error_code ec;

        sock.read_some(boost::asio::buffer(commandBuf), ec);

        if (command == "Register User")
        {
            std::string usernameBuf;
            std::string passwordBuf;

            sock.read_some(boost::asio::buffer(usernameBuf), ec);
            sock.read_some(boost::asio::buffer(passwordBuf), ec);

            RegisterUserImpl(usernameBuf, passwordBuf);
        }
        else if (command == "Authenticate User")
        {
            std::string usernameBuf;
            std::string passwordBuf;

            sock.read_some(boost::asio::buffer(usernameBuf), ec);
            sock.read_some(boost::asio::buffer(passwordBuf), ec);

            AuthenticateUserImpl(usernameBuf, passwordBuf);
        }
        else if (command == "Read File")
        {
            std::string filenameBuf;

            sock.read_some(boost::asio::buffer(filenameBuf), ec);

            ReadFileImpl(sock, filenameBuf);
        }
        else if (command == "Write File")
        {
            std::string filenameBuf;

            sock.read_some(boost::asio::buffer(filenameBuf), ec);

            WriteFileImpl(sock, filenameBuf);
        }
        else
        {
            sock.write_some(boost::asio::buffer(std::string{"Unknown command"}), ec);
        }
    }
}

int main()
{
    if (!configure_net_iface(
        DEFAULT_INTERFACE,
        DEFAULT_ADDR,
        DEFAULT_MASK,
        DEFAULT_GATEWAY,
        DEFAULT_MTU))
    {
        ERROR(GATEWAY_ENTITY, "Could not initialize network");
        return EXIT_FAILURE;
    }

    try
    {
        boost::asio::io_context io;
        tcp::acceptor acceptro{io, tcp::endpoint{tcp::v4(), 13}};

        while (true)
        {
            tcp::socket sock{io};
            acceptor.accept(sock);

            ProcessConnection(sock);
        }
    }
    catch (const std::exception& e)
    {
        ERROR(GATEWAY_ENTITY, "Fatal error during runtime: %s", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

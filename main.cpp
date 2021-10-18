#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>

using namespace boost;

int main()
{
    asio::io_context sapIoContext{};

    asio::ip::udp::endpoint sapEndpoint
    {
        asio::ip::make_address_v4("0.0.0.0"),
        9875
    };
    asio::ip::udp::socket   socket{ sapIoContext };

    socket.open(sapEndpoint.protocol());
    socket.set_option(asio::ip::udp::socket::reuse_address(true));
    socket.bind(sapEndpoint);

    socket.set_option
    (
        asio::ip::multicast::join_group
            (asio::ip::make_address_v4("239.255.255.255"))
    );

    std::array<char, 1500> buffer{};

    socket.receive_from(asio::buffer(buffer), sapEndpoint);

    return EXIT_SUCCESS;
}
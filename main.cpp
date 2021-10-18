#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include "libsdptransform/sdptransform.hpp"

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

    char buffer[1500]{};

    socket.receive_from(asio::buffer(buffer), sapEndpoint);

    json sdpJson = sdptransform::parse(&buffer[24]);

    if(sdpJson.find("name") != sdpJson.end())
    {
        std::cout << sdpJson.at("name") <<"\n";
    }
    else
    {
        std::cout << "NAY\n";
    }

    return EXIT_SUCCESS;
}
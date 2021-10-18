#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

#include <iostream>
#include "libsdptransform/sdptransform.hpp"
#include "sap.hpp"



int main()
{
    asio::io_context sapIoContext{};
    SAP::Receiver receiver{ sapIoContext };

    SAP::packet_buffer_t buffer{ receiver.syncReceive() };

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
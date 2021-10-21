#define BOOST_ASIO_NO_DEPRECATED
#include "boost/asio.hpp"
#include "boost/bind/bind.hpp"
using namespace boost;

#include <iostream>
#include "sdptransform.hpp"
#include "sap.hpp"
#include "sdp.hpp"
#include "lanio-version.h"



int main()
{
    std::cout << "Version : "
        << LanioSAP_VERSION_MAJOR << "." << LanioSAP_VERSION_MINOR
        << std::endl;

    asio::io_context sapIoContext{};
    SAP::Receiver receiver{ sapIoContext };

    try
    {
        SAP::Parser sapParser{ receiver.syncReceive() };
        SDP::Parser sdpParser{ sapParser.getSdp() };

        std::cout << "=== Full SDP ===" << "\n";
        std::cout << sapParser.getSdp();
        std::cout << "=== Full SDP ===" << "\n";
        std::cout << "Session name :\t" << sdpParser.getSessionName() << std::endl;
    }
    catch(const char* e)
    {
        std::cerr << "\nError : " << e << "\n";
    }

    return EXIT_SUCCESS;
}
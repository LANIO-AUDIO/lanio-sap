#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

#include <iostream>
#include "sdptransform.hpp"
#include "sap.hpp"
#include "sdp.hpp"
#include "lanio-version.h"
#include "printers.hpp"



int main()
{
    printVersion();

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
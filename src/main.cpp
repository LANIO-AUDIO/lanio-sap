#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

#include <iostream>
#include "sdptransform.hpp"
#include "sap.hpp"
#include "lanio-version.h"



int main()
{
    std::cout << "Version : "
        << LanioSAP_VERSION_MAJOR << "." << LanioSAP_VERSION_MINOR
        << std::endl;

    asio::io_context sapIoContext{};
    SAP::Receiver receiver{ sapIoContext };

    SAP::Parser parser{ receiver.syncReceive() };

    std::cout << parser.getSdp();

    return EXIT_SUCCESS;
}
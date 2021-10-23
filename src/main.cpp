#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

#include <iostream>
#include "sdptransform.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
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
        std::cout << "=== Parsed SDP ===" << "\n";
        std::cout << sdpParser.getJson().dump(4) << "\n";
        std::cout << "=== Parsed SDP ===" << "\n";

        std::cout << "Session name :\t\t"
            << sdpParser.getSessionName() << "\n";
        std::cout << "Stream address :\t"
            << sdpParser.getStreamIp() << ":"
            << sdpParser.getStreamPort() << "\n";
        std::cout << "Origin address :\t"
            << sdpParser.getOriginIp() << "\n";
    }
    catch(const char* e)
    {
        std::cerr << "\nError : " << e << "\n";
    }

    SQLite::Database    db("test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    return EXIT_SUCCESS;
}
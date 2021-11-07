#include <QDir>
#include "debug.hpp"
#include "application.hpp"
#include "sap.hpp"

int main(int argc, char* argv[]) try
{
    DiscoveryApplication app(argc, argv);

    SAP::Receiver receiver
        { QDir::toNativeSeparators(QDir::tempPath() + "/test.db") };

    return app.exec();
}
catch(const std::exception& e)
{
    qFatal(e.what());
}
catch(...)
{
    qFatal("Caught an unhandled exception.");
}
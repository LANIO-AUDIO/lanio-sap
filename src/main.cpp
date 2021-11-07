#include <QDir>
#include "debug.hpp"
#include "application.hpp"
#include "sap.hpp"

int main(int argc, char* argv[]) try
{
    DiscoveryApplication app(argc, argv);

    QString dbPath{ QDir::toNativeSeparators(QDir::tempPath() + "/test.db") };
    qCDebug(sql).noquote().nospace() << "Database path : " << dbPath;
    SAP::Receiver receiver{ dbPath };

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
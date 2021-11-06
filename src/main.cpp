#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include "version.h"
#include "debug.hpp"
#include "sap.hpp"

int main(int argc, char* argv[]) try
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion
        (PROJECT_VERSION_MAJOR "." PROJECT_VERSION_MINOR);

    QCommandLineParser cliParser{};
    cliParser.setApplicationDescription("AES67 SAP Discovery tool");
    cliParser.addHelpOption();
    cliParser.addVersionOption();

    cliParser.process(app);

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
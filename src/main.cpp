#include <QCoreApplication>
#include <QCommandLineParser>
#include "sap.hpp"
#include "lanio-version.h"


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

    SAP::Receiver receiver{"/tmp/test.db"};

    return app.exec();
}
catch(const std::exception& e)
{
    qCritical().noquote() << e.what();
    exit(EXIT_FAILURE);
}
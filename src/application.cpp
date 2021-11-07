#include "version.h"
#include "application.hpp"
#include <QCommandLineParser>
#include <QLoggingCategory>

DiscoveryApplication::DiscoveryApplication(int& argc, char** argv)
    : QCoreApplication(argc, argv)
{
    DiscoveryApplication::setApplicationName(PROJECT_NAME);
    DiscoveryApplication::setApplicationVersion
        (PROJECT_VERSION_MAJOR "." PROJECT_VERSION_MINOR);

    parseCommandLine();
}

void DiscoveryApplication::parseCommandLine()
{
    QCommandLineParser cliParser{};

    cliParser.setApplicationDescription("AES67 SAP Discovery tool");
    cliParser.addHelpOption();
    cliParser.addVersionOption();

    cliParser.addOptions
    ({
        {
            { "d", "debug" },
            "Show debug messages"
        }
    });

    cliParser.process(*this);

    if(!cliParser.isSet("debug"))
    {
        QLoggingCategory::setFilterRules("*.debug=false");
    }
}
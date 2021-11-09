#include "version.h"
#include "application.hpp"
#include "sap.hpp"
#include <QCommandLineParser>
#include <QLoggingCategory>

DiscoveryApplication::DiscoveryApplication(int& argc, char** argv)
    : QCoreApplication(argc, argv)
    , m_databasePath{ SAP::defaultDatabasePath }
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
        },
        {
            { "f", "db-file" },
            "Path to the database file\n(default is " + SAP::defaultDatabasePath
                + ")",
            "file",
            SAP::defaultDatabasePath
        }
    });

    cliParser.process(*this);

    if(!cliParser.isSet("debug"))
    {
        QLoggingCategory::setFilterRules("*.debug=false");
    }

    if(cliParser.isSet("db-file"))
    {
        m_databasePath = QDir::toNativeSeparators(cliParser.value("db-file"));
    }
}
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include "sap.hpp"
#include "lanio-version.h"


int main(int argc, char* argv[])
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

    SAP::Receiver receiver{};

    int programReturn{};
    try
    {
        programReturn = app.exec();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
    catch(const char* e)
    {
        std::cerr << "\nError : " << e << "\n";
        exit(EXIT_FAILURE);
    }

    return programReturn;
}
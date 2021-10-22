#include "lanio-version.h"
#include "printers.hpp"

void printVersion()
{
    std::cout << PROJECT_NAME << " version : "
        << LanioSAP_VERSION_MAJOR << "." << LanioSAP_VERSION_MINOR
        << std::endl;
}
#include <QCoreApplication>

class DiscoveryApplication : public QCoreApplication
{
    Q_OBJECT

public:
    DiscoveryApplication(int& argc, char** argv);

private:
    void parseCommandLine();
};
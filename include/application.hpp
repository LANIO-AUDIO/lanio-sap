#include <QCoreApplication>

class DiscoveryApplication : public QCoreApplication
{
    Q_OBJECT

public:
    DiscoveryApplication(int& argc, char** argv);
    inline const QString getDatabasePath() const { return m_databasePath; }

private:
    QString m_databasePath;
    void parseCommandLine();
};
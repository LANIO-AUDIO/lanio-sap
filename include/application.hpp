#include "sap.hpp"
#include <QCoreApplication>
#include <QScopedPointer>

class DiscoveryApplication : public QCoreApplication
{
    Q_OBJECT

public:
    DiscoveryApplication(int& argc, char** argv);
    inline const QString getDatabasePath() const { return m_databasePath; }

private:
    QString                         m_databasePath;
    QScopedPointer<SAP::Receiver>   m_sapReceiver;
    void parseCommandLine();
};
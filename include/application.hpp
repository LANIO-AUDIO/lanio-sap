#include "sap.hpp"
#include <QCoreApplication>
#include <QScopedPointer>

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    #define QTEXTSTREAM_PREFIX
#else
    #define QTEXTSTREAM_PREFIX Qt::
#endif

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
    void validateCliParameters();
};
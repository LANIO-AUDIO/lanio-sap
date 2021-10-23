#include <QtNetwork>

#include "sdptransform.hpp"

namespace SDP
{
    class Parser
    {
    public:
        Parser(const QString& sdp);

        inline QJsonDocument    getJson()           const { return m_parsedSdp; }
        inline QString          getSessionName()    const { return m_sessionName; }
        inline QHostAddress     getStreamIp()       const { return m_streamIp; }
        inline int              getStreamPort()     const { return m_streamPort; }
        inline QHostAddress     getOriginIp()       const { return m_originIp; }

    private:
        QJsonDocument   m_parsedSdp;
        QString         m_sessionName;
        QHostAddress    m_streamIp;
        int             m_streamPort;
        QHostAddress    m_originIp;

        QString         extractSessionName()    const;
        QHostAddress    extractStreamIp()       const;
        int             extractStreamPort()     const;
        QHostAddress    extractOriginIp()       const;
    };
}
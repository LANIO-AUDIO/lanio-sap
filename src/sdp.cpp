#include "sdp.hpp"

namespace SDP
{
    Parser::Parser(const QString& sdp)
    :   m_parsedSdp
        (
            QJsonDocument::fromJson
            (
                sdptransform::parse(sdp.toStdString()).dump().c_str()
            ).object()
        ),
        m_sessionName   { extractSessionName() },
        m_streamIp      { extractStreamIp() },
        m_streamPort    { extractStreamPort() },
        m_originIp      { extractOriginIp() }
    {}

    QString Parser::extractSessionName() const
    {
        QString sessionName{};

        if(m_parsedSdp.contains("name") && m_parsedSdp["name"].isString())
        {
            sessionName = m_parsedSdp["name"].toString();
        }
        else
        {
            throw "Unable to find session name in SDP";
        }

        return sessionName;
    }

    QHostAddress Parser::extractStreamIp()       const
    {
        QHostAddress streamIp{};

        if
        (
               m_parsedSdp.contains("connection")
            && m_parsedSdp["connection"].toObject().contains("ip")
        )
        {
            streamIp.setAddress
            (
                m_parsedSdp["connection"]
                    .toObject()["ip"].toString()
            );
        }
        else
        {
            throw "Unable to find stream IP in SDP";
        }

        return streamIp;
    }

    int         Parser::extractStreamPort()     const
    {
        int streamPort{};

        if
        (
               m_parsedSdp.contains("media")
            && m_parsedSdp["media"].toArray()[0].toObject().contains("port")
        )
        {
            streamPort = m_parsedSdp["media"]
                .toArray()[0]
                .toObject()["port"].toInt();
        }
        else
        {
            throw "Unable to find stream port in SDP";
        }

        return streamPort;
    }

    QHostAddress Parser::extractOriginIp()       const
    {
        QHostAddress originIp{};

        if
        (
               m_parsedSdp.contains("origin")
            && m_parsedSdp["origin"].toObject().contains("address")
        )
        {
            originIp.setAddress
            (
                m_parsedSdp["origin"]
                    .toObject()["address"].toString()
            );
        }
        else
        {
            throw "Unable to find origin IP in SDP";
        }

        return originIp;
    }
}
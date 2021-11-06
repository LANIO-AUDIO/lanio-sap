#include "sdp.hpp"

namespace SDP
{
    Parser::Parser(const QString& sdp)
    :   m_parsedSdp
        (
            QJsonDocument::fromJson
            (
                sdptransform::parse(sdp.toStdString()).dump().c_str()
            )
        ),
        m_sessionName   { extractSessionName() },
        m_streamIp      { extractStreamIp() },
        m_streamPort    { extractStreamPort() },
        m_originIp      { extractOriginIp() }
    {}

    QString Parser::extractSessionName() const
    {
        QJsonObject sdp{ m_parsedSdp.object() };
        QString sessionName{};

        if(sdp.contains("name") && sdp["name"].isString())
        {
            sessionName = sdp["name"].toString();
        }
        else
        {
            qCritical() << ParseError{ "session name" }.what();
        }

        return sessionName;
    }

    QHostAddress Parser::extractStreamIp()       const
    {
        QJsonObject sdp{ m_parsedSdp.object() };
        QHostAddress streamIp{};

        if
        (
               sdp.contains("connection")
            && sdp["connection"].toObject().contains("ip")
        )
        {
            streamIp.setAddress
            (
                sdp["connection"]
                    .toObject()["ip"].toString()
            );
        }
        else
        {
            qCritical() << ParseError{ "stream IP" }.what();
        }

        return streamIp;
    }

    int         Parser::extractStreamPort()     const
    {
        QJsonObject sdp{ m_parsedSdp.object() };
        int streamPort{};

        if
        (
               sdp.contains("media")
            && sdp["media"].toArray()[0].toObject().contains("port")
        )
        {
            streamPort = sdp["media"]
                .toArray()[0]
                .toObject()["port"].toInt();
        }
        else
        {
            qCritical() << ParseError{ "stream port" }.what();
        }

        return streamPort;
    }

    QHostAddress Parser::extractOriginIp()       const
    {
        QJsonObject sdp{ m_parsedSdp.object() };
        QHostAddress originIp{};

        if
        (
               sdp.contains("origin")
            && sdp["origin"].toObject().contains("address")
        )
        {
            originIp.setAddress
            (
                sdp["origin"]
                    .toObject()["address"].toString()
            );
        }
        else
        {
            qCritical() << ParseError{ "origin IP" }.what();
        }

        return originIp;
    }
}
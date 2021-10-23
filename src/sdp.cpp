#include "sdp.hpp"

namespace SDP
{
    Parser::Parser(const QString& sdp)
    :   m_parsedSdp     ( sdptransform::parse(sdp.toStdString()) ),
        m_sessionName   { extractSessionName() },
        m_streamIp      { extractStreamIp() },
        m_streamPort    { extractStreamPort() },
        m_originIp      { extractOriginIp() }
    {}

    QString Parser::extractSessionName() const
    {
        QString sessionName{};

        if(m_parsedSdp.find("name") != m_parsedSdp.end())
        {
            sessionName = QString::fromStdString
                (m_parsedSdp.find("name")->get<std::string>());
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
               m_parsedSdp.find("connection") != m_parsedSdp.end()
            && m_parsedSdp.at("connection").find("ip") != m_parsedSdp.at("connection").end()
        )
        {
            streamIp.setAddress
            (
                QString::fromStdString
                (
                    m_parsedSdp.at("connection")
                        .find("ip")->get<std::string>()
                )
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
               m_parsedSdp.find("media") != m_parsedSdp.end()
            && m_parsedSdp.at("media").at(0).find("port") != m_parsedSdp.at("media").at(0).end()
        )
        {
            streamPort = m_parsedSdp.at("media").at(0).at("port");
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
               m_parsedSdp.find("origin") != m_parsedSdp.end()
            && m_parsedSdp.at("origin").find("address") != m_parsedSdp.at("origin").end()
        )
        {
            originIp.setAddress
            (
                QString::fromStdString
                (
                    m_parsedSdp.at("origin")
                        .find("address")->get<std::string>()
                )
            );
        }
        else
        {
            throw "Unable to find origin IP in SDP";
        }

        return originIp;
    }
}
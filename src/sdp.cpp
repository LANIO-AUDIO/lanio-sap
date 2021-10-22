#include "sdp.hpp"

namespace SDP
{
    Parser::Parser(const std::string& sdp)
    :   m_parsedSdp     ( sdptransform::parse(sdp) ),
        m_sessionName   { extractSessionName() },
        m_streamIp      { asio::ip::make_address_v4(extractStreamIp()) },
        m_streamPort    { extractStreamPort() },
        m_originIp      { asio::ip::make_address_v4(extractOriginIp()) }
    {}

    std::string Parser::extractSessionName() const
    {
        std::string sessionName{};

        if(m_parsedSdp.find("name") != m_parsedSdp.end())
        {
            sessionName = m_parsedSdp.at("name");
        }
        else
        {
            throw "Unable to find session name in SDP";
        }

        return sessionName;
    }

    std::string Parser::extractStreamIp()       const
    {
        std::string streamIp{};

        if
        (
               m_parsedSdp.find("connection") != m_parsedSdp.end()
            && m_parsedSdp.at("connection").find("ip") != m_parsedSdp.at("connection").end()
        )
        {
            streamIp = m_parsedSdp.at("connection").at("ip");
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

    std::string Parser::extractOriginIp()       const
    {
        std::string originIp{};

        if
        (
               m_parsedSdp.find("origin") != m_parsedSdp.end()
            && m_parsedSdp.at("origin").find("address") != m_parsedSdp.at("origin").end()
        )
        {
            originIp = m_parsedSdp.at("origin").at("address");
        }
        else
        {
            throw "Unable to find origin IP in SDP";
        }

        return originIp;
    }
}
#include "sdp.hpp"

namespace SDP
{
    Parser::Parser(const std::string& sdp)
    :   m_parsedSdp( sdptransform::parse(sdp) )
    {}

    std::string Parser::getSessionName() const
    {
        std::string sessionName{};

        auto iterator = m_parsedSdp.find("name");

        if(iterator != m_parsedSdp.end())
        {
            sessionName = iterator->get<std::string>();
        }
        else
        {
            throw "Unable to find session name in SDP";
        }

        return sessionName;
    }
}
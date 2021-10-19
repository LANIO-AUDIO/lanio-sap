#include "sdptransform.hpp"

namespace SDP
{
    class Parser
    {
    public:
        Parser(const std::string& sdp)
        :   m_parsedSdp{ sdptransform::parse(sdp) }
        {}

        std::string getSessionName() const
        {
            std::string sessionName{ m_parsedSdp.at("name") };

#if 0
            auto iterator = m_parsedSdp.find("name");

            if(iterator != m_parsedSdp.end())
            {
                sessionName = iterator->get<std::string>();
            }
            else
            {
                throw "Unable to find session name in SDP";
            }
#endif

            return sessionName;
        }
    private:
        json m_parsedSdp;
    };
}
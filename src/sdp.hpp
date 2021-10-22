#include "sdptransform.hpp"

namespace SDP
{
    class Parser
    {
    public:
        Parser(const std::string& sdp);

        std::string getSessionName() const;

    private:
        json m_parsedSdp;
    };
}
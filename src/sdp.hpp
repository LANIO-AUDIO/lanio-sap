#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

#include "sdptransform.hpp"

namespace SDP
{
    class Parser
    {
    public:
        Parser(const std::string& sdp);

        inline json                 getJson()           const { return m_parsedSdp; }
        inline std::string          getSessionName()    const { return m_sessionName; }
        inline asio::ip::address_v4 getStreamIp()       const { return m_streamIp; }
        inline int                  getStreamPort()     const { return m_streamPort; }
        inline asio::ip::address_v4 getOriginIp()       const { return m_originIp; }

    private:
        json                    m_parsedSdp;
        std::string             m_sessionName;
        asio::ip::address_v4    m_streamIp;
        int                     m_streamPort;
        asio::ip::address_v4    m_originIp;

        std::string extractSessionName()    const;
        std::string extractStreamIp()       const;
        int         extractStreamPort()     const;
        std::string extractOriginIp()       const;
    };
}
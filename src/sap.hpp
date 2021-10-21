#define BOOST_ASIO_NO_DEPRECATED
#include "boost/asio.hpp"
#include "boost/bind/bind.hpp"
using namespace boost;

#include <array>
#include <bitset>
#include <cstdint>

namespace SAP
{
    using packet_buffer_t = std::array<char, 1500>;

    class Receiver
    {
    public:
        Receiver(asio::io_context& ioContext);

        packet_buffer_t syncReceive();

    private:
        asio::ip::udp::socket   m_socket;
        asio::ip::udp::endpoint m_endpoint;
        packet_buffer_t         m_packetBuffer;
    };

    class Parser
    {
    public:
        Parser(const packet_buffer_t& packetBuffer);
        inline std::uint_least16_t  getHash()
            { return m_messageIdentifierHash; }
        inline asio::ip::address_v4 getSourceAddress()
            { return m_sourceAddress; }
        inline std::string          getSdp()
            { return m_sdp; }
    private:
        enum SAPFlags
        {
            SAP_COMPRESSION,
            SAP_ENCRYPTION,
            SAP_MESSAGE_TYPE,
            SAP_RESERVED3,
            SAP_ADDRESS_TYPE,
            SAP_VERSION,
            SAP_RESERVED6,
            SAP_RESERVED7
        };
        enum SAPAddressType
        {
            SAP_IPV4,
            SAP_IPV6
        };
        std::bitset<8>          m_flags;
        std::uint_least8_t      m_authenticationLength;
        std::uint_least16_t     m_messageIdentifierHash;
        std::size_t             m_addressEndingByte;
        asio::ip::address_v4    m_sourceAddress;
        std::size_t             m_payloadTypeStartByte;
        std::string             m_payloadType;
        std::size_t             m_sdpStartByte;
        std::string             m_sdp;

        bool checkFlags();
    };
}
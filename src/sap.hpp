#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
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
        Parser(packet_buffer_t packetBuffer)
        :   m_packetBuffer
                { packetBuffer },
            m_flags
                { static_cast<unsigned char>(packetBuffer[0]) },
            m_authenticationLength
                { static_cast<std::uint_least8_t>(packetBuffer[1]) },
            m_messageIdentifierHash
            {
                static_cast<std::uint_least16_t>
                    ( (packetBuffer[2] << 8) | packetBuffer[3] )
            },
            m_addressEndingByte
            {
                m_flags.test(SAP_ADDRESS_TYPE) == SAP_IPV4
                    ? 7u : 19u
            },
            m_sourceAddress
            ({
                static_cast<unsigned char>(packetBuffer[4]),
                static_cast<unsigned char>(packetBuffer[5]),
                static_cast<unsigned char>(packetBuffer[6]),
                static_cast<unsigned char>(packetBuffer[7]),
            }),
            m_payloadTypeStartByte
                { m_addressEndingByte + m_authenticationLength + 1 },
            m_payloadType{ &packetBuffer[m_payloadTypeStartByte] },
            m_sdpStartByte{ m_payloadTypeStartByte + m_payloadType.size() +1 },
            m_sdp{ &packetBuffer[m_sdpStartByte] }
        {
            if(!checkFlags())
            {
                throw "INVALID SDP";
            }
        }

    private:
        packet_buffer_t m_packetBuffer;
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

        bool checkFlags()
        {
            return
            (
                   m_flags.test(SAP_VERSION)
                && m_flags.test(SAP_ADDRESS_TYPE) == SAP_IPV4
                && !m_flags.test(SAP_ENCRYPTION)
                && !m_flags.test(SAP_COMPRESSION)
                && m_payloadType == "application/sdp"
            );
        }
    };
}
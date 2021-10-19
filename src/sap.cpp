#include "sap.hpp"

namespace SAP // class Receiver
{
    Receiver::Receiver(asio::io_context& ioContext)
    :   m_socket{ ioContext },
        m_endpoint{ asio::ip::make_address_v4("0.0.0.0"), 9875 },
        m_packetBuffer{}
    {
        m_socket.open(m_endpoint.protocol());
        m_socket.set_option(asio::ip::udp::socket::reuse_address(true));
        m_socket.bind(m_endpoint);

        m_socket.set_option
        (
            asio::ip::multicast::join_group
                (asio::ip::make_address_v4("239.255.255.255"))
        );
    }

    packet_buffer_t Receiver::syncReceive()
    {
        m_socket.receive_from(asio::buffer(m_packetBuffer), m_endpoint);

        Parser parser{ m_packetBuffer };

        return m_packetBuffer;
    }
}

namespace SAP // class Parser
{
    Parser::Parser(const packet_buffer_t& packetBuffer)
    :   m_flags
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
            throw "Invalid SAP Packet";
        }
    }

    bool Parser::checkFlags()
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
}
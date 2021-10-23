#include "sap.hpp"
#include "sdp.hpp"
#include <iostream>

namespace SAP // class Receiver
{
    Receiver::Receiver()
    :   m_sapSocket{},
        m_packetBuffer{}
    {
        m_sapSocket.bind(QHostAddress::AnyIPv4, 9875, QUdpSocket::ShareAddress);
        m_sapSocket.joinMulticastGroup(QHostAddress("239.255.255.255"));

        connect(&m_sapSocket, &QUdpSocket::readyRead, this, &Receiver::processSapPacket);
    }

    void Receiver::processSapPacket()
    {
        m_packetBuffer = m_sapSocket.receiveDatagram();

        Parser sapParser{ m_packetBuffer.data().constData() };
        SDP::Parser sdpParser{ sapParser.getSdp() };

        std::cout << "=== SDP ===\n";
        std::cout << sapParser.getSdp().toStdString();
        std::cout << "=== SDP ===\n";
        std::cout << "=== Parsed SDP ===" << "\n";
        std::cout << sdpParser.getJson().dump(4) << "\n";
        std::cout << "=== Parsed SDP ===" << "\n";

        std::cout << "Session name :\t\t"
            << sdpParser.getSessionName().toStdString() << "\n";
        std::cout << "Stream address :\t"
            << sdpParser.getStreamIp().toString().toStdString() << ":"
            << sdpParser.getStreamPort() << "\n";
        std::cout << "Origin address :\t"
            << sdpParser.getOriginIp().toString().toStdString() << "\n";
    }
}

namespace SAP // class Parser
{
    Parser::Parser(const char* packetBuffer)
    :   m_flags
            { static_cast<unsigned char>(packetBuffer[0]) },
        m_authenticationLength
            { static_cast<quint8>(packetBuffer[1]) },
        m_messageIdentifierHash
        {
            static_cast<quint16>
                ( (packetBuffer[2] << 8) | packetBuffer[3] )
        },
        m_addressEndingByte
        {
            m_flags.test(SAP_ADDRESS_TYPE) == SAP_IPV4
                ? 7 : 19
        },
        m_sourceAddress{ static_cast<quint32>(packetBuffer[4]) },
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
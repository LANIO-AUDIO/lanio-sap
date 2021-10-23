#include <QtNetwork>
#include <bitset>

namespace SAP
{
    using packet_buffer_t = QNetworkDatagram;

    class Receiver : public QObject
    {
    public:
        Receiver();

    private:
        QUdpSocket      m_sapSocket;
        packet_buffer_t m_packetBuffer;

        void processSapPacket();
    };

    class Parser
    {
    public:
        Parser(const char* packetBuffer);

        inline quint16      getHash()
            { return m_messageIdentifierHash; }
        inline QHostAddress getSourceAddress()
            { return m_sourceAddress; }
        inline QString      getSdp()
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
        std::bitset<8>  m_flags;
        quint8          m_authenticationLength;
        quint16         m_messageIdentifierHash;
        int             m_addressEndingByte;
        QHostAddress    m_sourceAddress;
        int             m_payloadTypeStartByte;
        QString         m_payloadType;
        int             m_sdpStartByte;
        QString         m_sdp;

        bool checkFlags();
    };
}
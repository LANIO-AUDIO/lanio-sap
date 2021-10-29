#pragma once

#include <QtNetwork>
#include <QSqlDatabase>
#include <bitset>

namespace SAP
{
    using packet_buffer_t = QNetworkDatagram;

    class Receiver : public QObject
    {
        Q_OBJECT

    public:
        Receiver(const QString& dbPath);

    private:
        QSqlDatabase    m_db;
        QUdpSocket      m_sapSocket;
        packet_buffer_t m_packetBuffer;

    private slots:
        void processSapPacket();
    };

    class Parser
    {
    public:
        Parser(const char* packetBuffer);

        inline bool         isValid()
            { return m_valid; }
        inline bool         isAnnouncement()
            { return m_flags.test(SAP_MESSAGE_TYPE) == SAP_ANNOUNCEMENT; }
        inline bool         isDeletion()
            { return m_flags.test(SAP_MESSAGE_TYPE) == SAP_DELETION; }
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
        enum SAPMessageType
        {
            SAP_ANNOUNCEMENT,
            SAP_DELETION
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
        bool            m_valid;

        std::bitset<8>  extractFlags                (const char* packetBuffer);
        quint8          extractAuthenticationLength (const char* packetBuffer);
        quint16         extractMessageIdentifierHash(const char* packetBuffer);
        int             extractAddressEndingByte    ();
        quint32         extractSourceAddress        (const char* packetBuffer);
        int             extractPayloadTypeStartByte ();
        QString         extractPayloadType          (const char* packetBuffer);
        int             extractSdpStartByte         ();
        QString         extractSdp                  (const char* packetBuffer);

        bool            checkFlags();
    };
}
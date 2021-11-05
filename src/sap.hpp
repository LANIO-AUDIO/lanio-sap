#pragma once

#include "sdp.hpp"
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlError>
#include <bitset>

namespace SAP
{
    using packet_buffer_t = QNetworkDatagram;

    class SqlError : public std::exception
    {
    public:
        SqlError(const QSqlError& sqlError)
        :   m_sqlError{ sqlError }
        {}

        const char* what() const noexcept
        {
            return m_sqlError.driverText().toUtf8().constData();
        }
    private:
        QSqlError   m_sqlError;
    };

    class NetworkError : public std::exception
    {
    public:
        NetworkError(const QString& message)
        :   m_message{ message }
        {}

        const char* what() const noexcept
        {
            return m_message.toUtf8().constData();
        }
    private:
        QString m_message;
    };

    class Parser
    {
    public:
        Parser(const char* packetBuffer);

        inline bool         isValid()           const
            { return m_valid; }
        inline bool         isAnnouncement()    const
            { return m_flags.test(SAP_MESSAGE_TYPE) == SAP_ANNOUNCEMENT; }
        inline bool         isDeletion()        const
            { return m_flags.test(SAP_MESSAGE_TYPE) == SAP_DELETION; }
        inline quint16      getHash()           const
            { return m_messageIdentifierHash; }
        inline QHostAddress getSourceAddress()  const
            { return m_sourceAddress; }
        inline QString      getSdp()            const
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

    class Receiver : public QObject
    {
        Q_OBJECT

    public:
        Receiver(const QString& dbPath);

    private:
        QSqlDatabase    m_db;
        QUdpSocket      m_sapSocket;
        packet_buffer_t m_packetBuffer;

        void updateDb(const Parser& sapParser, const SDP::Parser& sdpParser);

    private slots:
        void processSapPacket();
    };
}
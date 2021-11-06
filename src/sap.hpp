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
        SqlError(const QSqlError& sqlError, const QString& query = "")
            : m_sqlError{ sqlError }
            , m_query{ query }
            , m_returnText
            {
                QString
                {
                    "SQLITE ERROR : "
                    + m_sqlError.driverText()
                }
            }
        {
            if(!m_sqlError.databaseText().isEmpty())
            {
                m_returnText += " / ";
                m_returnText += m_sqlError.databaseText();
            }

            if(!m_sqlError.nativeErrorCode().isEmpty())
            {
                m_returnText += " (Code ";
                m_returnText += m_sqlError.nativeErrorCode();
                m_returnText += ")";
            }

            if(!m_query.isEmpty())
            {
                m_returnText += "\nQuery :\n";
                m_returnText += m_query;
            }
        }

        const char* what() const noexcept
        {
            return m_returnText.toUtf8().constData();
        }

    private:
        QSqlError   m_sqlError;
        QString     m_query;
        QString     m_returnText;
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
        static const QString tableName;

    private:
        QMutex          m_mutex;
        QSqlDatabase    m_db;
        QUdpSocket      m_sapSocket;
        packet_buffer_t m_packetBuffer;

        void updateDb(const Parser& sapParser, const SDP::Parser& sdpParser);
        void createTable();
        void setUpSocket();

    private slots:
        void processSapPacket();
    };
}
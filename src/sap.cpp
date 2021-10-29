#include <QSqlQuery>
#include <QSqlError>
#include "sap.hpp"
#include "sdp.hpp"

namespace SAP // class Receiver
{
    Receiver::Receiver(const QString& dbPath)
    :   m_db{ QSqlDatabase::addDatabase("QSQLITE") },
        m_sapSocket{},
        m_packetBuffer{}
    {
        m_db.setDatabaseName(dbPath);
        if(!m_db.open())
        {
            throw m_db.lastError().driverText();
        }

        QSqlQuery query{};
        query.prepare
        (R"(
            CREATE TABLE IF NOT EXISTS SAP_Streams
            (
                id              INTEGER     PRIMARY KEY AUTOINCREMENT,
                timestamp       DATETIME    DEFAULT     CURRENT_TIMESTAMP,
                sap_hash        INTEGER     UNIQUE,
                sap_sourceip    VARCHAR,
                sdp_raw         VARCHAR,
                sdp_json        VARCHAR
            );
        )");
        if(!query.exec())
        {
            throw query.lastError().driverText();
        }
        query.prepare
        (R"(
            CREATE TRIGGER IF NOT EXISTS AFTER UPDATE ON SAP_Streams
                WHEN OLD.timestamp < CURRENT_TIMESTAMP - 60
                BEGIN
                    DELETE FROM SAP_Streams WHERE id = OLD.id;
                END
            ;
        )");
        if(!query.exec())
        {
            throw query.lastError().driverText();
        }

        m_sapSocket.bind(QHostAddress::AnyIPv4, 9875, QUdpSocket::ShareAddress);
        m_sapSocket.joinMulticastGroup(QHostAddress("239.255.255.255"));

        connect
        (
            &m_sapSocket,
            &QUdpSocket::readyRead,
            this,
            &Receiver::processSapPacket,
            Qt::UniqueConnection
        );
    }

    void Receiver::processSapPacket()
    {
        m_packetBuffer = m_sapSocket.receiveDatagram();

        Parser sapParser{ m_packetBuffer.data().constData() };

        if(!sapParser.isValid())
        {
            qInfo().noquote().nospace()
                << "Invalid SAP packet\t: Stream ID 0x"
                << Qt::hex << Qt::uppercasedigits << sapParser.getHash()
            ;
            return;
        }

        SDP::Parser sdpParser{ sapParser.getSdp() };

        qInfo().noquote().nospace()
            << (sapParser.isAnnouncement() ? "Announcement" : "Deletion")
            << "\t: Stream ID 0x"
            << Qt::hex << Qt::uppercasedigits << sapParser.getHash()
        ;

        updateDb(sapParser, sdpParser);
    }

    void Receiver::updateDb(const Parser& sapParser, const SDP::Parser& sdpParser)
    {
        QSqlQuery query{};

        if(sapParser.isAnnouncement())
        {
            query.prepare
            (R"(
                INSERT INTO SAP_Streams VALUES
                (
                    NULL,
                    CURRENT_TIMESTAMP,
                    :hash,
                    :sourceip,
                    :sdpraw,
                    :sdpjson
                )
                ON CONFLICT (sap_hash)
                    DO UPDATE SET timestamp = CURRENT_TIMESTAMP
                ;
            )");
            query.bindValue(":hash",        sapParser.getHash());
            query.bindValue(":sourceip",    sapParser.getSourceAddress().toString());
            query.bindValue(":sdpraw",      sapParser.getSdp());
            query.bindValue(":sdpjson",     sdpParser.getJson().toJson());
        }
        else if(sapParser.isDeletion())
        {
            query.prepare
            (R"(
                DELETE FROM SAP_Streams WHERE sap_hash = :hash ;
            )");
            query.bindValue(":hash", sapParser.getHash());
        }

        if(!query.exec())
        {
            throw query.lastError().driverText();
        }
    }
}

namespace SAP // class Parser
{
    Parser::Parser(const char* packetBuffer)
    :   m_flags                 { extractFlags(packetBuffer) },
        m_authenticationLength  { extractAuthenticationLength(packetBuffer) },
        m_messageIdentifierHash { extractMessageIdentifierHash(packetBuffer) },
        m_addressEndingByte     { extractAddressEndingByte() },
        m_sourceAddress         { extractSourceAddress(packetBuffer) },
        m_payloadTypeStartByte  { extractPayloadTypeStartByte() },
        m_payloadType           { extractPayloadType(packetBuffer) },
        m_sdpStartByte          { extractSdpStartByte() },
        m_sdp                   { extractSdp(packetBuffer) },
        m_valid                 { checkFlags() }
    {
    }

    std::bitset<8>  Parser::extractFlags                (const char* packetBuffer)
    {
        return static_cast<unsigned char>(packetBuffer[0]);
    }
    quint8          Parser::extractAuthenticationLength (const char* packetBuffer)
    {
        return static_cast<quint8>(packetBuffer[1]);
    }
    quint16         Parser::extractMessageIdentifierHash(const char* packetBuffer)
    {
        return static_cast<quint16>( (packetBuffer[2] << 8) | packetBuffer[3] );
    }
    int             Parser::extractAddressEndingByte    ()
    {
        return m_flags.test(SAP_ADDRESS_TYPE) == SAP_IPV4 ? 7 : 19;
    }
    quint32         Parser::extractSourceAddress        (const char* packetBuffer)
    {
        return static_cast<quint32>
        (
              static_cast<unsigned char>(packetBuffer[4]) << 24
            | static_cast<unsigned char>(packetBuffer[5]) << 16
            | static_cast<unsigned char>(packetBuffer[6]) << 8
            | static_cast<unsigned char>(packetBuffer[7])
        );
    }
    int             Parser::extractPayloadTypeStartByte ()
    {
        return m_addressEndingByte + m_authenticationLength + 1;
    }
    QString         Parser::extractPayloadType          (const char* packetBuffer)
    {
        return &packetBuffer[m_payloadTypeStartByte];
    }
    int             Parser::extractSdpStartByte         ()
    {
        return m_payloadTypeStartByte + m_payloadType.size() +1;
    }
    QString         Parser::extractSdp                  (const char* packetBuffer)
    {
        return &packetBuffer[m_sdpStartByte];
    }

    bool            Parser::checkFlags()
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
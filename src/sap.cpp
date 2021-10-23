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
            throw "Unable to open database";
        }

        QSqlQuery query{};
        query.prepare(R"(
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
        query.prepare(R"(
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

        connect(&m_sapSocket, &QUdpSocket::readyRead, this, &Receiver::processSapPacket);
    }

    void Receiver::processSapPacket()
    {
        m_packetBuffer = m_sapSocket.receiveDatagram();

        Parser sapParser{ m_packetBuffer.data().constData() };
        SDP::Parser sdpParser{ sapParser.getSdp() };

        qDebug().noquote().nospace()
            << (sapParser.isAnnouncement() ? "Announcement" : "Deletion")
            << "\t: Stream ID 0x" << Qt::hex << Qt::uppercasedigits << sapParser.getHash()
        ;

//        QSqlQuery query{};
//        query.prepare();
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
#include <QSqlQuery>
#include <QSqlError>
#include "debug.hpp"
#include "sap.hpp"
#include "sdp.hpp"

namespace SAP // class Receiver
{
    Receiver::Receiver(const QString& dbPath)
        : m_mutex{}
        , m_db{ QSqlDatabase::addDatabase("QSQLITE") }
        , m_sapSocket{}
        , m_packetBuffer{}
    {
        m_db.setDatabaseName(dbPath);
        if(!m_db.open())
        {
            qFatal(SqlError{ m_db.lastError() }.what());
        }

        createTable();

        setUpSocket();

        connect
        (
            &m_sapSocket,
            &QUdpSocket::readyRead,
            this,
            &Receiver::processSapPacket,
            Qt::UniqueConnection
        );

        QTimer* timer = new QTimer(this);

        connect(timer, &QTimer::timeout, this, &Receiver::removeOldEntries);

        timer->start(15 * 1000);
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
            << "== SAP "
            << (sapParser.isAnnouncement() ? "Announcement" : "Deletion")
            << " =="
            << "\tStream ID : 0x"
            << Qt::hex << Qt::uppercasedigits << sapParser.getHash()
            << "\tSource IP : " << sapParser.getSourceAddress().toString()
        ;

        m_mutex.lock();
        updateDb(sapParser, sdpParser);
        m_mutex.unlock();
    }

    void Receiver::updateDb(const Parser& sapParser, const SDP::Parser& sdpParser)
    {
        QSqlQuery query{};

        if(sapParser.isAnnouncement())
        {
            query.prepare
            (QString(R"(
                INSERT INTO %1 VALUES
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
            )").arg(SAP::tableName));

            query.bindValue(":hash",        sapParser.getHash());
            query.bindValue(":sourceip",    sapParser.getSourceAddress().toString());
            query.bindValue(":sdpraw",      sapParser.getSdp());
            query.bindValue(":sdpjson",     sdpParser.getJson().toJson());
        }
        else if(sapParser.isDeletion())
        {
            query.prepare
            (QString(R"(
                DELETE FROM %1 WHERE sap_hash = :hash ;
            )").arg(SAP::tableName));

            query.bindValue(":hash", sapParser.getHash());
        }

        if(!query.exec())
        {
            m_mutex.unlock();
            qCritical()
                << SqlError{ query.lastError(), query.lastQuery() }.what();
        }
    }

    void Receiver::createTable()
    {
        QSqlQuery query{};

        query.prepare
        (QString(R"(
            CREATE TABLE IF NOT EXISTS %1
            (
                id              INTEGER     PRIMARY KEY AUTOINCREMENT,
                timestamp       DATETIME    DEFAULT     CURRENT_TIMESTAMP,
                sap_hash        INTEGER     UNIQUE,
                sap_sourceip    VARCHAR,
                sdp_raw         VARCHAR,
                sdp_json        VARCHAR
            );
        )").arg(SAP::tableName));

        if(!query.exec())
        {
            qFatal(SqlError{ query.lastError(), query.lastQuery() }.what());
        }

        QVector<QString> triggerOperations{ "INSERT", "UPDATE", "DELETE" };

        for(const auto& operation : triggerOperations)
        {
            query.prepare
            (QString(R"(
                CREATE TRIGGER IF NOT EXISTS %2_trigger AFTER %2 ON %1
                    BEGIN
                        DELETE FROM %1 WHERE timestamp < DATETIME('now', '-60 seconds');
                    END
                ;
            )").arg(SAP::tableName).arg(operation));

            if(!query.exec())
            {
                qFatal(SqlError{ query.lastError(), query.lastQuery() }.what());
            }
        }
    }

    void Receiver::setUpSocket()
    {
        if
        (
            !(
                m_sapSocket.bind
                (
                    QHostAddress::AnyIPv4,
                    9875,
                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint
                )
                &&
                m_sapSocket.joinMulticastGroup(QHostAddress("239.255.255.255"))
            )
        )
        {
            qFatal(NetworkError{ m_sapSocket.errorString() }.what());
        }
    }

    void Receiver::removeOldEntries()
    {
        qCDebug(sql).noquote() << "Checking for outdated SAP entries...";

        QSqlQuery query{};

        query.prepare
        (QString(R"(
            SELECT COUNT(*) FROM %1 WHERE timestamp < DATETIME('now', '-60 seconds');
        )").arg(SAP::tableName));

        if(!query.exec())
        {
            qCritical()
                << SqlError{ query.lastError(), query.lastQuery() }.what();
        }

        qint32 querySize{};
        if(query.next())
        {
            querySize = query.value(0).toInt();
        }
        query.finish();

        if(querySize <= 0)
        {
            qCDebug(sql) << "All records are up-to-date.";
            return;
        }

        query.prepare
        (QString(R"(
            SELECT sap_hash, sap_sourceip FROM %1
                WHERE timestamp < DATETIME('now', '-60 seconds');
        )").arg(SAP::tableName));

        if(!query.exec())
        {
            qCritical()
                << SqlError{ query.lastError(), query.lastQuery() }.what();
        }

        QVector<QPair<quint32, QString>> rowsToDelete{};
        rowsToDelete.reserve(querySize);

        while(query.isActive() && query.next())
        {
            rowsToDelete.push_back
            ({
                query.value(0).toUInt(),
                query.value(1).toString()
            });
        }

        query.prepare
        (QString(R"(
            DELETE FROM %1 WHERE timestamp < DATETIME('now', '-60 seconds');
        )").arg(SAP::tableName));

        if(!query.exec())
        {
            qCritical()
                << SqlError{ query.lastError(), query.lastQuery() }.what();
        }

        for(const auto& row : rowsToDelete)
        {
            qInfo().noquote().nospace()
                << "== SAP Timeout ==\tStream ID : 0x"
                << Qt::hex << Qt::uppercasedigits << row.first
                << "\tSource IP : " << row.second
            ;
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
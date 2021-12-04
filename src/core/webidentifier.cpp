#include "webidentifier.h"
#include <QtCore/QDebug>
#include <QRandomGenerator>

#define IDENTIFIER_UDP_PORT 12335

#define IDENTITY_HEADER 0x4B9ACA00
#define IDENTITY_HEADER_LENGTH 10
#define IDENTITY_ID_LENGTH 4
#define IDENTITY_IP_LENGTH 16
#define IDENTITY_WORKINGPORT_LENGTH 5
#define IDENTITY_READABLENAME_LENGTH 16

WebIdentifier::WebIdentifier(const QString &identityReadableName, const port_t &socketWorkingPort, QObject *parent) :
    QObject(parent),
    m_identityReadableName(identityReadableName),
    m_identityId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
    m_socketWorkingPort(socketWorkingPort)
{
    m_identifierSocket.bind(IDENTIFIER_UDP_PORT, QUdpSocket::ShareAddress);
    connect(&m_identifierSocket, &QUdpSocket::readyRead, this, &WebIdentifier::onIdenReadReady);
}

void WebIdentifier::setIdentityReadableName(const QString &readableName)
{
    m_identityReadableName = readableName;
}

void WebIdentifier::setIdentityId(const int &id)
{
    m_identityId = id;
}

void WebIdentifier::setIdentityIp(QString ip)
{
    m_identityIp = ip;
}

void WebIdentifier::setWorkingPort(const port_t &port)
{
    m_socketWorkingPort = port;
}

void WebIdentifier::boardcastIdentityMessage()
{
    QByteArray datagram = generateIdentidyData(m_identityReadableName, m_socketWorkingPort);
    qDebug() << "boardcast: name" << m_identityReadableName << "port" << m_socketWorkingPort;
    m_identifierSocket.writeDatagram(datagram, QHostAddress::Broadcast, IDENTIFIER_UDP_PORT);
}

void WebIdentifier::onIdenReadReady()
{
    QByteArray datagram;
    QHostAddress senderAddress;
    while (m_identifierSocket.hasPendingDatagrams()) {
        datagram.resize(int(m_identifierSocket.pendingDatagramSize()));
        m_identifierSocket.readDatagram(datagram.data(), datagram.size(), &senderAddress);
        qint64 head = QString::fromUtf8(datagram.left(IDENTITY_HEADER_LENGTH)).toInt();
        if(head != IDENTITY_HEADER){
            qDebug() << "WebIdentifier: unreadable udp datagram head" << head;
            continue;
        }
        qint64 offset = IDENTITY_HEADER_LENGTH;
        QString senderId = QString::fromUtf8(datagram.mid(offset, IDENTITY_ID_LENGTH));
        offset += IDENTITY_ID_LENGTH;
        QString senderIp = QString::fromUtf8(datagram.mid(offset, IDENTITY_IP_LENGTH));
        offset += IDENTITY_IP_LENGTH;
        QString senderWorkingPort = QString::fromUtf8(datagram.mid(offset, IDENTITY_WORKINGPORT_LENGTH));
        offset += IDENTITY_WORKINGPORT_LENGTH;
        QString senderReadableName = QString::fromUtf8(datagram.mid(offset));
        // filt datagram from itself
        if(senderId == m_identityId && senderReadableName == m_identityReadableName){
            continue;
        }
        emit identityMessageParsed(senderIp, senderWorkingPort, senderReadableName, senderId);
        qDebug() << QString("ientify client: ip=%1, workingPort=%2, name=%3, id=%4")
                    .arg(senderIp, senderWorkingPort, senderReadableName, senderId);
    }
}

QByteArray WebIdentifier::generateIdentidyData(const QString &identityReadableName, const port_t &socketWorkingPort)
{
    QByteArray identityByteArray;
    identityByteArray.append(QString::number(IDENTITY_HEADER).toUtf8(), IDENTITY_HEADER_LENGTH);
    identityByteArray.append(QString::number(m_identityId).toUtf8(), IDENTITY_ID_LENGTH);
    identityByteArray.append(m_identityIp.toUtf8(), IDENTITY_IP_LENGTH);
    identityByteArray.append(QString::number(socketWorkingPort).toUtf8(), IDENTITY_WORKINGPORT_LENGTH);
    identityByteArray.append(identityReadableName.toUtf8());
    return identityByteArray;
}

#include "TCPNotifier.hpp"
#include <QDebug>
#include <QDateTime>
#include "Config.h"
#include <QRegExpValidator>
#include "pbx/pbx.h"

TCPNotifier::TCPNotifier(QObject *parent) :
    QObject(parent),
    tcpServer_(NULL)
{

}

bool TCPNotifier::initialize()
{
    QRegExpValidator validator(QRegExp("((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))"));
    int pos;
    QValidator::State state = validator.validate(Config::Instance()->notifierTCPServerIP_, pos);
    if (state != QValidator::Acceptable
            || Config::Instance()->notifierTCPServerPort_ < 0
            || Config::Instance()->notifierTCPServerPort_ > 65535) {
        qWarning() << QString("invalid server cfg:\n"
                              "\tstate: %1\n"
                              "\tip: %2\n"
                              "\tport: %3\n").arg(QString(state),
                                                  Config::Instance()->notifierTCPServerIP_,
                                                  QString::number(Config::Instance()->notifierTCPServerPort_));
        return false;
    }

    tcpServer_ = new QTcpServer(this);
    connect(tcpServer_, &QTcpServer::newConnection, this, [&](){
        while (tcpServer_->hasPendingConnections()) {
            QTcpSocket *peer = tcpServer_->nextPendingConnection();
            qDebug() << "new connection: " << peer->peerAddress() << peer->peerPort();
            peerSocks_.append(peer);
        }
    });

    if (!tcpServer_->listen(QHostAddress(Config::Instance()->notifierTCPServerIP_),
                            Config::Instance()->notifierTCPServerPort_)) {
        qWarning() << "tcpserver listen error";
        delete tcpServer_;
        tcpServer_ = NULL;
        return false;
    } else {
        qDebug() << "TCPServe start listen: " << tcpServer_->serverAddress() << tcpServer_->serverPort();
    }

    return true;
}

void TCPNotifier::cleanup()
{
    tcpServer_->close();
}

void TCPNotifier::handleExtenStateChanged(const QString &number, int state)
{
    if (tcpServer_ != NULL && tcpServer_->isListening()) {

        QString cmdStr;
        switch (state) {
        case PBX::eOffline: cmdStr = "NERR"; break;
        case PBX::eIdle: cmdStr = "EROK"; break;
        case PBX::eRing:
        case PBX::eRinging: cmdStr = "RING"; break;
        case PBX::eIncall: cmdStr = "PICK"; break;
        }

        QString content = QString("***%2[%1][%3]###").arg(number, cmdStr, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        for (auto peer: peerSocks_) {
            if (peer->state() != QAbstractSocket::ConnectedState)
                continue;

            peer->write(content.toLocal8Bit());
        }
    }
}

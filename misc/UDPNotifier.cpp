#include "UDPNotifier.h"
#include <QDebug>
#include <QDateTime>
#include "Config.h"
#include <QRegExpValidator>
#include "pbx/pbx.h"

UDPNotifier::UDPNotifier(QObject *parent) :
    QObject(parent),
    udpSock_(NULL)
{

}

bool UDPNotifier::initialize()
{
    QRegExpValidator validator(QRegExp("((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))"));
    int pos;
    QValidator::State state = validator.validate(Config::Instance()->notifierUDPIP_, pos);
    if (state != QValidator::Acceptable
            || Config::Instance()->notifierUDPPort_ < 0
            || Config::Instance()->notifierUDPPort_ > 65535) {
        qWarning() << QString("invalid peer cfg:\n"
                              "\tstate: %1\n"
                              "\tip: %2\n"
                              "\tport: %3\n").arg(QString(state),
                                                  Config::Instance()->notifierUDPIP_,
                                                  QString::number(Config::Instance()->notifierUDPPort_));
        return false;
    }

    udpSock_ = new QUdpSocket(this);
    if (!udpSock_->bind(QHostAddress::LocalHost)) {
        qWarning() << "udp socket bind error";
        delete  udpSock_;
        udpSock_ = NULL;
        return false;
    }

    peerIP_ = Config::Instance()->notifierUDPIP_;
    peerPort_ = Config::Instance()->notifierUDPPort_;

    return true;
}

void UDPNotifier::cleanup()
{
    udpSock_->close();
}

void UDPNotifier::handleExtenStateChanged(const QString &number, int state)
{
    if (udpSock_ != NULL && udpSock_->state() == QAbstractSocket::BoundState) {

        QString cmdStr;
        switch (state) {
        case PBX::eOffline: cmdStr = "NERR"; break;
        case PBX::eIdle: cmdStr = "EROK"; break;
        case PBX::eRing:
        case PBX::eRinging: cmdStr = "RING"; break;
        case PBX::eIncall: cmdStr = "PICK"; break;
            break;
        }

        QString content = QString("***%2[%1][%3]###").arg(number, cmdStr, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        udpSock_->writeDatagram(content.toLocal8Bit(), peerIP_, peerPort_);
    }
}

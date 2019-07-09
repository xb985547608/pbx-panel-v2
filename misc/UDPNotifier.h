#ifndef UDPNOTIFIER_H
#define UDPNOTIFIER_H

#include <QObject>
#include <QUdpSocket>

class UDPNotifier : public QObject
{
    Q_OBJECT
public:
    explicit UDPNotifier(QObject *parent = nullptr);


    bool initialize();
    void cleanup();

public slots:
    void handleExtenStateChanged(const QString &number, int state);

private:
    QUdpSocket *udpSock_;
    QHostAddress peerIP_;
    quint16 peerPort_;
};

#endif // UDPNOTIFIER_H

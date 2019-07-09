#ifndef TCPNOTIFIER_HPP
#define TCPNOTIFIER_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TCPNotifier : public QObject
{
    Q_OBJECT
public:
    explicit TCPNotifier(QObject *parent = nullptr);

    bool initialize();
    void cleanup();

public slots:
    void handleExtenStateChanged(const QString &number, int state);

private:
    QTcpServer *tcpServer_;
    QList<QTcpSocket*> peerSocks_;
};

#endif // TCPNOTIFIER_HPP

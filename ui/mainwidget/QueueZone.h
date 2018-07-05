#ifndef QUEUEZONE_H
#define QUEUEZONE_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

class QueueZone : public QWidget
{
    Q_OBJECT
public:
    explicit QueueZone(QWidget *parent = 0);
    ~QueueZone();
    void eventMessageReceived(QMap<QString, QString>&);
    static QString getQueueNumber() { return mOperatorQueueNumber; }
signals:
    void queueEventSignal(QString event, QString callerid, QString calleridname, QString channel);
private slots:
    void queueEventSlot(QString event, QString callerid, QString calleridname, QString channel);
private:
    QStandardItemModel* mQueueListModel;
    static QString mOperatorQueueNumber;
};

#endif // QUEUEZONE_H

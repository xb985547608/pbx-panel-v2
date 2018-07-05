#ifndef EVENTEXTENSIONSZONE_H
#define EVENTEXTENSIONSZONE_H

#include <QtGui>
#include "EventWidget.h"
#include "ui/base/multitablewidgets.h"
#include "pbx/pbx.h"
#include "EventExten.h"

class EventExtensionsZone : public QWidget
{
    Q_OBJECT
public:
    explicit EventExtensionsZone(EventWidget* ,QWidget *parent = 0);
    ~EventExtensionsZone();
    void loadEventExtens(QList<PBX::Extension>);
    void eventMessageReceived(QMap<QString, QString>&);
signals:
    void eventMessageReceivedSignal(QString extenNumber, int state);
    
public slots:
    void eventMessageReceivedSlot(QString extenNumber, int state);
private:
    EventWidget* mEventWidget;
    QList<EventExten*> mEventExtens;
    MultiTableWidgets* mEventExtenPanel;
};

#endif // EVENTEXTENSIONSZONE_H

#ifndef OPERATORZONE_H
#define OPERATORZONE_H

#include "ui/base/tablewidget.h"
#include "ui/base/uiextensionbase.h"
#include "pbx/pbx.h"

#include <QtGui>

class OperatorExten;
class OperatorZone : public QWidget
{
    Q_OBJECT
public:
    explicit OperatorZone(QWidget *parent = 0);
    ~OperatorZone();
    void eventMessageReceived(QMap<QString, QString>& msg);
    
signals:
    
public slots:
    
private:
    TableWidget* mHolderWidget;
    //<number, OperatorExten>
    QMap<QString, OperatorExten*> mOpExtens;
};

class OperatorExten : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit OperatorExten(PBX::Extension e, QWidget* parent = 0);
signals:
    void stateChangedSignal(int state);
public slots:
    void stateChangedSlot(int state);
public:
    void onStateChanged(int state);
private:
};

#endif // OPERATORZONE_H

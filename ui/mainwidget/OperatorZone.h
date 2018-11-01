#ifndef OPERATORZONE_H
#define OPERATORZONE_H

#include "ui/base/tablewidget.h"
#include "ui/base/uiextensionbase.h"
#include "pbx/pbx.h"
#include "MainWidget.h"

#include <QtGui>

class OperatorExten;
class OperatorZone : public QWidget
{
    Q_OBJECT
public:
    explicit OperatorZone(MainWidget* mw, QWidget *parent = 0);
    ~OperatorZone();
    void eventMessageReceived(QMap<QString, QString>& msg);
    void opExtenClicked(QString extenNumber);
    PBX::Extension getSelectedOpExten();
    QStringList getOperatorExtens() const { return  mOpExtens.keys(); }
    
signals:
    void operatorExtenStateChagne(QString number);

public slots:
    
private:
    TableWidget* mHolderWidget;
    //<number, OperatorExten>
    QMap<QString, OperatorExten*> mOpExtens;
    MainWidget* mMainWidget;
};

class OperatorExten : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit OperatorExten(PBX::Extension e, MainWidget*, QWidget* parent = 0);
signals:
    void stateChangedSignal();
public slots:
    void stateChangedSlot();
    void clickedSlot();
public:
    void onStateChanged();
private:
    MainWidget* mMainWidget;
};

#endif // OPERATORZONE_H

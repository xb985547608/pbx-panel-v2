#ifndef FUNCTIONZONE_H
#define FUNCTIONZONE_H

#include <QWidget>
#include "ui/base/tablewidget.h"
#include "EventWidget.h"

class FunctionZone : public QWidget
{
    Q_OBJECT
public:
    explicit FunctionZone(EventWidget*, QWidget *parent = 0);
    
signals:
    
public slots:
protected:
    virtual void initFunctions() = 0;
    TableWidget* mContentHolder;
    EventWidget* mEventWidget;
};

class FunctionZoneEmergency : public FunctionZone
{
    Q_OBJECT
public:
    explicit FunctionZoneEmergency(EventWidget* ew, QWidget* parent = 0);
public slots:
    void onBtnAlertClickedSlot();
protected:
    virtual void initFunctions();
};

class FunctionZoneNormal : public FunctionZone
{
    Q_OBJECT
public:
    explicit FunctionZoneNormal(EventWidget*, QWidget* parent = 0);
protected:
    virtual void initFunctions();
};

#endif // FUNCTIONZONE_H

#ifndef EMERGENCYEVENTWIDGET_H
#define EMERGENCYEVENTWIDGET_H

#include "ui/base/BaseWidget.h"
#include "pbx/EventModel.h"

class EventExtensionsZone;
class DescriptionZone;
class FunctionZone;
class PlanZone;

class EventWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit EventWidget(EventModel::EventType, MainWindow *mainWindow, QWidget *parent = 0);
    EventModel::EventType getWidgetType() { return mEventType; }
    DescriptionZone* getDescriptionZone() { return mDescriptionZone; }
    EventExtensionsZone* getEventExtensionsZone() { return mEventExtensionsZone; }
    FunctionZone* getFunctionZone() { return mFunctionZone; }
    PlanZone* getPlanZone() { return mPlanZone; }
    void eventPlanSelected();
    bool saveEvent();
    QString getEventId() { return mEventId; }
    virtual void onResume(QMap<QString, QVariant> &);

public slots:
    void broadcast();
    void makeCall(bool chase = false);
    void sendSms();
    void playSound();
    void hangupAll();
protected:
    virtual void initFrames();
    EventModel::EventType mEventType;
private:
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QStringList getChannels2Call();

    DescriptionZone* mDescriptionZone;
    EventExtensionsZone* mEventExtensionsZone;
    FunctionZone* mFunctionZone;
    PlanZone* mPlanZone;
    QString mEventId; //rpc调用create_event后，会返回这个值
};

#endif // EMERGENCYEVENTWIDGET_H

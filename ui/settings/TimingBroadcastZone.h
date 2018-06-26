#ifndef TIMINGBROADCASTZONE_H
#define TIMINGBROADCASTZONE_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "pbx/EventModel.h"
#include "SettingsWidget.h"
#include "ui/base/multitablewidgets.h"

class SettingsWidget;
class TimingBroadcastZone : public QWidget
{
    Q_OBJECT
public:
    explicit TimingBroadcastZone(EventModel::EventType eventType, SettingsWidget*, QWidget *parent = 0);
    void loadPlans();
private slots:
    void onBtnNewTimingBroadcastClicked();
    void onBtnPlanClicked();
    void onPopupActionEditTriggered();
    void onPopupActionDeleteTriggered();
private:
    MultiTableWidgets *mTimingBroadcastHolder;
    EventModel::EventType mZoneType;
    SettingsWidget *mSettingsWidget;
};

#endif // TIMINGBROADCASTZONE_H

#ifndef PLANLISTZONE_H
#define PLANLISTZONE_H

#include <QtGui>
#include "ui/base/multitablewidgets.h"
#include "pbx/EventModel.h"
#include "SettingsWidget.h"

class PlanListZone : public QWidget
{
    Q_OBJECT
public:
    explicit PlanListZone(EventModel::EventType eventType, SettingsWidget*, QWidget *parent = 0);
    void loadPlans();
signals:
    
private slots:
    void onBtnNewEventClicked();
    void onBtnPlanClicked();
    void onPopupActionEditTriggered();
    void onPopupActionDeleteTriggered();
private :
    MultiTableWidgets* mPlanListHolder;
    EventModel::EventType mZoneType;
    SettingsWidget* mSettingsWidget;
};

class PlanName : public QPushButton
{
    Q_OBJECT
public:
    PlanName(QString text, QWidget* parent = 0);
    ~PlanName();
};

class NewEventNameDialog : public QDialog
{
    Q_OBJECT
public:
    NewEventNameDialog(EventModel::EventType, QWidget *parent = 0);
    QString getName() { return mTxtName->text(); }
signals:
    //void eventNameConfirmed(QString& name);
private slots:
    void onOk();
    void onCancel();
private:
    QLineEdit* mTxtName;
};

#endif // PLANLISTZONE_H

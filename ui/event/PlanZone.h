#ifndef PLANZONE_H
#define PLANZONE_H

#include <QtGui>
#include "EventWidget.h"
#include "ui/base/multitablewidgets.h"

class PlanZone : public QWidget
{
    Q_OBJECT
public:
    explicit PlanZone(EventWidget*, QWidget *parent = 0);
    void loadData(QString planid = "");
    EventModel::Plan getSelectedPlan() { return mSelectedPlan; }
signals:
    
public slots:
    void onBtnPlanClicked();
private:
    EventWidget* mEventWidget;
    MultiTableWidgets* mHolder;
    EventModel::Plan mSelectedPlan;
};

class NewEventAlerting: public QDialog
{
    Q_OBJECT
public :
    explicit NewEventAlerting(QWidget* parent = 0);
private slots:
    void onOk();
    void onCancel();
};

#endif // PLANZONE_H

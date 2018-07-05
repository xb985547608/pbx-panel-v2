#include "TimingBroadcastZone.h"
#include "ui/base/misccomponents.h"
#include "PlanListZone.h"
#include "ui/timedbroadcast/TimedBroadcastEditor.h"
#include "ui/MainWindow.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"

/*************************************PlanName类和NewEventNameDialog类定义在PlanListZone.h文件中*/

TimingBroadcastZone::TimingBroadcastZone(EventModel::EventType eventType, SettingsWidget* settingsWidget, QWidget *parent) :
    QWidget(parent), mZoneType(eventType), mSettingsWidget(settingsWidget)
{

    QPushButton *btnAddNew = new QPushButton(tr("添加新组"), this);
    connect(btnAddNew, SIGNAL(clicked()), this, SLOT(onBtnNewTimingBroadcastClicked()));
    mTimingBroadcastHolder = new MultiTableWidgets(8, 1, this, btnAddNew);
    QVBoxLayout *rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getSectionLabel(tr("定时广播"), this));
    rootBox->addWidget(mTimingBroadcastHolder);
    setLayout(rootBox);
}

void TimingBroadcastZone::onBtnNewTimingBroadcastClicked()
{
    NewEventNameDialog* dialog = new NewEventNameDialog(mZoneType, this);
    if (dialog->exec() == QDialog::Accepted) {
        EventModel::Plan plan;
        plan.id = "";
        plan.name = dialog->getName();
        plan.type = mZoneType;
        MainWindow* mainWindow = mSettingsWidget->getMainWindow();
        TimedBroadcastEditor* editor = (TimedBroadcastEditor*)mainWindow->getActivity(BaseWidget::eTimedBroadcastEditor);
        editor->loadData(plan);
        QMap<QString, QVariant> params;
        mainWindow->loadActivity(BaseWidget::eTimedBroadcastEditor, params);
    }
    delete dialog;
}

void TimingBroadcastZone::loadPlans()
{
    mTimingBroadcastHolder->clear(false, true);
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans();
    foreach (EventModel::Plan plan, plans) {
        LOG(Logger::Debug, "loadPlans->plan: %s\n", plan.name.toStdString().c_str());
        if (plan.type != mZoneType) {
            LOG(Logger::Debug, "loadPlans->plan type != mZoneType\n");
            continue;
        }
        EventModel::PlanData* planData = new EventModel::PlanData(plan);
        //QString text = "任务名称:" + plan.name + "\n" + "起始时间:" + plan.mStartTime;
        QString text = plan.name + "\n" + "起始时间:" + plan.mStartTime;
        PlanName* btnPlanName = new PlanName(text, mTimingBroadcastHolder);
        btnPlanName->setUserData(0, planData);
        connect(btnPlanName, SIGNAL(clicked()), this, SLOT(onBtnPlanClicked()));
        mTimingBroadcastHolder->appendWidget(btnPlanName);
        LOG(Logger::Debug, "loadPlans->plan, plan btn appended\n");
    }
    mTimingBroadcastHolder->refresh();
}

void TimingBroadcastZone::onBtnPlanClicked()
{
    QPushButton* sender = qobject_cast<QPushButton *>(QObject::sender());
    EventModel::PlanData* planData = (EventModel::PlanData*)sender->userData(0);
    QMenu* popupMenu = new QMenu(this);
    QAction* editAction = new QAction(tr("修改"), this);
    popupMenu->addAction(editAction);
    connect(editAction, SIGNAL(triggered()), this, SLOT(onPopupActionEditTriggered()));
    editAction->setData(planData->getData().id);

    QAction* deleteAction = new QAction(tr("删除"), this);
    popupMenu->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onPopupActionDeleteTriggered()));
    deleteAction->setData(planData->getData().id);
    popupMenu->show();
    QPoint pos = QCursor::pos();
    LOG(Logger::Debug, "pos(x, y) = %d,%d\n", pos.x(), pos.y());
    popupMenu->move(pos);
}

void TimingBroadcastZone::onPopupActionEditTriggered()
{
    QAction* action = qobject_cast<QAction *>(QObject::sender());
    QString planId = action->data().toString();
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans(planId);
    if (!plans.contains(planId)){
        LOG(Logger::Debug, "%s not found\n", planId.toStdString().c_str());
        return;
    }
    MainWindow* mainWindow = mSettingsWidget->getMainWindow();
    TimedBroadcastEditor* editor = (TimedBroadcastEditor*)mainWindow->getActivity(BaseWidget::eTimedBroadcastEditor);
    EventModel::Plan plan = plans.value(planId);
    editor->loadData(plan);
    QMap<QString, QVariant> params;
    mainWindow->loadActivity(BaseWidget::eTimedBroadcastEditor, params);
}

void TimingBroadcastZone::onPopupActionDeleteTriggered()
{
    QAction* action = qobject_cast<QAction *>(QObject::sender());
    QString planId = action->data().toString();
    RPCCommand::deleteEventPlan(planId);
    loadPlans();
}

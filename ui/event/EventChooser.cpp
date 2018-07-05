#include "EventChooser.h"
#include "ui/base/multitablewidgets.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"

EventChooser::EventChooser(QWidget *parent) :
    QDialog(parent)
{
    setMinimumSize(400, 300);
    QVBoxLayout* rootbox = new QVBoxLayout;
    setLayout(rootbox);
    QLabel* title = new QLabel(tr("事件处理"), this);
    rootbox->addWidget(title, 1);

    QHBoxLayout* content = new QHBoxLayout;
    rootbox->addLayout(content, 7);

    MultiTableWidgets* eventPlanTab = new MultiTableWidgets(8, 2, this);
    eventPlanTab->setHeader(MiscComponents::getSectionLabel(tr("一般事件新增列表"), eventPlanTab));
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans();
    foreach (EventModel::Plan plan, plans) {
        if (plan.type != EventModel::eNormal)
            continue;
        QPushButton* eventPlanBtn = new QPushButton(plan.name);
        EventModel::PlanData* data = new EventModel::PlanData(plan);
        eventPlanBtn->setUserData(0, data);
        eventPlanTab->appendWidget(eventPlanBtn);
        connect(eventPlanBtn, SIGNAL(clicked()), this, SLOT(onEventPlanClicked()));
    }
    eventPlanTab->refresh();
    content->addWidget(eventPlanTab);

    MultiTableWidgets* eventHistoryTab = new MultiTableWidgets(8, 2, this);
    eventHistoryTab->setHeader(MiscComponents::getSectionLabel(tr("历史事件跟进列表"), eventHistoryTab));
    QList<EventModel::Event> events = RPCCommand::getEvent();
    foreach (EventModel::Event event, events) {
        QString planId = event.planId;
        if (!plans.contains(planId))
            continue;
        EventModel::Plan plan = plans.value(planId);
        if (plan.type != EventModel::eNormal)
            continue;
        QPushButton* eventBtn = new QPushButton(event.title);
        EventModel::EventData* data = new EventModel::EventData(event);
        eventBtn->setUserData(0, data);
        eventHistoryTab->appendWidget(eventBtn);
        connect(eventBtn, SIGNAL(clicked()), this, SLOT(onEventClicked()));
    }
    eventHistoryTab->refresh();
    content->addWidget(eventHistoryTab);
}

void EventChooser::onEventPlanClicked()
{
    QObject* object = QObject::sender();
    QPushButton* btnPlan = (QPushButton*)qobject_cast<QPushButton*>(object);
    EventModel::PlanData* planData = (EventModel::PlanData*)btnPlan->userData(0);
    EventModel::Plan plan = planData->getData();
    mWhichSelected = "plan";
    mSelectedId = plan.id;
    accept();
    LOG(Logger::Debug, "onEventPlanClicked: plan: %s selected\n", plan.id.toStdString().c_str());
}

void EventChooser::onEventClicked()
{
    QObject* object = QObject::sender();
    QPushButton* btnEvent = (QPushButton*)qobject_cast<QPushButton*>(object);
    EventModel::EventData* eventData = (EventModel::EventData*)btnEvent->userData(0);
    EventModel::Event event = eventData->getData();
    mWhichSelected = "event";
    mSelectedId = event.id;
    accept();
    LOG(Logger::Debug, "onEventClicked: event %s selected\n", event.id.toStdString().c_str());
}

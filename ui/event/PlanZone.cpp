#include "PlanZone.h"
#include "ui/base/multitablewidgets.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "DescriptionZone.h"
#include "EventExtensionsZone.h"

PlanZone::PlanZone(EventWidget* eventWidget, QWidget *parent) :
    QWidget(parent),
    mEventWidget(eventWidget)
{
    QString sectionTitle;
    if (mEventWidget->getWidgetType() == EventModel::eEmergency)
        sectionTitle = tr("应急预案列表");
    else
        sectionTitle = tr("事件列表");
    /*
    if (mEventWidget->getWidgetType() == EventModel::eEmergency)
        mHolder = new MultiTableWidgets(16, 4, this);
    else {
        QPushButton* btnAllExten = new QPushButton(tr("所有分机"), this);
        mHolder = new MultiTableWidgets(16, 4, this, btnAllExten);
    }
    */
    mHolder = new MultiTableWidgets(16, 4, this);
    mHolder->setHeader(MiscComponents::getSectionLabel(sectionTitle, this));

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(mHolder);
    setLayout(rootBox);

    //loadData();
}

void PlanZone::loadData(QString planid)
{
    mSelectedPlan.id = "";
    mHolder->clear(false, true);
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans();
    foreach (EventModel::Plan plan, plans.values()) {
        if (plan.type != mEventWidget->getWidgetType())
            continue;
        if (plan.id == planid)
            mSelectedPlan = plan;
        QPushButton* btnPlan = new QPushButton(plan.name, mHolder);
        EventModel::PlanData* planData = new EventModel::PlanData(plan);
        btnPlan->setUserData(0, planData);
        connect(btnPlan, SIGNAL(clicked()), this, SLOT(onBtnPlanClicked()));
        mHolder->appendWidget(btnPlan);
    }
    mHolder->refresh();
}

void PlanZone::onBtnPlanClicked()
{
    QPushButton* sender = qobject_cast<QPushButton *>(QObject::sender());
    EventModel::PlanData* planData = (EventModel::PlanData*)sender->userData(0);
    if (mSelectedPlan == planData->getData()) //如果点击的是之前已经选中的，则不处理
        return;
    //TODO: 如果eventid非空，则需要提示用户，这样选择会导致创建新的事件，是否继续
    if (planData->getData().id != "") {
        if (mSelectedPlan.id != "") {
            NewEventAlerting* dialog = new NewEventAlerting(this);
            if (dialog->exec() == QDialog::Accepted) {
                mSelectedPlan = planData->getData();
                mEventWidget->eventPlanSelected();
            }
            delete dialog;
        } else {
            mSelectedPlan = planData->getData();
            mEventWidget->eventPlanSelected();
        }
    }
}

NewEventAlerting::NewEventAlerting(QWidget *parent) :
    QDialog(parent)
{
    QString title = tr("创建新事件");
    setWindowTitle(title);

    QVBoxLayout* rootBox = new QVBoxLayout;
    QLabel* note = new QLabel(tr("此操作将创建一个新事件，您确定？"), this);
    rootBox->addWidget(note);

    QHBoxLayout* hbox = new QHBoxLayout;
    QPushButton* btnOk = new QPushButton(tr("确定"), this);
    QPushButton* btnCancel = new QPushButton(tr("取消"), this);
    hbox->addWidget(btnOk);
    hbox->addWidget(btnCancel);

    rootBox->addLayout(hbox);

    setLayout(rootBox);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

void NewEventAlerting::onOk()
{
    accept();
}

void NewEventAlerting::onCancel()
{
    reject();
}

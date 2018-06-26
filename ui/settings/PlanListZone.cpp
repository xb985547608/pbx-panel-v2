#include "PlanListZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "ui/eventplan/EventPlanEditor.h"
#include "ui/MainWindow.h"

PlanListZone::PlanListZone(EventModel::EventType eventType, SettingsWidget* settingsWidget, QWidget *parent) :
    QWidget(parent), mZoneType(eventType), mSettingsWidget(settingsWidget)
{
    QPushButton* btnAddNew = new QPushButton(tr("添加新组"), this);
    connect(btnAddNew, SIGNAL(clicked()), this, SLOT(onBtnNewEventClicked()));
    mPlanListHolder = new MultiTableWidgets(8, 2, this, btnAddNew);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(mPlanListHolder);
    setLayout(rootBox);

    if (eventType == EventModel::eNormal)
        mPlanListHolder->setHeader(MiscComponents::getSectionLabel(tr("一般事件"), mPlanListHolder));
    else
        mPlanListHolder->setHeader(MiscComponents::getSectionLabel(tr("应急预案"), mPlanListHolder));
}

void PlanListZone::loadPlans()
{
    mPlanListHolder->clear(false, true);
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans();
    foreach (EventModel::Plan plan, plans) {
        if (plan.type != mZoneType)
            continue;
        EventModel::PlanData* planData = new EventModel::PlanData(plan);
        PlanName* btnPlanName = new PlanName(plan.name, mPlanListHolder);
        btnPlanName->setUserData(0, planData);
        connect(btnPlanName, SIGNAL(clicked()), this, SLOT(onBtnPlanClicked()));
        mPlanListHolder->appendWidget(btnPlanName);
    }
    mPlanListHolder->refresh();
}

void PlanListZone::onBtnPlanClicked()
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

void PlanListZone::onPopupActionEditTriggered()
{
    QAction* action = qobject_cast<QAction *>(QObject::sender());
    QString planId = action->data().toString();
    QMap<QString, EventModel::Plan> plans = RPCCommand::getEventPlans(planId);
    if (!plans.contains(planId)) {
        LOG(Logger::Debug, "%s not found\n", planId.toStdString().c_str());
        return;
    }
    MainWindow* mainWindow = mSettingsWidget->getMainWindow();
    EventPlanEditor* editor = (EventPlanEditor*)mainWindow->getActivity(BaseWidget::eEventPlanEditor);
    EventModel::Plan plan = plans.value(planId);
    editor->loadData(plan);
    QMap<QString, QVariant> params;
    mainWindow->loadActivity(BaseWidget::eEventPlanEditor, params);
}

void PlanListZone::onPopupActionDeleteTriggered()
{
    QAction* action = qobject_cast<QAction *>(QObject::sender());
    QString planId = action->data().toString();
    RPCCommand::deleteEventPlan(planId);
    loadPlans();
}

void PlanListZone::onBtnNewEventClicked()
{
    NewEventNameDialog* dialog = new NewEventNameDialog(mZoneType, this);
    if (dialog->exec() == QDialog::Accepted) {
        EventModel::Plan plan;
        plan.id = "";
        plan.name = dialog->getName();
        plan.type = mZoneType;
        MainWindow* mainWindow = mSettingsWidget->getMainWindow();
        EventPlanEditor* editor = (EventPlanEditor*)mainWindow->getActivity(BaseWidget::eEventPlanEditor);
        editor->loadData(plan);
        QMap<QString, QVariant> params;
        mainWindow->loadActivity(BaseWidget::eEventPlanEditor, params);
    }
    delete dialog;
}

PlanName::PlanName(QString text, QWidget *parent) :
    QPushButton(text, parent){}

PlanName::~PlanName()
{
    LOG(Logger::Debug, "PlanName btn deleting.\n");
}

NewEventNameDialog::NewEventNameDialog(EventModel::EventType type, QWidget *parent) :
    QDialog(parent)
{
    //setAttribute(Qt::WA_DeleteOnClose);
    QString title = tr("请输入%1名称");
    if (type == EventModel::eNormal)
        title = title.arg("事件");
    else
        title = title.arg("预案");
    setWindowTitle(title);
    mTxtName = new QLineEdit(this);
    QPushButton* btnOk = new QPushButton(tr("确定"));
    QPushButton* btnCancel = new QPushButton(tr("取消"));

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->addWidget(mTxtName);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(btnOk, 1);
    hbox->addWidget(btnCancel, 1);
    rootBox->addLayout(hbox);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    setLayout(rootBox);
}

void NewEventNameDialog::onCancel()
{
    reject();
}

void NewEventNameDialog::onOk()
{
    QString name = mTxtName->text();
    if (name == "")
        return;
    //emit eventNameConfirmed(name);
    accept();
}

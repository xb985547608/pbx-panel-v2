#include "EventWidget.h"
#include "misc/Config.h"
#include "PlanZone.h"
#include "FunctionZone.h"
#include "DescriptionZone.h"
#include "EventExtensionsZone.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "ui/mainwidget/QueueZone.h"
#include "ui/dialogs/SMSWidget.h"
#include "ui/dialogs/PlaySoundWidget.h"
#include "unistd.h"

EventWidget::EventWidget(EventModel::EventType type, MainWindow *mainWindow, QWidget *parent) :
    BaseWidget(
        (type == EventModel::eEmergency) ? tr("应急预案") : tr("事件处理"),
        (type == EventModel::eEmergency) ? Config::Instance()->Res_icon_emergency : Config::Instance()->Res_icon_calendar,
        (type == EventModel::eEmergency) ? QColor(230,138,112) : QColor(234,160,115),
        (type == EventModel::eEmergency) ? eEmergency : eEvent,
        eMainWidget,
        mainWindow,
        parent),
    mEventType(type)
{
    initFrames();
    if (mEventType == EventModel::eEmergency)
        mDescriptionZone->enableSaveBtn(false);
}

void EventWidget::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(2);
    hbox->addWidget(getLeftFrame(), 4);
    hbox->addWidget(getRightFrame(), 6);
    mFrameRootBox->addLayout(hbox);
}

QWidget* EventWidget::getLeftFrame()
{
    QWidget* leftFrame = new QWidget(this);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->setSpacing(0);
    leftFrame->setLayout(rootBox);
    mPlanZone = new PlanZone(this, this);
    //mPlanZone->loadData();
    rootBox->addWidget(mPlanZone, 2);

    if (mEventType == EventModel::eEmergency)
        mFunctionZone = new FunctionZoneEmergency(this, this);
    else
        mFunctionZone = new FunctionZoneNormal(this, this);
    rootBox->addWidget(mFunctionZone, 1);
    return leftFrame;
}

QWidget* EventWidget::getRightFrame()
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);

    mEventExtensionsZone = new EventExtensionsZone(this, this);
    rootBox->addWidget(mEventExtensionsZone, 2);

    mDescriptionZone = new DescriptionZone(this, this);
    rootBox->addWidget(mDescriptionZone, 1);

    QWidget* rightFrame = new QWidget(this);
    rightFrame->setLayout(rootBox);
    return rightFrame;
}

void EventWidget::onResume(QMap<QString, QVariant>& param)
{
    mEventId = "";
    mDescriptionZone->setText("");
    mDescriptionZone->setRecorder("");
    mDescriptionZone->setDateTimeLabel("");
    mDescriptionZone->setTitleTxt("");
    QString planid;
    if (param.contains("planid"))
        planid = param.value("planid").toString();
    QString eventid;
    if (param.contains("eventid"))
        eventid = param.value("eventid").toString();
    mPlanZone->loadData(planid);
    if (planid != "")
        eventPlanSelected();
    else {
        QList<PBX::Extension> exts;
        mEventExtensionsZone->loadEventExtens(exts);
    }
    if (eventid != "") {
        QList<EventModel::Event> events = RPCCommand::getEvent(eventid);
        if (events.size() == 0)
            return;
        EventModel::Event event = events.at(0);
        mDescriptionZone->setTitleTxt(event.title);
        mDescriptionZone->setRecorder(event.recorder);
        mDescriptionZone->setText(event.description);
        mDescriptionZone->setDateTimeLabel(event.created_date.toString("yyyy-MM-dd HH:mm:ss"));
        mEventId = event.id;
    }
    QMap<QString, QVariant> bParam;
    BaseWidget::onResume(bParam);
}

void EventWidget::eventPlanSelected()
{
    mEventId = "";
    EventModel::Plan selectedPlan = mPlanZone->getSelectedPlan();
    if (selectedPlan.type == EventModel::eEmergency)
        getDescriptionZone()->enableSaveBtn(false);
    //更新右侧描述窗口
    getDescriptionZone()->setTitleTxt(selectedPlan.name);
    //读取plan中分机信息，更新右侧分机列表
    QList<PBX::Extension> pbxExtens2Load;
    QStringList phonebookIds;
    foreach (QString id, selectedPlan.numbers.keys())
        phonebookIds.append(id);
    QMap<QString, PhoneBook> records = RPCCommand::getPhonebookById(phonebookIds);
    foreach(QString id, selectedPlan.numbers.keys()) {
        if (!records.contains(id))
            continue;
        QString numbers = selectedPlan.numbers.value(id);
        if (!numbers.contains("E"))
            continue;
        PhoneBook recordInPhonebook = records.value(id);
        //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(recordInPhonebook.extenNumber);
        QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(recordInPhonebook.extenNumber);
        if (!extens.contains(recordInPhonebook.extenNumber))
            continue;
        PBX::Extension pbxExten = extens.value(recordInPhonebook.extenNumber);
        pbxExtens2Load.append(pbxExten);
    }
    getEventExtensionsZone()->loadEventExtens(pbxExtens2Load);
}

bool EventWidget::saveEvent()
{
    EventModel::Plan selectedPlan = mPlanZone->getSelectedPlan();
    if (selectedPlan.id == "") {
        QMessageBox::about(this, tr("错误"), tr("请选择事件/预案"));
        return false;
    }
    if (selectedPlan.type == EventModel::eEmergency)
        getDescriptionZone()->enableSaveBtn(true);
    //mEventId = RPCCommand::updateEvent(mEventId, selectedPlan.id, selectedPlan.name, mDescriptionZone->getText(), mDescriptionZone->getRecorder());
    mEventId = RPCCommand::updateEvent(mEventId, selectedPlan.id, mDescriptionZone->getTitleTxt(), mDescriptionZone->getText(), mDescriptionZone->getRecorder());
    return true;
}

void EventWidget::makeCall(bool chase) {
    if (mEventType == EventModel::eNormal) {
        if (mEventId == "") {
            QMessageBox::about(this, tr("错误"), tr("请先保存事件"));
            return;
        }
        if (!saveEvent())
            return;
    }
    QStringList channels2Call = getChannels2Call();
    if (channels2Call.size() == 0)
        return;
    Config* cfg = Config::Instance();
    QString opChannel;
    if (QueueZone::getQueueNumber() == "")
        opChannel = "LOCAL/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
    else
        opChannel = "LOCAL/" + QueueZone::getQueueNumber() + "@" + cfg->OperatorQueueContext + "/n";
    QString callerid = "\"OPGROUP\"&lt;" + cfg->DefaultCallerId + "&gt;";
    QString roomid = cfg->getGroupcallNumber();
    QString context = cfg->OperatorGroupCallContext;
    //呼叫管理员
    RPCCommand::makeConferenceCall(opChannel, context, cfg->DefaultCallerId, callerid, roomid, true, 3000000);
    //呼叫其他号码
    foreach (QString channel, channels2Call) {
        if (!chase)
            RPCCommand::makeConferenceCall(channel, context, cfg->DefaultCallerId, callerid, roomid, false, 3000000);
        else {
            //CONF_NO=%5|IS_OPERATOR=%6|RECORD_FILE=M-%7-%8
            QMap<QString, QString> vars;
            vars.insert("CONF_NO", roomid);
            vars.insert("IS_OPERATOR", "0");
            vars.insert("RECORD_FILE", QString("M-%1-%2").arg(roomid).arg(Config::Instance()->uptime));
            RPCCommand::chaseCall(channel, context, cfg->DefaultCallerId, callerid, vars);
            usleep(1000);
        }
    }
}

QStringList EventWidget::getChannels2Call()
{
    QStringList channelsToCall;
    Config* cfg = Config::Instance();
    EventModel::Plan plan = mPlanZone->getSelectedPlan();
    QStringList phonebookIds = plan.numbers.keys();
    QMap<QString, PhoneBook> phonebook = RPCCommand::getPhonebookById(phonebookIds);
    foreach (QString id, phonebook.keys()) {
        if (plan.numbers.contains(id)) {
            PhoneBook record = phonebook.value(id);
            QString numbers = plan.numbers.value(id);
            QString channel;
            //通讯录id-E-M-H-D
            if (numbers.contains("E")) {
                channel = "SIP/" + record.extenNumber;
                channelsToCall.append(channel);
            }
            if (numbers.contains("M")) {
                channel = "LOCAL/" + record.mobile + "@" + cfg->DefaultContext + "/n";
                channelsToCall.append(channel);
            }
            if (numbers.contains("H")) {
                channel = "LOCAL/" + record.homeNumber + "@" + cfg->DefaultContext + "/n";
                channelsToCall.append(channel);
            }
            if (numbers.contains("D")) {
                channel = "LOCAL/" + record.directdid + "@" + cfg->DefaultContext + "/n";
                channelsToCall.append(channel);
            }
        }
    }
#ifndef QT_NO_DEBUG
    LOG(Logger::Debug, "channels to call: ");
    foreach (QString channel, channelsToCall)
        LOG(Logger::Debug, "%s\t", channel.toStdString().c_str());
    LOG(Logger::Debug, "\n");
#endif
    return channelsToCall;
}

void EventWidget::broadcast()
{
    if (mEventType == EventModel::eNormal) {
        if (mEventId == "") {
            QMessageBox::about(this, tr("错误"), tr("请先保存事件"));
            return;
        }
        if (!saveEvent())
            return;
    }
    EventModel::Plan plan = mPlanZone->getSelectedPlan();
    if (plan.terminals.size() == 0) {
        LOG(Logger::Warn, "Plan(%s) has no terminal to broadcast\n", plan.id.toStdString().c_str());
        return;
    }
    Config* cfg = Config::Instance();
    QString channel = "";
    if (QueueZone::getQueueNumber() == "")
            channel = "LOCAL/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
        else
            channel = "LOCAL/" + QueueZone::getQueueNumber() + "@" + cfg->OperatorQueueContext + "/n";
    QString callerId = "\"OPGROUP\"&lt;" + cfg->DefaultCallerId + "&gt;";
    QString context = "app-operator-page";
    QString exten = "10000";
    QString varDS = "";
    foreach (QString terminal, plan.terminals)
        varDS += "SIP/" + terminal + "&";
    //去掉最后一个'&'
    varDS = varDS.left(varDS.lastIndexOf("&"));
    LOG(Logger::Debug, "broadcast to channels: %s\n", varDS.toStdString().c_str());
    varDS = varDS.replace("&", "&amp;");
    QMap<QString, QString> variables;
    variables.insert("DS", varDS);
    RPCCommand::originate(channel, context, exten, callerId, variables);
}

void EventWidget::playSound()
{
    if (mEventType == EventModel::eNormal) {
        if (mEventId == "") {
            QMessageBox::about(this, tr("错误"), tr("请先保存事件"));
            return;
        }
        if (!saveEvent())
            return;
    }
    EventModel::Plan plan = mPlanZone->getSelectedPlan();
    if (plan.broadcastFiles.size() == 0) {
        LOG(Logger::Warn, "Plan(%s) has no broadcast file\n", plan.id.toStdString().c_str());
        return;
    }
    if (plan.terminals.size() == 0) {
        LOG(Logger::Warn, "Plan(%s) has no terminal to paly sound\n", plan.id.toStdString().c_str());
        return;
    }
    QString soundFile = plan.broadcastFiles.at(0);
    soundFile = soundFile.left(soundFile.lastIndexOf("."));
    LOG(Logger::Debug, "playSound file %s\n", soundFile.toStdString().c_str());
    int loopCnt = 9999999;
    if (mEventType == EventModel::eNormal) {
        PlaySoundWidget* playSoundDialog = new PlaySoundWidget(this, false);
        if (playSoundDialog->exec() == QDialog::Accepted)
            loopCnt = playSoundDialog->getLoopCount();
        else
            loopCnt = 0;
        delete playSoundDialog;
        if (loopCnt == 0) {
            LOG(Logger::Debug, "Sound play for normal event has been canceled");
            return;
        }
    }
    QString channel = "Local/10000@app-operator-page/n";
    QString context = "app-operator-page";
    QString exten = "10010";
    QString& callerId = Config::Instance()->SoundPlayerCallerId;
    QString varDS = "";
    foreach (QString terminal, plan.terminals)
        varDS += "SIP/" + terminal + "&";
    //去掉最后一个'&'
    varDS = varDS.left(varDS.lastIndexOf("&"));
    LOG(Logger::Debug, "playSound to terminal: %s\n", varDS.toStdString().c_str());
    varDS = varDS.replace("&", "&amp;");
    QMap<QString, QString> variables;
    variables.insert("DS", varDS);
    variables.insert("ROUNDS", QString("%1").arg(loopCnt));
    variables.insert("PAGE_FILE", soundFile);
    RPCCommand::originate(channel, context, exten, callerId, variables);
}

void EventWidget::sendSms()
{
    if (mEventType == EventModel::eNormal) {
        if (mEventId == "") {
            QMessageBox::about(this, tr("错误"), tr("请先保存事件"));
            return;
        }
        if (!saveEvent())
            return;
    }
    QStringList number2Send;
    EventModel::Plan plan = mPlanZone->getSelectedPlan();
    QStringList phonebookIds = plan.numbers.keys();
    QMap<QString, PhoneBook> phonebook = RPCCommand::getPhonebookById(phonebookIds);
    foreach (QString id, phonebook.keys()) {
        if (plan.numbers.contains(id)) {
            PhoneBook record = phonebook.value(id);
            QString numbers = plan.numbers.value(id);
            if (numbers.contains("E"))
                number2Send.append(record.extenNumber);
        }
    }
    if (number2Send.size() == 0) {
        LOG(Logger::Warn, "plan(%s) has no extens to send message\n", plan.id.toStdString().c_str());
        return;
    }
    Config* cfg = Config::Instance();
    QString sms = plan.sms;
    if (mEventType == EventModel::eNormal) {
        SMSWidget* smsDialog = new SMSWidget(this, false);
        smsDialog->setSms(plan.sms);
        if (smsDialog->exec() != QDialog::Accepted)
            return;
        sms = smsDialog->getSms();
        if (sms == "")
            return;
    }
    RPCCommand::sendSms(cfg->DefaultCallerIdName, number2Send, sms);
#ifndef QT_NO_DEBUG
    LOG(Logger::Debug, "send sms: ");
    foreach (QString number, number2Send)
        LOG(Logger::Debug, "%s,", number.toStdString().c_str());
    LOG(Logger::Debug, "\n");
#endif
}

void EventWidget::hangupAll()
{
    if (mEventId == "" || !saveEvent())
        return;
    LOG(Logger::Debug, "hangupAll\n");
    //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed();
    QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail();
    foreach (PBX::Extension exten, extens) {
        QList<QString> channels = exten.peers.keys();
        foreach (QString channel, channels)
            RPCCommand::hangup(channel);
    }
}

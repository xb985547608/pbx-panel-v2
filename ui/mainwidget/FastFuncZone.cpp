#include "FastFuncZone.h"
#include "misc/Config.h"
#include "ui/base/misccomponents.h"
#include "RinggroupsZone.h"
#include "SelectedExtensZone.h"
#include "ExtensionsZone.h"
#include "ui/dialogs/DialPad.h"
#include "ui/dialogs/SMSWidget.h"
#include "ui/dialogs/PlaySoundWidget.h"
#include "misc/logger.h"
#include "message_queue/RPCCommand.h"
#include "QueueZone.h"
#include "ui/MainWindow.h"
#include "ui/phonebook/UiPhoneBookList.h"
#include "OperatorZone.h"

FastFuncZone::FastFuncZone(MainWidget *mainWidget, QWidget *parent) :
    QWidget(parent), mMainWidget(mainWidget)
{
    Config* cfg = Config::Instance();

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setSpacing(0);
    hbox->setMargin(0);
    QGroupBox* groupbox = MiscComponents::getGroupBox(hbox, parent);


    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setSpacing(0);
    rootBox->setMargin(0);
    rootBox->addWidget(groupbox);
    setLayout(rootBox);

    //呼叫按钮
    {
        mBtnGroupCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnGroupCall, cfg->Res_icon_make_call, tr("呼叫"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnGroupCall, SIGNAL(clicked()), this, SLOT(onBtnGroupCallClicked()));
        hbox->addWidget(mBtnGroupCall);
    }
    //紧急呼叫按钮
    {
        mBtnEmergensyCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnEmergensyCall, cfg->Res_icon_make_call, tr("紧急呼叫"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnEmergensyCall, SIGNAL(clicked()), this, SLOT(onBtnEmergensyCallClicked()));
        hbox->addWidget(mBtnEmergensyCall);
    }
    //短信按钮
    {
        mBtnSms = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnSms, cfg->Res_icon_sms_square, tr("短信"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnSms, SIGNAL(clicked()), this, SLOT(onBtnSmsClicked()));
        hbox->addWidget(mBtnSms);
    }
    /*
    //"当前通话"按钮和checkbox
    {
        mBtnCurrentCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnCurrentCall, cfg->Res_icon_current_call, tr("当前通话"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnCurrentCall, SIGNAL(clicked()), this, SLOT(onBtnCurrentCallClicked()));
        hbox->addWidget(mBtnCurrentCall);
    }
    */
    //转接到三方通话按钮
    {
        mBtn3Way = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtn3Way, cfg->Res_icon_threeway, tr("三方通话"), Qt::ToolButtonTextUnderIcon);
        connect(mBtn3Way, SIGNAL(clicked()), this, SLOT(onBtn3WayClicked()));
        hbox->addWidget(mBtn3Way);
        mBtn3Way->setEnabled(false);
    }
    /*
    //通讯录按钮
    {
        mBtnPhoneBook = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPhoneBook, cfg->Res_icon_phone_book, tr("通讯录"), Qt::ToolButtonTextUnderIcon);
        //TODO: connect signal
        hbox->addWidget(mBtnPhoneBook);
    }
    */
    //广播
    {
        mBtnBroadcast = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnBroadcast, cfg->Res_icon_broadcast, tr("广播"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnBroadcast, SIGNAL(clicked()), this, SLOT(onBtnBroadcastClicked()));
        hbox->addWidget(mBtnBroadcast);
    }
    //扩音
    {
        mBtnPaging = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPaging, cfg->Res_icon_play_record, tr("录音播放"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnPaging, SIGNAL(clicked()), this, SLOT(onBtnPagingClicked()));
        hbox->addWidget(mBtnPaging);
    }
    //监听
    {
        mBtnMonitor = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnMonitor, cfg->Res_icon_monitor, tr("监听"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnMonitor, SIGNAL(clicked()), this, SLOT(onBtnMonitorClicked()));
        hbox->addWidget(mBtnMonitor);
    }
    //强拆
    {
        mBtnHangup = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnHangup, cfg->Res_icon_hangup, tr("强拆"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnHangup, SIGNAL(clicked()), this, SLOT(onBtnHangupClicked()));
        hbox->addWidget(mBtnHangup);
    }
    //强插
    {
        mBtnOverride = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnOverride, cfg->Res_icon_override, tr("强插"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnOverride, SIGNAL(clicked()), this, SLOT(onBtnOverrideClicked()));
        hbox->addWidget(mBtnOverride);
    }
    //转接
    {
        mBtnTransfer = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnTransfer, cfg->Res_icon_transfer, tr("转接"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnTransfer, SIGNAL(clicked()), this, SLOT(onBtnTransferClicked()));
        hbox->addWidget(mBtnTransfer);
    }
    //代接
    {
        mBtnPickup = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPickup, cfg->Res_icon_pickup, tr("代接"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnPickup, SIGNAL(clicked()), this, SLOT(onBtnPickupClicked()));
        hbox->addWidget(mBtnPickup);
    }
    //点名
    {
        mBtnRollCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnRollCall, cfg->Res_icon_rollcall, tr("点名"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnRollCall, SIGNAL(clicked()), this, SLOT(onBtnRollCallClicked()));
        hbox->addWidget(mBtnRollCall);
    }
    //轮询
    {
        mBtnPoll = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPoll, cfg->Res_icon_poll, tr("轮询"), Qt::ToolButtonTextUnderIcon);
        //todo: CONNECT signal
        hbox->addWidget(mBtnPoll);
    }
}

void FastFuncZone::selectedExtenCntChanged(int count)
{
    switch (count) {
    case 0:
        mBtnSms->setEnabled(false);
        //mBtnPhoneBook
        //mBtnCurrentCal
        //mBtnGroupCall->setEnabled(false);
        mBtnBroadcast->setEnabled(false);
        mBtnPaging->setEnabled(false);
        mBtnMonitor->setEnabled(false);
        mBtnHangup->setEnabled(false);
        mBtnOverride->setEnabled(false);
        mBtnTransfer->setEnabled(false);
        mBtnPickup->setEnabled(false);
        mBtnRollCall->setEnabled(false);
        mBtnPoll->setEnabled(false);
        break;
    case 1:
        mBtnSms->setEnabled(true);
        //mBtnPhoneBook
        //mBtnCurrentCal
        //mBtnGroupCall->setEnabled(true);
        mBtnBroadcast->setEnabled(true);
        mBtnPaging->setEnabled(true);
        mBtnMonitor->setEnabled(true);
        mBtnHangup->setEnabled(true);
        mBtnOverride->setEnabled(true);
        mBtnTransfer->setEnabled(true);
        mBtnPickup->setEnabled(true);
        mBtnRollCall->setEnabled(true);
        mBtnPoll->setEnabled(false);
        break;
    default:
        mBtnSms->setEnabled(true);
        //mBtnPhoneBook
        //mBtnCurrentCal
        mBtnGroupCall->setEnabled(true);
        mBtnBroadcast->setEnabled(true);
        mBtnPaging->setEnabled(true);
        mBtnMonitor->setEnabled(false);
        mBtnHangup->setEnabled(true);
        mBtnOverride->setEnabled(false);
        mBtnTransfer->setEnabled(false);
        mBtnPickup->setEnabled(false);
        mBtnRollCall->setEnabled(true);
        mBtnPoll->setEnabled(false);
        break;
    }
}

void FastFuncZone::onBtnSmsClicked()
{
    SMSWidget* smsDialog = new SMSWidget(this);
    if (smsDialog->exec() == QDialog::Accepted) {
        QString sms = smsDialog->getSms();
        LOG(Logger::Debug, "sms to send: %s\n", sms.toStdString().c_str());
        QList<PBX::Extension> pbxExtens = mMainWidget->getSelectedExtensZone()->getExtens();
        QStringList toList;
        foreach (PBX::Extension e, pbxExtens) {
            toList.append(e.number);
            mMainWidget->getSelectedExtensZone()->removeExten(e);
        }
        if (toList.size() > 0)
            RPCCommand::sendSms("Operator", toList, sms.toUtf8());
    }
    delete smsDialog;
}

/*
void FastFuncZone::onBtnPhoneBookClicked()
{
}
*/

void FastFuncZone::onBtnCurrentCallClicked()
{
    mMainWidget->getRinggroupsZone()->toggleGroup(Config::Instance()->CurrentCallGroupNumber);
}

void FastFuncZone::onBtnGroupCallClicked()
{
    Config* cfg = Config::Instance();
    QString opChannel;
    if (QueueZone::getQueueNumber() == "")
        opChannel = "LOCAL/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
    else
        opChannel = "LOCAL/" + QueueZone::getQueueNumber() + "@" + cfg->OperatorQueueContext + "/n";
    QString callerid = "\"OPGROUP\"&lt;" + cfg->DefaultCallerId + "&gt;";
    QString roomid = cfg->getGroupcallNumber();
    QString context = cfg->OperatorGroupCallContext;
    //如果未选中分机，则弹出拨号窗口
    if (mMainWidget->getSelectedExtensZone()->getCount() == 0) {
        DialPad* dialPad = new DialPad();
        if (dialPad->exec() == QDialog::Accepted) {
            if (dialPad->isPhonebookSelected()) {
                mMainWidget->onBtnPhonebookClicked();
            } else {
                QString dialedNumber = dialPad->getNumber();
                LOG(Logger::Debug, "dialed number: %s\n", dialedNumber.toStdString().c_str());
                //对单个号码，发起点对点呼叫
                QMap<QString, QString> variables;
                RPCCommand::originate(opChannel, cfg->DefaultContext, dialedNumber, cfg->DefaultCallerId, variables);
                /*
                //呼叫管理员
                RPCCommand::makeConferenceCall(opChannel, context, cfg->DefaultCallerId, roomid, roomid, true);
                //呼叫被叫号
                QString channel;
                QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(dialedNumber);
                if (extens.contains(dialedNumber)) {
                    PBX::Extension e = extens.value(dialedNumber);
                    channel = e.tech + "/" + e.number;
                } else
                    channel = "LOCAL/" + dialedNumber + "@" + cfg->DefaultContext + "/n";
                RPCCommand::makeConferenceCall(channel, context, dialedNumber, roomid, roomid, false);
                */
            }
        }
        delete dialPad;
    } else {
        QList<PBX::Extension> extens = mMainWidget->getSelectedExtensZone()->getExtens();
        if (extens.size() == 0)
            return;
        //对单个号码，发起点对点呼叫
        if (extens.size() == 1) {
            PBX::Extension e = extens.at(0);
            QString dialedNumber = e.number;
            QMap<QString, QString> variables;
            RPCCommand::originate(opChannel, cfg->DefaultContext, dialedNumber, e.number, variables);
            mMainWidget->getSelectedExtensZone()->removeExten(e);
            return;
        }
        //呼叫管理员
        RPCCommand::makeConferenceCall(opChannel, context, cfg->DefaultCallerId, roomid, roomid, true);
        //呼叫被叫号
        foreach (PBX::Extension e, extens) {
            QString channel = e.tech + "/" + e.number;
            RPCCommand::makeConferenceCall(channel, context, e.number, roomid, roomid, false);
            mMainWidget->getSelectedExtensZone()->removeExten(e);
        }
    }
}

void FastFuncZone::onBtnBroadcastClicked()
{
    Config* cfg = Config::Instance();
    QString channel;
    if (QueueZone::getQueueNumber() == "")
        channel ="Local/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
    else
        channel ="Local/" + QueueZone::getQueueNumber() + "@" + cfg->OperatorQueueContext + "/n";
    QString context = "app-operator-page";
    QString exten = "10000";
    QString& callerId = cfg->DefaultCallerId;
    QString varDS;
    QList<PBX::Extension> extens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (extens.size() == 0)
        return;
    foreach (PBX::Extension ext, extens) {
        //QString dsExten = ext.tech + "/" + ext.number;
        QString dsExten = "Local/" + ext.number + "@from-internal";
        varDS += dsExten + "&";
        mMainWidget->getSelectedExtensZone()->removeExten(ext);
    }
    //去掉最后一个'&'
    varDS = varDS.left(varDS.lastIndexOf("&"));
    varDS = varDS.replace("&", "&amp;");
    QMap<QString, QString> variables;
    variables.insert("DS", varDS);
    RPCCommand::originate(channel, context, exten, callerId, variables);
}

void FastFuncZone::onBtnPagingClicked()
{
    QString soundFile = "";
    int loopCnt = 0;
    PlaySoundWidget* playWidget = new PlaySoundWidget;
    if (playWidget->exec() == QDialog::Accepted) {
        loopCnt = playWidget->getLoopCount();
        soundFile = playWidget->getSoundFile();
        LOG(Logger::Debug, "%s will be play for %d times\n", soundFile.toStdString().c_str(), loopCnt);
    }
    delete playWidget;
    soundFile = soundFile.left(soundFile.lastIndexOf("."));
    if (soundFile == "" || loopCnt == 0)
        return;
    QString channel = "Local/10000@app-operator-page/n";
    QString context = "app-operator-page";
    QString exten = "10010";
    QString& callerId = Config::Instance()->SoundPlayerCallerId;
    QString varDS = "";
    QList<PBX::Extension> extens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (extens.size() == 0)
        return;
    foreach (PBX::Extension ext, extens) {
        //QString dsExten = ext.tech + "/" + ext.number;
        QString dsExten = "Local/" + ext.number + "@from-internal";
        varDS += dsExten + "&";
        mMainWidget->getSelectedExtensZone()->removeExten(ext);
    }
    //去掉最后一个'&'
    varDS = varDS.left(varDS.lastIndexOf("&"));
    varDS = varDS.replace("&", "&amp;");
    QMap<QString, QString> variables;
    variables.insert("DS", varDS);
    variables.insert("ROUNDS", QString("%1").arg(loopCnt));
    variables.insert("PAGE_FILE", soundFile);
    RPCCommand::originate(channel, context, exten, callerId, variables);
}

void FastFuncZone::onBtnMonitorClicked()
{
    chanSpy(false);
}

void FastFuncZone::onBtnHangupClicked()
{
    QList<PBX::Extension> extens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (extens.size() == 0)
        return;
    foreach (PBX::Extension e, extens) {
        mMainWidget->getSelectedExtensZone()->removeExten(e);
        //QMap<QString, PBX::Extension> extensDetail = RPCCommand::getExtensionDetailed(e.number);
        QMap<QString, PBX::Extension> extensDetail = PBX::Instance()->getExtensionDetail(e.number);
        if (!extensDetail.contains(e.number))
            continue;
        PBX::Extension extenDetail = extensDetail.value(e.number);
        if (extenDetail.peers.size() == 0)
            continue;
        foreach (PBX::PeerInfo peer, extenDetail.peers.values()) {
            QString channel = peer.channel;
            RPCCommand::hangup(channel);
        }
    }
}

void FastFuncZone::onBtnOverrideClicked()
{
    chanSpy(true);
}

void FastFuncZone::onBtnTransferClicked()
{
    //找出分机
    QList<PBX::Extension> pbxExtens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (pbxExtens.size() != 1)
        return;
    QString number =pbxExtens.at(0).number;
    //QMap<QString, PBX::Extension> realtimeExtens = RPCCommand::getExtensionDetailed(number);
    QMap<QString, PBX::Extension> realtimeExtens = PBX::Instance()->getExtensionDetail(number);
    //取该分机实时状态
    if (!realtimeExtens.contains(number))
        return;
    PBX::Extension exten = realtimeExtens.value(number);
    //找出该分机第一个正在通话的channel
    QString channel = "";
    QString context = exten.context;
    foreach (PBX::PeerInfo peer, exten.peers.values())
        if (peer.state == PBX::eIncall) {
            channel = peer.channel;
            break;
        }
    if (channel == "")
        return;
    //弹出对话框等待调度员输入目的号码
    Config* cfg = Config::Instance();
    QString targetNumber = "";
    DialPad* dp = new DialPad();
    if (dp->exec() == QDialog::Accepted) {
        if (dp->isPhonebookSelected()) {
            QMap<QString, QVariant> param;
            param.insert("method", "redirect-call");
            param.insert("channel", channel);
            param.insert("context", context);
            param.insert("ringgroup-number", cfg->AllExtensionGroupNumber);
            param.insert("ringgroup-name", cfg->AllExtensionGroupName);
            param.insert("list-mode", UiPhoneBookList::eSelectSingle);
            param.insert("ok-btn-name", tr("转接"));
            mMainWidget->getMainWindow()->loadActivity(BaseWidget::ePhoneBook, param, BaseWidget::eMainWidget);
        } else {
            targetNumber = dp->getNumber();
            LOG(Logger::Debug, "call will be transfered to %s\n", targetNumber.toStdString().c_str());
        }
    }
    delete dp;
    if (targetNumber == "")
        return;
    RPCCommand::redirectCall(channel, context, targetNumber);
    mMainWidget->getSelectedExtensZone()->removeExten(pbxExtens.at(0));
}

void FastFuncZone::onBtnRollCallClicked()
{
    ExtensionsZone* ez = mMainWidget->getExtensionsZone();
    QList<PBX::Extension> selectedExtens = mMainWidget->getSelectedExtensZone()->getExtens();
    //通知所选分机改变点名图标
    foreach (PBX::Extension e, selectedExtens) {
        ez->extenRollCallResultSlot(e.number, PBX::eUnChecked);
    }
    //发起点名呼叫
    RPCCommand::makeRollCall(selectedExtens);
    //清除所选分机
    foreach (PBX::Extension e, selectedExtens) {
        mMainWidget->getSelectedExtensZone()->removeExten(e);
    }
}

void FastFuncZone::onBtn3WayClicked()
{
    PBX::Extension opExten = mMainWidget->getOperatorZone()->getSelectedOpExten();
    if (opExten.number == "")
        return;
    //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(opExten.number);
    QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(opExten.number);
    if (!extens.contains(opExten.number))
        return;
    opExten = extens.value(opExten.number);
    QString channel;
    QString extraChan;
    foreach (PBX::PeerInfo peer, opExten.peers.values()) {
        if (peer.state == PBX::eIncall) {
            channel = peer.channel;
            extraChan = peer.peerChannel;
            break;
        }
    }
    if (channel == "" || extraChan == "") {
        LOG(Logger::Debug, "3way calling: not enough channels");
        return;
    }
    Config* cfg = Config::Instance();
    QString targetNumber = "";
    DialPad* dp = new DialPad();
    if (dp->exec() == QDialog::Accepted) {
        if (dp->isPhonebookSelected()) {
            QMap<QString, QVariant> param;
            param.insert("method", "3way-call");
            param.insert("channel", channel);
            param.insert("extra-channel", extraChan);
            param.insert("context", cfg->OperatorBreakinContext);
            param.insert("ringgroup-number", cfg->AllExtensionGroupNumber);
            param.insert("ringgroup-name", cfg->AllExtensionGroupName);
            param.insert("list-mode", UiPhoneBookList::eSelectSingle);
            param.insert("ok-btn-name", tr("呼叫"));
            mMainWidget->getMainWindow()->loadActivity(BaseWidget::ePhoneBook, param, BaseWidget::eMainWidget);
        } else {
            targetNumber = dp->getNumber();
            LOG(Logger::Debug, "part(%s) will be invited to 3 way call\n", targetNumber.toStdString().c_str());
        }
    }
    delete dp;
    if (targetNumber == "")
        return;
    QString roomid = cfg->getGroupcallNumber();
    QMap<QString, QString> vars;
    //把第3方邀请进入会议
    QString thirdChan = "LOCAL/" + targetNumber + "@" + cfg->DefaultContext + "/n";
    RPCCommand::originate(thirdChan, cfg->OperatorBreakinContext, roomid, cfg->DefaultCallerId, vars);
    //把通话双发转入会议
    RPCCommand::redirectCall(channel, cfg->OperatorBreakinContext, roomid, extraChan);
}

void FastFuncZone::onBtnPickupClicked()
{
    //找出分机
    QList<PBX::Extension> pbxExtens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (pbxExtens.size() != 1)
        return;
    QString number = pbxExtens.at(0).number;
    //取该分机实时状态
    //QMap<QString, PBX::Extension> realtimeExtens = RPCCommand::getExtensionDetailed(number);
    QMap<QString, PBX::Extension> realtimeExtens = PBX::Instance()->getExtensionDetail(number);
    if (!realtimeExtens.contains(number))
        return;
    PBX::Extension exten = realtimeExtens.value(number);
    /*
    //找到该分机第一个处于ringing状态的对端channel
    QString peerChannel = "";
    foreach (PBX::PeerInfo peer, exten.peers.values())
        if (peer.state == PBX::eRinging) {
            peerChannel = peer.peerChannel;
            break;
        }
        */
    //查找该分机第一个处于ring状态的channel
    QString peerChannel = "";
    foreach (PBX::PeerInfo peer, exten.peers.values())
        if (peer.state == PBX::eRing) {
            peerChannel = peer.channel;
            break;
        }
    if (peerChannel == "")
        return;
    Config* cfg = Config::Instance();
    if (QueueZone::getQueueNumber() == "")
        RPCCommand::redirectCall(peerChannel, cfg->OperatorGroupContext, cfg->DefaultCallerId);
    else
        RPCCommand::redirectCall(peerChannel, cfg->OperatorQueueContext, QueueZone::getQueueNumber());
    mMainWidget->getSelectedExtensZone()->removeExten(pbxExtens.at(0));
}

void FastFuncZone::onBtnEmergensyCallClicked()
{
    mMainWidget->ShowPhonebook(true);
}

/*
void FastFuncZone::onBtnRollCallClicked();
void FastFuncZone::onBtnPollClicked();
*/


void FastFuncZone::chanSpy(bool whisper)
{
    /*
        [app-operator-chanspy]
        exten => 10010,1,NoOp(app-operator-chanspy)
        exten => 10010,n,Answer()
        exten => 10010,n,Wait(1)
        exten => 10010,n,ChanSpy(${CHAN_SPY_ON}|${SPY_OPT}) ;如果需要悄悄监听，就加上q选项
        exten => 10010,n,Hangup()
    */
    Config* cfg = Config::Instance();
    QList<PBX::Extension> extens = mMainWidget->getSelectedExtensZone()->getExtens();
    if (extens.size() != 1)
        return;
    PBX::Extension pbxExten = extens.at(0);
    //QMap<QString, PBX::Extension> realtimeExts = RPCCommand::getExtensionDetailed(pbxExten.number);
    QMap<QString, PBX::Extension> realtimeExts = PBX::Instance()->getExtensionDetail(pbxExten.number);
    if (!realtimeExts.contains(pbxExten.number))
        return;
    PBX::Extension realtimeExt = realtimeExts.value(pbxExten.number);
    if (realtimeExt.peers.size() == 0)
        return;
    //找出第一个在通话中的channel
    QString chanSpyOn = "";
    foreach (PBX::PeerInfo peer, realtimeExt.peers.values())
        if (peer.channel != "" && peer.state == PBX::eIncall) {
            chanSpyOn = peer.channel;
            break;
        }
    if (chanSpyOn == "")
        return;
    QString channel;
    if (QueueZone::getQueueNumber() == "")
        channel = "Local/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
    else
        channel = "Local/" + QueueZone::getQueueNumber() + "@" + cfg->OperatorQueueContext + "/n";
    QString context = "app-operator-chanspy";
    QString exten = "10010";
    QString& callerId = cfg->DefaultCallerId;
    QMap<QString, QString> variables;
    variables.insert("CHAN_SPY_ON", chanSpyOn);
    variables.insert("SPY_OPT", (whisper) ? "w" : "");
    RPCCommand::originate(channel, context, exten, callerId, variables);
    mMainWidget->getSelectedExtensZone()->removeExten(realtimeExt);
}

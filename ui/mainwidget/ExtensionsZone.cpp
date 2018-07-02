#include "ExtensionsZone.h"
#include "SelectedExtensZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/Config.h"

//static const long CALL_NOT_CONNECTED = -1;
static QStringList sCallbacks;
static ExtensionsZone* sExtensionsZone = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sExtensionsZone != NULL)
        sExtensionsZone->eventMessageReceived(eventMessage);
}

void ExtensionsZone::eventMessageReceived(QMap<QString, QString> &msg)
{
    if (!msg.contains("Event"))
        return;
    QString number = "";
    if (msg.value("Event") == "PeerStatus" && msg.contains("Peer") && msg.contains("PeerStatus")) {
        QString e = msg.value("Peer"); //sip/8012
        number = e.right(e.length() - (e.lastIndexOf("/") + 1));
    }
    else if (msg.value("Event") == "NewState" || msg.value("Event") == "Hangup") {
        QString channel = msg.value("Channel"); //SIP/8012-1f7f5270
        number = channel.left(channel.lastIndexOf("-"));
        number = number.right(number.length() - (number.lastIndexOf("/") + 1));
        LOG(Logger::Debug, "number: %s\n", number.toStdString().c_str());
    }
    else if (msg.value("Event") == "RollCallResult") {
        PBX::RollCallState rollCallState = PBX::eUnChecked;
        QString e = msg.value("Peer"); //sip/8012
        number = e.right(e.length() - (e.lastIndexOf("/") + 1));
        QString rResult = msg.value("Result");
        if (rResult == "true")
            rollCallState = PBX::eCheckOK;
        else
            rollCallState = PBX::eCheckFailed;
        LOG(Logger::Debug, "Got rollcalresult for number %s\n", number.toStdString().c_str());
        emit extenRollCallResultSignal(number, rollCallState);
        return;
    }
    if (number == "")
        return;
    //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
    QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(number);
    if (!extens.contains(number))
        return;
    PBX::Extension pbxExten = extens.value(number);
    emit extenStateChangedSignal(number, pbxExten.getState());
}
void ExtensionsZone::extenRollCallResultSlot(QString number, int rollCallResult)
{
    UiExtension* uiExtension = NULL;
    foreach (UiExtension* e, mExtensions.values()) {
        if (e->getPbxExten().number == number) {
            uiExtension = e;
            break;
        }
    }
    LOG(Logger::Debug, "number: %s, rollCallResult: %d\n", number.toStdString().c_str(), rollCallResult);
    if (uiExtension == NULL)
        return;
    uiExtension->setRollCallState(rollCallResult);
}

void ExtensionsZone::extenStateChangedSlot(QString number, int state)
{
    UiExtension* uiExten = NULL;
    foreach (UiExtension* e, mExtensions.values()) {
        if (e->getPbxExten().number == number) {
            uiExten = e;
            break;
        }
    }
    LOG(Logger::Debug, "number: %s, state: %d\n", number.toStdString().c_str(), state);
    if (uiExten == NULL) {
        if (mRinggroupNumber == Config::Instance()->CurrentCallGroupNumber) {
            if (state == PBX::eIncall || state == PBX::eRing || state == PBX::eRinging) { //往当前通话中添加按钮
                //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
                QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(number);
                if (!extens.contains(number))
                    return;
                QList<PBX::Extension> extensToLoad;
                extensToLoad.append(extens.value(number));
                loadExtensions(extensToLoad, true);
                return;
            }
        }
        if (mRinggroupNumber == Config::Instance()->OnlineExtensionGroupNumber) {
            if (state != PBX::eOffline) {
                //对新上线的分机，添加到在线分机组里
                //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
                QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(number);
                if (!extens.contains(number))
                    return;
                QList<PBX::Extension> extensToLoad;
                extensToLoad.append(extens.value(number));
                loadExtensions(extensToLoad, true);
                return;
            }
        }
    } else {
        if (mRinggroupNumber == Config::Instance()->CurrentCallGroupNumber) {
            if (state == PBX::eIdle || state == PBX::eOffline) {
                //从当前通话中删除按钮
                //QMap<QString, PBX::Extension> pbxExtens = RPCCommand::getExtensionDetailed(number);
                QMap<QString, PBX::Extension> pbxExtens = PBX::Instance()->getExtensionDetail(number);
                if (!pbxExtens.contains(number))
                    return;
                PBX::Extension pbxExten = pbxExtens.value(number);
                if (pbxExten.getState() == PBX::eIdle || pbxExten.getState() == PBX::eOffline)
                    removeExtension(number);
                return;
            }
        }
        if (mRinggroupNumber == Config::Instance()->OnlineExtensionGroupNumber) {
            if (state == PBX::eOffline) {
                //从在线分机组中删除
                //QMap<QString, PBX::Extension> pbxExtens = RPCCommand::getExtensionDetailed(number);
                QMap<QString, PBX::Extension> pbxExtens = PBX::Instance()->getExtensionDetail(number);
                if (!pbxExtens.contains(number))
                    return;
                PBX::Extension pbxExten = pbxExtens.value(number);
                if (pbxExten.getState() == PBX::eOffline)
                    removeExtension(number);
                return;
            }
        }
        uiExten->onStateChanged(state);
    }
}

ExtensionsZone::ExtensionsZone(MainWidget *mainWidget, QWidget *parent) :
    QWidget(parent), mMainWidget(mainWidget)
{
    sExtensionsZone = this;

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getGroupBox(vbox, this));

    mTextArea = new TextArea(this);
    mExtensPanel = new MultiTableWidgets(24 , 4, this, mTextArea, MultiTableWidgets::CtrlBtnBelowWidget, true);
    vbox->addWidget(mExtensPanel);
    loadExtensFromRinggroup(Config::Instance()->AllExtensionGroupNumber);
    mTextArea->setExtensCount(mExtensions.size());
    mTextArea->setOnlineExtensCount(0);
    mTextArea->setConferenceRoomCount(0);
    //QString callbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
    /* 获取实时消息 */
    QString callbackUuid = PBX::Instance()->addCallback(EventCallback);
    sCallbacks.append(callbackUuid);
    connect(this, SIGNAL(extenStateChangedSignal(QString,int)), this, SLOT(extenStateChangedSlot(QString,int)));
    connect(this, SIGNAL(extenRollCallResultSignal(QString,int)), this, SLOT(extenRollCallResultSlot(QString,int)));
}

ExtensionsZone::~ExtensionsZone()
{
    foreach (QString uuid, sCallbacks) {
        //MessageClient::Instance()->getEventMonitor()->removeCallback(uuid);
        PBX::Instance()->removeCallback(uuid);
    }
    foreach (UiExtension* e, mExtensions.values())
        delete e;
    mExtensions.clear();
    mExtensPanel->clear();
}

void ExtensionsZone::loadExtensFromRinggroup(QString grpNumber)
{
    mRinggroupNumber = grpNumber;
    QList<PBX::Extension> extensToLoad;
    if (grpNumber != Config::Instance()->AllExtensionGroupNumber &&
            grpNumber != Config::Instance()->CurrentCallGroupNumber &&
            grpNumber != Config::Instance()->OnlineExtensionGroupNumber) {
        QMap<QString, PBX::RingGroup> ringgroups;
        ringgroups = RPCCommand::getRinggroups(grpNumber);
        if (!ringgroups.contains(grpNumber))
            return;
        PBX::RingGroup ringgroup = ringgroups.value(grpNumber);
        foreach (QString extNumber, ringgroup.extenNumbers) {
            //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(extNumber);
            QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(extNumber);
            if (!extens.contains(extNumber)) {
                LOG(Logger::Debug, "%s not found\n", extNumber.toStdString().c_str());
                continue;
            }
            extensToLoad.append(extens.value(extNumber));
        }
    } else {
        //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed();
        QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail();
        foreach(PBX::Extension e, extens.values()) {
            PBX::PeerState state = e.getState();
            if (grpNumber == Config::Instance()->CurrentCallGroupNumber) {
                if (state == PBX::eIdle || state == PBX::eOffline)
                    continue;
            } else if (grpNumber == Config::Instance()->OnlineExtensionGroupNumber) {
                if (state == PBX::eOffline)
                    continue;
            }
            extensToLoad.append(e);
        }
    }
    loadExtensions(extensToLoad);
}

void ExtensionsZone::loadExtensions(QList<PBX::Extension>& extens, bool appendToExists)
{
    if (!appendToExists) {
        mExtensPanel->clear();
        foreach (UiExtension* uiExten, mExtensions.values())
            delete uiExten;
        mExtensions.clear();
    }
    foreach (PBX::Extension pbxExten, extens) {
        UiExtension* uiExten = new UiExtension(mMainWidget, pbxExten);
        mExtensions.insert(pbxExten.number, uiExten);
        mExtensPanel->appendWidget(uiExten);
        //如果在已选分机有此分机，则toggle
        if (mMainWidget->getSelectedExtensZone()->hasExtension(pbxExten.number))
            uiExten->toggle(true);
    }
    if (!appendToExists)
        mExtensPanel->refresh();
    else
        mExtensPanel->markTimedRefresh();
}

void ExtensionsZone::removeExtension(QString number)
{
    if (!mExtensions.contains(number))
        return;
    UiExtension* uiExten = mExtensions.value(number);
    mExtensions.remove(number);
    mExtensPanel->removeWidget(uiExten, false, true);
    mExtensPanel->markTimedRefresh();
}

void ExtensionsZone::toggleExten(bool toggle, QString extNumber)
{
    if (extNumber == "") {
        foreach (UiExtension* exten, mExtensions) {
            exten->toggle(toggle);
            if (exten->isToggled())
                mMainWidget->getSelectedExtensZone()->addExten(exten->getPbxExten());
            else
                mMainWidget->getSelectedExtensZone()->removeExten(exten->getPbxExten());
        }
        return;
    }
    foreach (UiExtension* exten, mExtensions) {
        if (exten->getPbxExten().number == extNumber) {
            exten->toggle(toggle);
            return;
        }
    }
}

UiExtension::UiExtension(MainWidget* mw, PBX::Extension e, QWidget *parent) :
    UiExtensionBase(e, parent), mMainWidget(mw)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    {
        QHBoxLayout* hbox = new QHBoxLayout;
        hbox->setMargin(0);
        hbox->setSpacing(0);
        QLabel* lbNumber = new QLabel(e.number, this);
        lbNumber->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        mIcon->setAlignment(Qt::AlignRight);
        hbox->addWidget(mIcon);
        hbox->addWidget(lbNumber);
        QLabel* lbName = new QLabel(e.name, this);
        lbName->setAlignment(Qt::AlignCenter);

        QVBoxLayout* infoBox = new QVBoxLayout;
        infoBox->addLayout(hbox);
        infoBox->addWidget(lbName);
        rootBox->addLayout(infoBox);
    }
    {
        mRollCallIcon = new QLabel(this);
        mRollCallIcon->setAlignment(Qt::AlignRight);
        mRollCallIcon->setPixmap(QPixmap(Config::Instance()->Res_icon_unchecked));
        mLabelPeer = new QLabel(this);
        mLabelPeer->setAlignment(Qt::AlignLeft);
        mLableArrow = new QLabel(this);
        mLableArrow->setPixmap(QPixmap(Config::Instance()->Res_arrow_right));
        mLableArrow->setAlignment(Qt::AlignRight);
        mLableArrow->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        mLableArrow->setVisible(false);
        mLabelDuration = new QLabel(this);
        mLabelDuration->setAlignment(Qt::AlignRight);
        //mCallDuration = CALL_NOT_CONNECTED;
        mCallDuration = getLongestDuration();
        QVBoxLayout* callBox = new QVBoxLayout;
        callBox->setMargin(0);
        callBox->setSpacing(0);
        callBox->addWidget(mRollCallIcon);
        QHBoxLayout* hbox = new QHBoxLayout;
        hbox->setAlignment(Qt::AlignRight);
        hbox->setMargin(0);
        hbox->setSpacing(1);
        hbox->addWidget(mLableArrow);
        hbox->addWidget(mLabelPeer);
        callBox->addLayout(hbox);
        callBox->addWidget(mLabelDuration);
        rootBox->addLayout(callBox);
    }
    setLayout(rootBox);
    connect(this, SIGNAL(stateChangedSignal(int)), this, SLOT(stateChangedSlot(int)));
    connect(this, SIGNAL(widgetClickedSignal()), this, SLOT(clickedSlot()));
    startTimer(1000);
}

UiExtension::~UiExtension()
{
    LOG(Logger::Debug, "UiExtension(%s) deleting\n", mPbxExten.number.toStdString().c_str());
}

void UiExtension::onStateChanged(int state)
{
    emit stateChangedSignal(state);
}

void UiExtension::stateChangedSlot(int state) {
    QMap<QString, PBX::Extension> exts = PBX::Instance()->getExtensionDetail(mPbxExten.number);
    if (!exts.contains(mPbxExten.number))
        return;
    mPbxExten = exts.value(mPbxExten.number);
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(state)));
    switch (state) {
    case PBX::eIncall:
        if (mCallDuration == CALL_NOT_CONNECTED)
            mCallDuration = 1;
        break;
    case PBX::eIdle:
    default:
        mCallDuration = CALL_NOT_CONNECTED;
        mLabelDuration->setText("");
        mLableArrow->setVisible(false);
        break;
    }
}

void UiExtension::clickedSlot()
{
    if (isToggled())
        mMainWidget->getSelectedExtensZone()->addExten(getPbxExten());
    else
        mMainWidget->getSelectedExtensZone()->removeExten(getPbxExten());
}

void UiExtension::timerEvent(QTimerEvent *)
{
    if (mCallDuration == CALL_NOT_CONNECTED) {
        if (mLabelPeer->text() != "") {
            mLableArrow->setVisible(false);
            mLabelPeer->setText("");
        }
        return;
    }
    mCallDuration++;
    QString duration = QString("%1").arg(mCallDuration);
    mLabelDuration->setText(duration);
    if (mLabelPeer->text() == "") {
        PBX::PeerInfo peer = getPeerWithLongestDuration();
        //QString peerNumber = getPeerWithLongestDuration().number;
        QString peerNumber = peer.number;
        if (peerNumber == "") {
            QString peerChannel = peer.channel;
            peerNumber = peerChannel.left(peerChannel.lastIndexOf("-"));
        }
        mLabelPeer->setText(peerNumber);
        mLableArrow->setVisible(true);
    }
}

void UiExtension::setRollCallState(int result)
{
    Config* cfg = Config::Instance();
    QString icon = cfg->Res_icon_unchecked;
    switch (result) {
    case PBX::eCheckOK :
        icon = cfg->Res_icon_checked;
        break;
    case PBX::eCheckFailed :
        icon = cfg->Res_icon_check_failed;
        break;
    }
    mRollCallIcon->setPixmap(QPixmap(icon));
}

TextArea::TextArea(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    setLayout(rootBox);
    {
        QHBoxLayout* extCntLayout = new QHBoxLayout;
        extCntLayout->setMargin(0);
        extCntLayout->addWidget(new QLabel(tr("分机总数"), this), 0, Qt::AlignLeft);
        mExtensCount = new QLabel(this);
        extCntLayout->addWidget(mExtensCount, 0, Qt::AlignRight);
        DingoButton* btn = new DingoButton(this);
        btn->setLayout(extCntLayout);
        btn->setToggleable(false);
        rootBox->addWidget(btn);
    }
//    {
//        QHBoxLayout* onlineCntLayout = new QHBoxLayout;
//        onlineCntLayout->setMargin(0);
//        onlineCntLayout->addWidget(new QLabel(tr("在线分机数"), this), 0, Qt::AlignLeft);
//        mOnlineExtensCount = new QLabel(this);
//        onlineCntLayout->addWidget(mOnlineExtensCount, 0, Qt::AlignRight);
//        DingoButton* btn = new DingoButton(this);
//        btn->setLayout(onlineCntLayout);
//        btn->setToggleable(false);
//        rootBox->addWidget(btn);
//    }
//    {
//        QHBoxLayout* roomCntLayout = new QHBoxLayout;
//        roomCntLayout->setMargin(0);
//        roomCntLayout->addWidget(new QLabel(tr("在线会议室数"), this), 0, Qt::AlignLeft);
//        mConferenceRoomCount = new QLabel(this);
//        roomCntLayout->addWidget(mConferenceRoomCount, 0, Qt::AlignRight);
//        DingoButton* btn = new DingoButton(this);
//        btn->setLayout(roomCntLayout);
//        btn->setToggleable(false);
//        rootBox->addWidget(btn);
//    }
}

void TextArea::setExtensCount(int cnt)
{
    QString txtCnt = QString("%1").arg(cnt);
    mExtensCount->setText(txtCnt);
}

void TextArea::setOnlineExtensCount(int cnt)
{
//    QString txtCnt = QString("%1").arg(cnt);
//    mOnlineExtensCount->setText(txtCnt);
}

void TextArea::setConferenceRoomCount(int cnt)
{
//    QString txtCnt = QString("%1").arg(cnt);
//    mConferenceRoomCount->setText(txtCnt);
}

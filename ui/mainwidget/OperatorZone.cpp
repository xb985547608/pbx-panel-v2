#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "OperatorZone.h"
#include "message_queue/RPCCommand.h"
#include "message_queue/MessageClient.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"
#include "misc/Config.h"
#include "FastFuncZone.h"
#include "MainWidget.h"

static OperatorZone* sOperatorZone = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sOperatorZone != NULL)
        sOperatorZone->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void OperatorZone::eventMessageReceived(QMap<QString, QString> &msg)
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
    }
    if (!mOpExtens.contains(number))
        return;
    /*
    QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
    if (!extens.contains(number))
        return;
    PBX::Extension pbxExten = extens.value(number);
    */
    PBX::Instance()->getExtensionDetail("", true);
    OperatorExten* e = mOpExtens.value(number);
    e->onStateChanged();
    emit operatorExtenStateChagne(e->getPbxExten().number);
}

OperatorZone::OperatorZone(MainWidget *mw, QWidget *parent) :
    QWidget(parent), mMainWidget(mw)
{
    sOperatorZone = this;
    TableWidget* tWidget = new TableWidget(2, this);
    QMap<QString, PBX::Extension> extensions = RPCCommand::getOperators();
    foreach (PBX::Extension e, extensions.values()) {
        OperatorExten* opExten = new OperatorExten(e, mMainWidget, this);
        mOpExtens.insert(e.number, opExten);
        tWidget->appendItem(opExten);
    }

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(tWidget);
    QGroupBox* groupBox = MiscComponents::getGroupBox(rootBox, this);

    QVBoxLayout* box = new QVBoxLayout;
    box->setMargin(0);
    box->setSpacing(0);
    box->addWidget(groupBox);

    setLayout(box);
    //sCallbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
    sCallbackUuid = PBX::Instance()->addCallback(EventCallback);
}

OperatorZone::~OperatorZone()
{
    if (sCallbackUuid != "")
        MessageClient::Instance()->getEventMonitor()->removeCallback(sCallbackUuid);
    sOperatorZone = NULL;
    foreach (OperatorExten* e, mOpExtens)
        delete e;
}

void OperatorZone::opExtenClicked(QString extenNumber)
{
    if (!mOpExtens.contains(extenNumber))
        return;
    OperatorExten* opExten = mOpExtens.value(extenNumber);
    if (!opExten->isToggled() || opExten->getPbxExten().getState() != PBX::eIncall) {
        mMainWidget->getFastFuncZone()->enable3WayBtn(false);
        opExten->toggle(false);
        return;
    }
    foreach (OperatorExten* exten, mOpExtens.values()) {
        if (exten != opExten)
            exten->toggle(false);
    }
    mMainWidget->getFastFuncZone()->enable3WayBtn(true);
}

PBX::Extension OperatorZone::getSelectedOpExten()
{
    foreach (OperatorExten* opExten, mOpExtens.values()) {
        if (opExten->isToggled())
            return opExten->getPbxExten();
    }
    PBX::Extension exten;
    return exten;
}

OperatorExten::OperatorExten(PBX::Extension e, MainWidget* mw, QWidget *parent) :
    UiExtensionBase(e, parent), mMainWidget(mw)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(mIcon);
    rootBox->addWidget(new QLabel(tr("调度员"), this));
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(new QLabel(mPbxExten.name, this));
    vbox->addWidget(new QLabel(mPbxExten.number, this));
    rootBox->addLayout(vbox);
    setLayout(rootBox);
    connect(this, SIGNAL(stateChangedSignal()), this, SLOT(stateChangedSlot()));
    connect(this, SIGNAL(widgetClickedSignal()), this, SLOT(clickedSlot()));
    //setToggleable(false);
}

void OperatorExten::clickedSlot()
{
    mMainWidget->getOperatorZone()->opExtenClicked(this->getPbxExten().number);
}

void OperatorExten::onStateChanged()
{
    emit stateChangedSignal();
}

void OperatorExten::stateChangedSlot() {
    QString number = getPbxExten().number;
    //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
    QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(number);
    if (!extens.contains(number))
        return;
    mPbxExten = extens.value(number);
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(mPbxExten.getState())));
    mMainWidget->getOperatorZone()->opExtenClicked(number);
}

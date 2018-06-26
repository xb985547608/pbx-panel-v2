#include "OperatorZone.h"
#include "message_queue/RPCCommand.h"
#include "message_queue/MessageClient.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"
#include "misc/Config.h"

static OperatorZone* sOperatorZone = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sOperatorZone != NULL)
        sOperatorZone->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void OperatorZone::eventMessageReceived(QMap<QString, QString> &msg)
{
    if (msg.contains("Event") && msg.value("Event") == "PeerStatus" && msg.contains("Peer") && msg.contains("PeerStatus")) {
        QString status = msg.value("PeerStatus");
        int peerStatus;
        if (status == "OK" || status == "Reachable" || status == "Registered")
            peerStatus = PBX::eIdle;
        else
            peerStatus = PBX::eOffline;
        QString e = msg.value("Peer");
        QString number = e.right(e.length() - (e.lastIndexOf("/") + 1));
        if (mOpExtens.contains(number)) {
            OperatorExten* e = mOpExtens.value(number);
            e->onStateChanged(peerStatus);
        }
    }
}

OperatorZone::OperatorZone(QWidget *parent) :
    QWidget(parent)
{
    sOperatorZone = this;
    TableWidget* tWidget = new TableWidget(2, this);
    QMap<QString, PBX::Extension> extensions = RPCCommand::getOperators();
    foreach (PBX::Extension e, extensions.values()) {
        OperatorExten* opExten = new OperatorExten(e, this);
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
    sCallbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
}

OperatorZone::~OperatorZone()
{
    if (sCallbackUuid != "")
        MessageClient::Instance()->getEventMonitor()->removeCallback(sCallbackUuid);
    sOperatorZone = NULL;
    foreach (OperatorExten* e, mOpExtens)
        delete e;
}

OperatorExten::OperatorExten(PBX::Extension e, QWidget *parent) :
    UiExtensionBase(e, parent)
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
    connect(this, SIGNAL(stateChangedSignal(int)), this, SLOT(stateChangedSlot(int)));
    setToggleable(false);
}

void OperatorExten::onStateChanged(int state)
{
    emit stateChangedSignal(state);
}

void OperatorExten::stateChangedSlot(int state) {
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(state)));
}

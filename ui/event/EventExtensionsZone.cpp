#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "EventExtensionsZone.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"

static EventExtensionsZone* sNormalEventExtensionsZone = NULL;
static EventExtensionsZone* sEmergencyEventExtensionsZone = NULL;

static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sNormalEventExtensionsZone != NULL)
        sNormalEventExtensionsZone->eventMessageReceived(eventMessage);
    if (sEmergencyEventExtensionsZone != NULL)
        sEmergencyEventExtensionsZone->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void EventExtensionsZone::eventMessageReceived(QMap<QString, QString>& msg)
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
    if (number == "")
        return;
    //QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number);
    QMap<QString, PBX::Extension> extens = PBX::Instance()->getExtensionDetail(number);
    if (!extens.contains(number))
        return;
    PBX::Extension pbxExten = extens.value(number);
    emit eventMessageReceivedSignal(number, pbxExten.getState());
}

void EventExtensionsZone::eventMessageReceivedSlot(QString extenNumber, int state)
{
    foreach (EventExten* exten, mEventExtens) {
        QString number = exten->getPbxExten().number;
        if (number == extenNumber)
            exten->onStateChangedSlot(state);
    }
}

EventExtensionsZone::EventExtensionsZone(EventWidget* eventWidget, QWidget *parent) :
    QWidget(parent),
    mEventWidget(eventWidget)
{
    mEventExtenPanel = new MultiTableWidgets(18, 6);
    mEventExtenPanel->setHeader(MiscComponents::getSectionLabel(tr("分机列表"), mEventExtenPanel));
    QVBoxLayout* rootBox = new QVBoxLayout(this);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(mEventExtenPanel);
    setLayout(rootBox);
    if (eventWidget->getWidgetType() == EventModel::eNormal)
        sNormalEventExtensionsZone = this;
    else
        sEmergencyEventExtensionsZone = this;
    if (sCallbackUuid == "")
        sCallbackUuid = PBX::Instance()->addCallback(EventCallback);
    connect(this, SIGNAL(eventMessageReceivedSignal(QString,int)), this, SLOT(eventMessageReceivedSlot(QString,int)));
}

EventExtensionsZone::~EventExtensionsZone()
{
    if (sCallbackUuid != "") {
        PBX::Instance()->removeCallback(sCallbackUuid);
        sCallbackUuid = "";
    }
}

void EventExtensionsZone::loadEventExtens(QList<PBX::Extension> extens)
{
    mEventExtens.clear();
    mEventExtenPanel->clear(false, true);
    foreach (PBX::Extension e, extens) {
        EventExten* exten = new EventExten(e);
        mEventExtens.append(exten);
        mEventExtenPanel->appendWidget(exten);
    }
    mEventExtenPanel->refresh();
}

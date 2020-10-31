#include "pbx.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/tools.h"

static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    PBX::Instance()->eventMessageReceived(e, eventMessage);
}

void PBX::eventMessageReceived(MessageClient::EventType e, QMap<QString, QString> &msg)
{
    if (!msg.contains("Event"))
        return;
    QString number = "";
    RollCallState rollCallState = PBX::eUnChecked;
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
        QString e = msg.value("Peer"); //sip/8012
        number = e.right(e.length() - (e.lastIndexOf("/") + 1));
        QString rResult = msg.value("Result");
        if (rResult == "true")
            rollCallState = PBX::eCheckOK;
        else
            rollCallState = PBX::eCheckFailed;
        LOG(Logger::Debug, "Got rollcalresult for number %s\n", number.toStdString().c_str());
    }
//    else if (msg.value("Event") == "DisconnectEvent") {
//        mExtensions = RPCCommand::getExtensionDetailed();
//    }
    if (number == "")
        return;
    QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(number, 2);
    if (!extens.contains(number))
        return;
    if (mExtensions.contains(number)) {
        PBX::Extension extToRemove = mExtensions.value(number);
        if (rollCallState == PBX::eUnChecked) //保留原有点名状态
            rollCallState = extToRemove.rollCallState;
        mExtensions.remove(number);
    }
    PBX::Extension extToAdd = extens.value(number);
    extToAdd.rollCallState = rollCallState;
    mExtensions.insert(number, extToAdd);
    fireCallback(e, msg);
}

QString PBX::addCallback(MessageClient::EventMonitorCallback c, MessageClient::EventType e)
{
    EventMonitor::EventCallbackPair pair;
    pair.first = e;
    pair.second = c;
    QString uuid = QString(Tools::getUuid().c_str());
    mCallbacks.insert(uuid, pair);
    return uuid;
}

void PBX::removeCallback(QString uuid)
{
    if (mCallbacks.contains(uuid))
        mCallbacks.remove(uuid);
}

void PBX::fireCallback(MessageClient::EventType e, QMap<QString, QString> &message)
{
    foreach (EventMonitor::EventCallbackPair pair, mCallbacks.values())
        if (pair.first == e || pair.first == MessageClient::eALL)
            (pair.second)(e, message);
}

bool PBX::initialise()
{
    /* 获取分机详情 */
    mExtensions = RPCCommand::getExtensionDetailed();
    /* 增加消息回调函数 */
    MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
    return true;
}

PBX::Extension PBX::getExtensionDetail(PBX::Extension &e) {
    foreach (PBX::Extension ext, mExtensions.values()) {
        if (e.number == e.number) {
            e = ext;
            return e;
        }
    }
    e.number = "";
    e.name = "";
    return e;
}

QMap<QString, PBX::Extension> PBX::getExtensionDetail(QString number, bool update) {
    if (update)
        mExtensions = RPCCommand::getExtensionDetailed();

    if (number == "")
        return mExtensions;
    QMap<QString, PBX::Extension> extens;
    if (mExtensions.contains(number))
        extens.insert(number, mExtensions.value(number));
    return extens;
}

QString PBX::RingGroup::asString()
{
    QString ret = description + "(" + number + "): extensions: ";
    foreach (QString ext, extenNumbers)
        ret += ext + "-";
    return ret.left(ret.lastIndexOf("-"));
}

PBX::Extension::Extension()
{
    state = PBX::eOffline;
    tech = "SIP";
    rollCallState = PBX::eUnChecked;
}

PBX::PeerState PBX::Extension::getState()
{
    if (peers.size() == 0)
        return state;
    PBX::PeerState pstate = PBX::eOffline;
    foreach (PeerInfo info, peers.values()) {
        if (info.state > pstate)
            pstate = info.state;
    }
    return pstate;
}

/*
PBX::PeerInfo::_struct_peer_info()
{
    channel = "";
    peerChannel = "";
    name = "";
    number = "";
    state = PBX::eOffline; //eRing/eRinging/eIncall
    upTime = QDateTime::currentDateTime();
}
*/
PBX::PeerInfo::PeerInfo()
{
    channel = "";
    peerChannel = "";
    name = "";
    number = "";
    state = PBX::eOffline; //eRing/eRinging/eIncall
    upTime = QDateTime::currentDateTime();
}

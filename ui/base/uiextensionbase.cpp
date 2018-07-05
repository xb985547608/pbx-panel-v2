#include "uiextensionbase.h"
#include "misc/Config.h"
//#include "pbx/pbx.h"

int UiExtensionBase::CALL_NOT_CONNECTED = 0;

UiExtensionBase::UiExtensionBase(PBX::Extension e, QWidget *parent) :
    DingoButton(parent), mPbxExten(e)
{
    Config* cfg = Config::Instance();
    mStateIconsRes.insert(PBX::eOffline, cfg->Res_icon_offline);
    mStateIconsRes.insert(PBX::eIdle, cfg->Res_icon_online);
    mStateIconsRes.insert(PBX::eRing, cfg->Res_icon_ring);
    mStateIconsRes.insert(PBX::eRinging, cfg->Res_icon_ringing);
    mStateIconsRes.insert(PBX::eIncall, cfg->Res_icon_incall);
    mIcon = new QLabel(this);
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(mPbxExten.getState())));
}

void UiExtensionBase::setStateIconRes(PBX::PeerState state, QString iconRes)
{
    mStateIconsRes[state] = iconRes;
}

PBX::PeerInfo UiExtensionBase::getPeerWithLongestDuration()
{
    /*
    PBX::PeerInfo myPeer;
    foreach (PBX::PeerInfo p, mPbxExten.peers.values()) {
        if (p.state == PBX::eIncall) {
            if (p.upTime < myPeer.upTime) {
                myPeer.channel = p.channel;
                myPeer.name = p.name;
                myPeer.number = p.number;
                myPeer.state = p.state;
                myPeer.upTime = p.upTime;
            }
        }
    }
    return myPeer;
    */
    foreach (PBX::PeerInfo p, mPbxExten.peers.values()) {
        if (p.state == PBX::eIncall)
            return p;
    }
    PBX::PeerInfo peer;
    return peer;
}

int UiExtensionBase::getLongestDuration()
{
    int duration = CALL_NOT_CONNECTED;
    QDateTime now = QDateTime::currentDateTime();
    foreach (PBX::PeerInfo peer, mPbxExten.peers.values()) {
        if (peer.state == PBX::eIncall) {
            int d = now.toTime_t() - peer.upTime.toTime_t();
            if (d > duration)
                duration = d;
        }
    }
    return duration;
}

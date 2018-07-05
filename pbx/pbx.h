#ifndef PBX_H
#define PBX_H

#include <QtGui>
#include <misc/singleton.h>
#include "message_queue/MessageClient.h"

class PBX : public Singleton<PBX>
{
public:
    typedef enum _peerState {eOffline, eIdle, eRing, eRinging, eIncall} PeerState;
    typedef enum _rollCallState { eUnChecked, eCheckOK, eCheckFailed } RollCallState; //点名状态类型
    typedef struct _struct_ring_group {
        QString number;
        QString description;
        QStringList extenNumbers;
        QString asString();
    } RingGroup;

    /*
     *
     */
    /*
    typedef struct _struct_peer_info {
        _struct_peer_info();
        QString channel;
        QString peerChannel;
        QString name;
        QString number;
        PBX::PeerState state; //eRing/eRinging/eIncall
        QDateTime upTime; //如果channel处于up状态，其接通的时间
    } PeerInfo;
    */
    class PeerInfo {
    public:
        PeerInfo();
        QString channel;
        QString peerChannel;
        QString name;
        QString number;
        PBX::PeerState state; //eRing/eRinging/eIncall
        QDateTime upTime; //如果channel处于up状态，其接通的时间
    };


    class Extension {
    public:
        Extension();
        QString name;
        QString displayName;
        QString number;
        QString tech;
        PBX::PeerState state; //eOffline/eIdle
        QString context;
        /*
         * qmap<本地channel, peerinfo>
         */
        QMap<QString, PeerInfo> peers;
        PBX::PeerState getState();
        RollCallState rollCallState;
    };

    typedef struct _struct_current_call {
        QString channel;
        int duration;
        PeerInfo peer;
    } CurrentCall;

    typedef struct _struct_cdr {
        QString id;
        QString src;
        QString dst;
        QDateTime calldate;
        QString duration;
        QString userField;
    } CDR;

    typedef struct _struct_meetme_member {
        QString userNum;
        QString callerid;
        QString channel;
        bool isAdmin;
        bool isMuted;
    } MeetmeMember;

    PBX() : Singleton<PBX>("PBX") {}
    virtual ~PBX() {}
    bool initialise();
    PBX::Extension getExtensionDetail(PBX::Extension& e);
    QMap<QString, PBX::Extension> getExtensionDetail(QString number = "");

    void eventMessageReceived(MessageClient::EventType, QMap<QString, QString> &msg);
    /*
     * 增加回调，并返回该回调的uuid
     */
    QString addCallback(MessageClient::EventMonitorCallback c, MessageClient::EventType e = MessageClient::eALL);
    /*
     * 根据uuid删除回调
     */
    void removeCallback(QString uuid);
private :
    //<group-number, ringgroup*>
    QMap<QString, PBX::RingGroup> mRinggroups;
    //<extension-number, extension>
    QMap<QString, PBX::Extension> mExtensions;
    /*
     * <uuid, EventCallbackPair>
     */
    QMap<QString, EventMonitor::EventCallbackPair> mCallbacks;
    void fireCallback(MessageClient::EventType, QMap<QString, QString> &message);
};

#endif // PBX_H

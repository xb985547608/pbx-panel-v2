#ifndef MESSAGECLIENT_H
#define MESSAGECLIENT_H

#include <QtGui>
#include <QThread>
#include "misc/singleton.h"
extern "C" {
#include "amqp_framing.h"
}

class EventMonitor;
class Housekeeper;

class MessageClient : public Singleton<MessageClient>
{
public:
    /*
     * eSystem - 系统消息，如连接故障等
     * eMqEvent - 来自队列的消息
     */
    typedef enum _EventType { eSystem, eMqEvent, eALL } EventType;
    /*
     * 自队列消息回调
     * @param: EventType - 消息类型
     * @param: QMap<QString, QString> - 消息内容，如: <key = Event, value = PeerStatus>
     */
    typedef void (*EventMonitorCallback)(MessageClient::EventType, QMap<QString, QString>& eventMessage);
    class RPCClient
    {
    public:
        RPCClient() {}
        bool initialise(
                const QString &host,
                const QString &rpcRoutingkey,
                const QString &rpcExchange,
                int port,
                const QString &user,
                const QString &passwd
                );
        QString rpcCall(const QString& params);
        ~RPCClient();
    private:
        amqp_connection_state_t mConnection;
        amqp_bytes_t mQueueName;
        //amqp_pool_t mPool;
        QString mRoutingkey;
        QString mExchange;
        int mSockfd;
    };
    MessageClient() : Singleton<MessageClient>("MessageClient") {}
    virtual ~MessageClient();
    /*
       @host: 服务器地址
       @port: 端口号
       @rpcRoutingkey: 可填写为rpc_queue，用于区分不同的rpc调用
       @rpcExchange: 默认可填写为""
       @eventBindingkey: 事件接收所用的binding key
       @eventExchange: 事件接收所用的exchange
     */
    bool initialise(
            const QString& host,
            const QString& eventBindingkey,
            const QString& eventExchange,
            const QString& rpcRoutingkey = "rpc_queue",
            const QString& rpcExchange = "",
            int port = 5672,
            const QString& user = "guest",
            const QString& passwd = "guest"
            );
    /*
     * rpc调用
     * @param: params - rpc参数，一个rpc的xml
     */
    QString rpcCall(const QString& params) { return mRPCClient->rpcCall(params); }
    RPCClient* getRPCClient() { return mRPCClient; }
    EventMonitor* getEventMonitor() { return mEventMonitor; }
    Housekeeper* getHousekeeper() { return mHousekeeper; }
private:
    RPCClient* mRPCClient;
    EventMonitor* mEventMonitor;
    Housekeeper* mHousekeeper;
};

//用于检测与mq通讯是否正常，目前的实现方法是判断能否成功发起连接到mq
//最好通过mainwindow的timer定时调用
class Housekeeper
{
public:
    Housekeeper(const QString &host, int port) :
        mHost(host), mPort(port) {}
    bool check();
private:
    QString mHost;
    int mPort;
};
/************************ 监听事件 ************************/
class EventMonitor : public QThread
{
    Q_OBJECT
public:
    EventMonitor(
            const QString &host,
            const QString &eventBindingkey,
            const QString &eventExchange,
            int port,
            const QString &user,
            const QString &passwd,
            QObject *parent = 0);
    ~EventMonitor();
    bool initialise();
    /*
     * 增加回调，并返回该回调的uuid
     */
    QString addCallback(MessageClient::EventMonitorCallback c, MessageClient::EventType e = MessageClient::eALL);
    /*
     * 根据uuid删除回调
     */
    void removeCallback(QString uuid);
    void stop();

signals:
    void removeCallbackSignal(QString uuid);
private slots:
    void removeCallbackSlot(QString uuid);
protected:
    virtual void run();

public:
    typedef QPair<MessageClient::EventType, MessageClient::EventMonitorCallback> EventCallbackPair;
private:
    /*
     * <uuid, EventCallbackPair>
     */
    QMap<QString, EventCallbackPair> mCallbacks;
    amqp_connection_state_t mConnection;
    amqp_pool_t mPool;
    QString mHost;
    int mPort;
    QString mBindingkey;
    QString mExchange;
    QString mUser;
    QString mPasswd;
    bool mStop;

    void fireCallback(MessageClient::EventType, QMap<QString, QString> &message);
    void fireSystemEventCallback(QString message);
    void fireMqMsgCallback(QString message);
};
#endif // MESSAGECLIENT_H

#ifdef WIN32
#include "winsock2.h"
#else
#include <stdint.h>
#endif
extern "C" {
#include <assert.h>
}

#include "MessageClient.h"
#include "misc/tools.h"
#include "misc/logger.h"
#include "misc/Config.h"

QMutex RpcLock;

static bool rabbit_mq_reply_is_ok(amqp_rpc_reply_t x, const char* context);
static int read_timeout(int fd, int sec);

MessageClient::~MessageClient()
{
    delete mRPCClient;
    mEventMonitor->stop();
    delete mEventMonitor;
    delete mHousekeeper;
}

bool MessageClient::initialise(
        const QString &host,
        const QString &eventBindingkey,
        const QString &eventExchange,
        const QString &rpcRoutingkey,
        const QString &rpcExchange,
        int port,
        const QString &user,
        const QString &passwd)
{
    mRPCClient = new RPCClient();
    mEventMonitor = new EventMonitor(host, eventBindingkey, eventExchange, port, user, passwd);
    mHousekeeper = new Housekeeper(host, port);
    if (!mRPCClient->initialise(host, rpcRoutingkey, rpcExchange, port, user, passwd)) {
        LOG(Logger::Error, "rpc client initialise failed\n");
        return false;
    }
    if (!mEventMonitor->initialise()) {
        LOG(Logger::Error, "event monitor client initialise failed\n");
        return false;
    }
    return true;
}

MessageClient::RPCClient::~RPCClient()
{
    amqp_rpc_reply_t rpc_reply = amqp_channel_close(mConnection, 1, AMQP_REPLY_SUCCESS);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "Closing channel"))
        return;
    rpc_reply = amqp_connection_close(mConnection, AMQP_REPLY_SUCCESS);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "Closing connection"))
        return;
    if (amqp_destroy_connection(mConnection) < 0)
        LOG(Logger::Debug, "Ending connection failed\n");
}

bool MessageClient::RPCClient::initialise(
        const QString &host,
        const QString &rpcRoutingkey,
        const QString &rpcExchange,
        int port,
        const QString &user,
        const QString &passwd)
{
    amqp_rpc_reply_t rpc_reply;

    mRoutingkey = rpcRoutingkey;
    mExchange = rpcExchange;
    mConnection = amqp_new_connection();
    mSockfd = amqp_open_socket(host.toStdString().c_str(), port);
    if (mSockfd < 0) {
        LOG(Logger::Warn, "rpc_call: Opening socket failed\n");
        return false;
    }
    amqp_set_sockfd(mConnection, mSockfd);
    rpc_reply = amqp_login(mConnection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, user.toStdString().c_str(), passwd.toStdString().c_str());
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rpc_call: logging in"))
        return false;
    amqp_channel_open(mConnection, 1);
    rpc_reply = amqp_get_rpc_reply(mConnection);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rpc_call: Openning channel"))
        return false;
    {
        /* 绑定以接收rpc响应 */
        amqp_queue_declare_ok_t *r = amqp_queue_declare(mConnection, 1, amqp_empty_bytes_local, 0, 0, 0, 1, amqp_empty_table_local);
        rpc_reply = amqp_get_rpc_reply(mConnection);
        if (!rabbit_mq_reply_is_ok(rpc_reply, "rpc_call: declaring queue"))
            return false;
        mQueueName = amqp_bytes_malloc_dup(r->queue);
#ifndef QT_NO_DEBUG
        char queueName[MAX_BUFFER_LEN] = { '\0' };
        memcpy(queueName, mQueueName.bytes, mQueueName.len);
        LOG(Logger::DDebug, "rpc quename: %s\n", queueName);
#endif
        if (mQueueName.bytes == NULL) {
            LOG(Logger::DDebug, "Out of memory while copying queue name");
            return false;
        }
        amqp_basic_consume(mConnection, 1, mQueueName, amqp_empty_bytes_local, 0, 1, 0, amqp_empty_table_local);
        rpc_reply = amqp_get_rpc_reply(mConnection);
        if (!rabbit_mq_reply_is_ok(rpc_reply, "Consuming"))
            return false;
    }
    return true;
}

QString MessageClient::RPCClient::rpcCall(const QString &params)
{
#ifdef EEPBX
    QString rpcID = QString("RPCID-") + Tools::getUuid().c_str() + "\r\r\n\n";
#endif
    RpcLock.lock();
    char* return_buffer = NULL;
    {	/* 发送rpc调用 */
#ifdef EEPBX
        QString rpcParameter = rpcID + params;
#else
        QString rpcParameter = params;
#endif
        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                AMQP_BASIC_DELIVERY_MODE_FLAG |
                AMQP_BASIC_CORRELATION_ID_FLAG |
                AMQP_BASIC_REPLY_TO_FLAG;
        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        std::string corr_id = Tools::getUuid();
        props.correlation_id = amqp_cstring_bytes(corr_id.c_str());
        props.reply_to = mQueueName;
        LOG(Logger::DDebug, "corr_id=%s, queuename=%s\n", corr_id.c_str(), (char*)mQueueName.bytes);
        int presult = amqp_basic_publish(mConnection,
                     1,
                     amqp_cstring_bytes(mExchange.toStdString().c_str()),
                     amqp_cstring_bytes(mRoutingkey.toStdString().c_str()),
                     0,
                     0,
                     &props,
                     amqp_cstring_bytes(rpcParameter.toStdString().c_str()));
        if (presult  < 0) {
            LOG(Logger::Warn, "sending rpc_call method failed: %d\n", presult);
            RpcLock.unlock();
            return "";
        }
    }
    QString ret; //返回值
#ifdef EEPBX
    do {
#endif
        ret = "";
        amqp_frame_t frame;
        int result;
        amqp_basic_deliver_t *d;
        amqp_basic_properties_t *p;
        size_t body_target;
        size_t body_received;
        int readable_bytes = 0;
        do {
            amqp_maybe_release_buffers(mConnection);
            readable_bytes = read_timeout(mSockfd, Config::Instance()->RpcTimeout);
            if (readable_bytes == -1 || readable_bytes == 0) {
                LOG(Logger::Error, "RPC read_timeout for command: %s\n", params.toStdString().c_str());
                break;
            }
            result = amqp_simple_wait_frame(mConnection, &frame);
            LOG(Logger::DDebug, "Result %d\n", result);
            if (result < 0) {
                LOG(Logger::Error, "RPC amqp_simple_wait_frame failed\n");
                break;
            }
            LOG(Logger::DDebug, "Frame type %d, channel %d\n", frame.frame_type, frame.channel);
            if (frame.frame_type != AMQP_FRAME_METHOD) {
                LOG(Logger::Error, "RPC frame.frame_type != AMQP_FRAME_METHOD\n");
                continue;
            }
            LOG(Logger::DDebug, "Method %s\n", amqp_method_name(frame.payload.method.id));
            if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD) {
                LOG(Logger::Error, "RPC frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD\n");
                continue;
            }
            d = (amqp_basic_deliver_t *) frame.payload.method.decoded;
#ifndef QT_NO_DEBUG
            LOG(Logger::DDebug, "Delivery %u, exchange %.*s routingkey %.*s\n",
                (unsigned) d->delivery_tag,
                (int) d->exchange.len, (char *) d->exchange.bytes,
                (int) d->routing_key.len, (char *) d->routing_key.bytes);
#endif
            result = amqp_simple_wait_frame(mConnection, &frame);
            if (result < 0) {
                LOG(Logger::Error, "RPC amqp_simple_wait_frame failed\n");
                break;
            }
            if (frame.frame_type != AMQP_FRAME_HEADER) {
                LOG(Logger::Error, "Expected header!");
                break;
            }
            p = (amqp_basic_properties_t *) frame.payload.properties.decoded;
#ifndef QT_NOT_DEBUG
            if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
                LOG(Logger::DDebug, "\nContent-type: %.*s\n",
                       (int) p->content_type.len, (char *) p->content_type.bytes);
            }
#endif
            body_target = frame.payload.properties.body_size;
            body_received = 0;
            while (body_received < body_target) {
                result = amqp_simple_wait_frame(mConnection, &frame);
                if (result < 0) {
                    LOG(Logger::Error, "RPC amqp_simple_wait_frame wait for body failed\n");
                    break;
                }
                if (frame.frame_type != AMQP_FRAME_BODY) {
                    LOG(Logger::Error, "Expected body!");
                    break;
                }
                body_received += frame.payload.body_fragment.len;
                if (frame.payload.body_fragment.len > 0) {
                    return_buffer = (char*)malloc(frame.payload.body_fragment.len + 1);
                    memset(return_buffer, '\0', frame.payload.body_fragment.len + 1);
                    strncpy(return_buffer, (char*)frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
                    ret += QString(return_buffer);
                    free(return_buffer);
                }
            }
            if (body_received != body_target) {
                /* Can only happen when amqp_simple_wait_frame returns <= 0 */
                /* We break here to close the connection */
                LOG(Logger::DDebug, "RPC body_received != body_target\n");
                ret = "";
                break;
            }
            //amqp_basic_ack(mConnection, 1, d->delivery_tag, 0);
        } while(false);
#ifdef EEPBX
    } while (!ret.startsWith(rpcID));
#endif
    RpcLock.unlock();
#ifdef EEPBX
    ret = ret.right(ret.length() - (ret.lastIndexOf("\r\r\n\n") + 4));
#endif
    return ret;
}

EventMonitor::EventMonitor(
        const QString &host,
        const QString &eventBindingkey,
        const QString &eventExchange,
        int port,
        const QString &user,
        const QString &passwd,
        QObject *parent) :
    QThread(parent),
    mHost(host), mPort(port), mBindingkey(eventBindingkey),
    mExchange(eventExchange), mUser(user), mPasswd(passwd)
{
    connect(this, SIGNAL(removeCallbackSignal(QString)), this, SLOT(removeCallbackSlot(QString)));
}

EventMonitor::~EventMonitor()
{
    LOG(Logger::Debug, "EventMonitor deleting\n");
}

bool EventMonitor::initialise()
{
    mStop = false;
    init_amqp_pool(&mPool, 0);
    int sockfd;
    amqp_bytes_t queuename;
    amqp_rpc_reply_t rpc_reply;

    LOG(Logger::Debug, "EventMonitor starting\n");
    /* 建立连接 */
    mConnection = amqp_new_connection();
    sockfd = amqp_open_socket(mHost.toStdString().c_str(), mPort);
    if (sockfd < 0) {
        LOG(Logger::Warn, "rabbit_mq_receiver_run: Opening socket failed\n");
        fireSystemEventCallback("receiver_thread_func socket open failed");
        return false;
    }
    amqp_set_sockfd(mConnection, sockfd);
    /* 登录设备 */
    rpc_reply = amqp_login(mConnection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, mUser.toStdString().c_str(), mPasswd.toStdString().c_str());
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rabbit_mq_receiver_run: Logging in")){
        fireSystemEventCallback("receiver_thread_func loging in failed");
        return false;
    }
    /* 打开通道 */
    amqp_channel_open(mConnection, 1);
    rpc_reply = amqp_get_rpc_reply(mConnection);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rabbit_mq_receiver_run: Opening channel")){
        fireSystemEventCallback("receiver_thread_func open channel failed");
        return false;
    }
    /* 声明一个队列 */
    amqp_queue_declare_ok_t *r = amqp_queue_declare(mConnection, 1, amqp_empty_bytes_local, 0, 0, 0, 1,
                            amqp_empty_table_local);
    rpc_reply = amqp_get_rpc_reply(mConnection);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rabbit_mq_receiver_run: Declaring queue")){
        fireSystemEventCallback("receiver_thread_func declaring queue failed");
        return false;
    }
    queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL) {
        LOG(Logger::DDebug, "rabbit_mq_receiver_run: Out of memory while copying queue name");
        fireSystemEventCallback("receiver_thread_func out of memory");
        return false;
    }
    /* Exchange与queue进行绑定 */
    amqp_queue_bind(mConnection, 1, queuename,
                    amqp_cstring_bytes(mExchange.toStdString().c_str()),
                    amqp_cstring_bytes(mBindingkey.toStdString().c_str()),
                    amqp_empty_table_local);
    rpc_reply = amqp_get_rpc_reply(mConnection);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rabbit_mq_receiver_run: Binding queue")){
        fireSystemEventCallback("receiver_thread_func binding queue failed");
        return false;
    }
    amqp_basic_consume(mConnection, 1, queuename, amqp_empty_bytes_local, 0, 1, 0, amqp_empty_table_local);
    rpc_reply = amqp_get_rpc_reply(mConnection);
    if (!rabbit_mq_reply_is_ok(rpc_reply, "rabbit_mq_receiver_run: Consuming")){
        fireSystemEventCallback("receiver_thread_func consuming failed");
        return false;
    }
    start();
    printf("rabbit_mq_receiver_run: ready to listen\n");
    return true;
}

void EventMonitor::run()
{
    amqp_frame_t frame;
    int result;

    amqp_basic_deliver_t *d;
#ifndef QT_NO_DEBUG
    amqp_basic_properties_t *p;
#endif
    size_t body_target;
    size_t body_received;

    while (!mStop) {
        amqp_maybe_release_buffers(mConnection);
        result = amqp_simple_wait_frame(mConnection, &frame);
        LOG(Logger::DDebug, "Result %d\n", result);
        if (result < 0)
            break;
        LOG(Logger::DDebug, "Frame type %d, channel %d\n", frame.frame_type, frame.channel);
        if (frame.frame_type != AMQP_FRAME_METHOD)
            continue;
        LOG(Logger::DDebug, "Method %s\n", amqp_method_name(frame.payload.method.id));
        if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD)
            continue;

        d = (amqp_basic_deliver_t *) frame.payload.method.decoded;
        LOG(Logger::DDebug, "Delivery %u, exchange %.*s routingkey %.*s\n",
               (unsigned) d->delivery_tag,
               (int) d->exchange.len, (char *) d->exchange.bytes,
               (int) d->routing_key.len, (char *) d->routing_key.bytes);
        result = amqp_simple_wait_frame(mConnection, &frame);
        if (result < 0)
            break;
        if (frame.frame_type != AMQP_FRAME_HEADER) {
            LOG(Logger::DDebug, "rabbit_mq_receiver_run: Expected header!");
            break;
        }
#ifndef QT_NO_DEBUG
        p = (amqp_basic_properties_t *) frame.payload.properties.decoded;
        if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
            LOG(Logger::DDebug, "\nContent-type: %.*s\n",
               (int) p->content_type.len, (char *) p->content_type.bytes);
        }
#endif

        body_target = frame.payload.properties.body_size;
        body_received = 0;

        while (body_received < body_target) {
            result = amqp_simple_wait_frame(mConnection, &frame);
            if (result < 0)
            break;

            if (frame.frame_type != AMQP_FRAME_BODY) {
                LOG(Logger::DDebug, "rabbit_mq_receiver_run: Expected body!");
                break;
            }
            body_received += frame.payload.body_fragment.len;
#ifndef QT_NO_DEBUG
            assert(body_received <= body_target);
            /*
            amqp_dump(frame.payload.body_fragment.bytes,
                  frame.payload.body_fragment.len);
                  */
#endif
            /* 把数据通过回调送出 */
            int len = frame.payload.body_fragment.len + 1;
            char* message = (char*)malloc(len);
            memset(message, '\0', len);
            memcpy(message, frame.payload.body_fragment.bytes, len - 1);
            QString msg = message;
            LOG(Logger::Debug, "mq msg: %s\n", msg.toStdString().c_str());
            fireMqMsgCallback(msg);
            free(message);
        }
        if (body_received != body_target) {
            /* Can only happen when amqp_simple_wait_frame returns <= 0 */
            /* We break here to close the connection */
            break;
        }
    }
    LOG(Logger::Notice, "rabbit_mq_receiver_run: quiting\n");
    if (!mStop) //因mq读多错误，此处发送消息到mainwindow，弹出提示，重启客户端
        fireSystemEventCallback("receiver_thread_func thread finished");
}

QString EventMonitor::addCallback(MessageClient::EventMonitorCallback c, MessageClient::EventType e)
{
    EventCallbackPair pair;
    pair.first = e;
    pair.second = c;
    QString uuid = QString(Tools::getUuid().c_str());
    mCallbacks.insert(uuid, pair);
    return uuid;
}

void EventMonitor::removeCallback(QString uuid)
{
    emit removeCallbackSignal(uuid);
}

void EventMonitor::removeCallbackSlot(QString uuid)
{
    if (mCallbacks.contains(uuid))
        mCallbacks.remove(uuid);
}

void EventMonitor::fireCallback(MessageClient::EventType e, QMap<QString, QString>& message)
{
    foreach (EventCallbackPair pair, mCallbacks.values())
        if (pair.first == e || pair.first == MessageClient::eALL)
            (pair.second)(e, message);
}

void EventMonitor::fireSystemEventCallback(QString message) {
    QMap<QString, QString> map;
    map.insert("system", message);
    fireCallback(MessageClient::eSystem, map);
}

void EventMonitor::fireMqMsgCallback(QString message) {
    LOG(Logger::Debug, "%s\n", message.toStdString().c_str());
    QMap<QString, QString> map;
    QXmlStreamReader xml(message);

    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "Status") {
            xml.readNext();
            while (!xml.atEnd()) {
                QString key = "";
                key.append(xml.name());
                QString value = xml.readElementText();
                if (key != "Status" || value != "" || !map.contains("Status"))
                    map.insert(key, value);

                xml.readNext();
            }
        }
        xml.readNext();
    }
    fireCallback(MessageClient::eMqEvent, map);
}

void EventMonitor::stop()
{
    mStop = true;
    this->terminate();
}

bool Housekeeper::check()
{
    int sockfd = amqp_open_socket(mHost.toStdString().c_str(), mPort);
    if (sockfd < 0)
        return false;
//    amqp_connection_state_t connection;
//    amqp_set_sockfd(connection, sockfd);
//    amqp_destroy_connection(connection);
    return true;
}

static bool rabbit_mq_reply_is_ok(amqp_rpc_reply_t x, const char* context)
{
    switch (x.reply_type) {
    case AMQP_RESPONSE_NORMAL:
        return true;
    case AMQP_RESPONSE_NONE:
        LOG(Logger::Debug, "%s: missing RPC reply type!\n", context);
        break;
    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        LOG(Logger::Debug, "%s: %s\n", context, amqp_error_string(x.library_error));
        break;
    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id) {
            case AMQP_CONNECTION_CLOSE_METHOD: {
            amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
            LOG(Logger::Debug, "%s: server connection error %d, message: %.*s\n",
                context,
                m->reply_code,
                (int) m->reply_text.len, (char *) m->reply_text.bytes);
            break;
            }
            case AMQP_CHANNEL_CLOSE_METHOD: {
            amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
            LOG(Logger::Debug, "%s: server channel error %d, message: %.*s\n",
                context,
                m->reply_code,
                (int) m->reply_text.len, (char *) m->reply_text.bytes);
            break;
            }
            default:
            LOG(Logger::Debug, "%s: unknown server error, method id 0x%08X\n", context, x.reply.id);
            break;
        }
        break;
    }
  return false;
}

static int read_timeout(int fd, int sec) {
    fd_set rset;
    struct timeval tv;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
#ifdef EEPBX
    tv.tv_sec = sec * 10;
#else
    tv.tv_sec = sec;
#endif
    tv.tv_usec = 0;
    return (select(fd+1, &rset, NULL, NULL, &tv));
}

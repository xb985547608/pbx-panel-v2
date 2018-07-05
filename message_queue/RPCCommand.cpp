#include "RPCCommand.h"
#include "MessageClient.h"
#include "misc/logger.h"
#include "misc/Config.h"
#include "ui/mainwidget/QueueZone.h"
#include "misc/dbwrapper.h"
#include "misc/ftpclient.h"

#ifdef WIN32
static void usleep(qint64 uMsec)
{
    QTime dieTime = QTime::currentTime().addMSecs(uMsec / 1000);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
#else
//#include <unistd.h>
#endif

QString RPCCommand::mLastErrorMessage = "";
QString RPCCommand::mSessionId = "";

QString RPCCommand::getLastErrorMessage()
{
    return mLastErrorMessage;
}

void RPCCommand::clearCache()
{
    QString cmd = QString("<Method><Name>clear-cache</Name></Method>");
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "result of clearCache: %s\n", result.toStdString().c_str());
}

bool RPCCommand::operatorLogin(QString username, QString password)
{
    QString command = QString(
                "<Method>"
                    "<Name>operator-login</Name>"
                    "<Username>%1</Username>"
                    "<Password>%2</Password>"
                "</Method>").arg(username).arg(password);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    bool ret = false;
    QString tmp = "";
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "Result") {
            xml.readNext();
            while (!xml.atEnd()) {
                if (xml.isStartElement()) {
                    if (xml.name() == "Code") {
                        if (xml.readElementText().toUpper() == "SUCCESS")
                            ret = true;
                    }
                    if (xml.name() == "Value") {
                        tmp = xml.readElementText();
                    }
                }
                xml.readNext();
            }
        } else
            xml.readNext();
    }
    if (ret) {
        mSessionId = tmp;
        LOG(Logger::Debug, "operator-login successfully with sessionid(%s)\n", mSessionId.toStdString().c_str());
    } else {
        mSessionId = "";
        mLastErrorMessage = tmp;
        LOG(Logger::Debug, "operator-login failed with reason: %s\n", mLastErrorMessage.toStdString().c_str());
    }
    return ret;
}

#ifdef EEPBX
QMap<QString, int> RPCCommand::getStorageState()
{
    QMap<QString, int> storage;
    QString cmd = "<Method><Name>get-storage-state</Name></Method>";
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    QXmlStreamReader xml(result);
    //<Result><Storage><Name>/dev/shm</Name><Value>0</Value></Storage><Storage><Name>/</Name><Value>70</Value></Storage></Result>'

    QString tmp;
    while (!xml.atEnd()) {
        tmp = QString(xml.name().data());
        if (xml.isStartElement() && xml.name() == "Storage") {
            xml.readNext();
            QString mountPoint;
            int percentage = 0;
            while (!xml.isEndDocument() && !(xml.name() == "Storage")) {
                tmp = QString(xml.name().data());
                if (xml.name() == "Name")
                    mountPoint = xml.readElementText();
                if (xml.name() == "Value")
                    percentage = xml.readElementText().toInt();
                xml.readNext();
            }
            storage.insert(mountPoint, percentage);
        }
        xml.readNext();
    }
    return storage;
}

void RPCCommand::removePath(QString path)
{
    QString cmd = QString("<Method><Name>remove-file</Name><Path>%1</Path></Method>").arg(path);
    MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
}
#endif

QMap<QString, PBX::RingGroup> RPCCommand::getRinggroups(QString groupNumber)
{
    QMap<QString, PBX::RingGroup> ringgroups;
    QString command = QString(
                "<Method>"
                    "<Name>get-ringgroups</Name>"
                    "<SessionID>%1</SessionID>"
                    "<Number>%2</Number>"
                "</Method>").arg(mSessionId).arg(groupNumber);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);

    QString tmp;
    while (!xml.atEnd()) {
        tmp = QString(xml.name().data());
        if (xml.isStartElement() && xml.name() == "RingGroupList") {
            xml.readNext();
            while (!xml.isEndDocument() && !(xml.name() == "RingGroupList")) {
                tmp = QString(xml.name().data());
                if (xml.name() == "RingGroup") {
                    xml.readNext();
                    PBX::RingGroup ringgroup;
                    while (!xml.isEndDocument() && !(xml.name() == "RingGroup")) {
                        if (xml.name() == "grpnum")
                            ringgroup.number = xml.readElementText();
                        if (xml.name() == "description") {
                            QString description = xml.readElementText();
                            LOG(Logger::Debug, "%s\n", description.toStdString().c_str());
                            ringgroup.description = description;
                        }
                        if (xml.name() == "grplist")
                            ringgroup.extenNumbers = xml.readElementText().split("-", QString::SkipEmptyParts);
                        xml.readNext();
                    }
                    if (!ringgroups.contains(ringgroup.number))
                        ringgroups.insert(ringgroup.number, ringgroup);
                    else
                        LOG(Logger::Notice, "Ringgroups already contains ringgroup(%s): %s\n",
                            ringgroup.number.toStdString().c_str(),
                            ringgroup.asString().toStdString().c_str());
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return ringgroups;
}

QMap<QString, PBX::Extension> RPCCommand::getExtensionDetailed(QString number, int wait)
{
    LOG(Logger::Debug, "getExtensionDetailed for extension: %s\n", number.toStdString().c_str());
    usleep(100000 * wait);
    QMap<QString, PBX::Extension> extensions;
    QString command = QString(
                "<Method>"
                    "<Name>get-extension-detail</Name>"
                    "<SessionID>%1</SessionID>"
                    "<Ext-number>%2</Ext-number>"
                "</Method>").arg(mSessionId).arg(number);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    LOG(Logger::Debug, "%s\n", result.toStdString().c_str());
    /*
    <Result><UserInfoList>
    <UserInfo>
        <Peers>
            <channel>SIP/8011-1c003100</channel>
            <peer_channel>SIP/8012-1f822b90</peer_channel>
            <peer_name>8011</peer_name>
            <peer_number>8011</peer_number>
            <state>Up</state>
            <time>1368007252761</time>
        </Peers>
        <context>from-internal</context>
        <extension>8011</extension>
        <id>455</id>
        <name>8011</name>
        <status>Reachable</status>
    </UserInfo>
    </UserInfoList></Result>
    */
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "UserInfoList") {
            xml.readNext();
            while (!xml.isEndElement() && !(xml.name() == "UserInfoList")) {
                if (xml.isStartElement() && xml.name() == "UserInfo") {
                    xml.readNext();
                    PBX::Extension e;
                    while (!xml.isEndDocument() && !(xml.name() == "UserInfo")) {
                        if (xml.name() == "name")
                            e.name = xml.readElementText();
                        if (xml.name() == "displayname")
                            e.displayName = xml.readElementText();
                        if (xml.name() == "extension")
                            e.number = xml.readElementText();
                        if (xml.name() == "context")
                            e.context = xml.readElementText();
                        if (xml.name() == "status") {
                            QString status = xml.readElementText();
                            if (status.toUpper() == "REGISTERED" || status.toUpper() == "REACHABLE")
                                e.state = PBX::eIdle;
                            else
                                e.state = PBX::eOffline;
                        }
                        if (xml.isStartElement() && xml.name() == "Peers") {
                            PBX::PeerInfo peer;
                            peer.state = PBX::eOffline;
                            xml.readNext();
                            while (!xml.isEndElement() || xml.name() != "Peers") {
                                /*
                                QString tagName = xml.name().toString();
                                bool isEndElement = xml.isEndElement();
                                LOG(Logger::Debug, "tagName: %s, is end element: %s\n", tagName.toStdString().c_str(),
                                    isEndElement ? "yes" : "no");
                                    */
                                if (xml.name() == "channel")
                                    peer.channel = xml.readElementText();
                                if (xml.name() == "peer_channel")
                                    peer.peerChannel = xml.readElementText();
                                if (xml.name() == "peer_name")
                                    peer.name = xml.readElementText();
                                if (xml.name() == "peer_number")
                                    peer.number = xml.readElementText();
                                if (xml.name() == "state") {
                                    QString state = xml.readElementText();
                                    if (state == "Ring")
                                        peer.state = PBX::eRing;
                                    if (state == "Ringing")
                                        peer.state = PBX::eRinging;
                                    if (state == "Up")
                                        peer.state = PBX::eIncall;
                                }
                                if (xml.name() == "time") {
                                    QString now = xml.readElementText();
                                    if (now.length() > 10)
                                        now = now.left(10);
                                    peer.upTime = QDateTime::fromTime_t(now.toUInt());
                                }
                                xml.readNext();
                            }
                            /*
                            QString tagName = xml.name().toString();
                            bool isEndElement = xml.isEndElement();
                            LOG(Logger::Debug, "TagName: %s, is end element: %s\n", tagName.toStdString().c_str(),
                                isEndElement ? "yes" : "no");
                                */
                            e.peers.insert(peer.channel, peer);
                        }
                        xml.readNext();
                    }
                    extensions.insert(e.number, e);
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return extensions;
    /*
    <result>
    　　<extension>
        　　<number>分机号
        　　<displayname>显示名称
        　　<tech>所用协议，如SIP或IAX
        　　<name>对应管理web中的用户姓名
        　　<state> 状态：online/offline
        　　<peer>对端：在处于ring/ringing/incall状态时，需给出正在通话的对端的号码或应用（如13880880030或meetme）
            　　<peer-channel>对端channel
            　　<peer-name>
            　　<peer-number>
            　　<channel>本端channel
            　　<state> 状态：incall/ring/ringing
            <context> 分机发起拨号时的拨号方案起始位置
    <result>
    */
}

QList<PBX::CurrentCall> RPCCommand::getConcurrentCalls(QString extension)
{
    QList<PBX::CurrentCall> calls;
    QString command = QString (
                "<method>"
                    "<name>get-concurrent-calls</name>"
                    "<extension>%1</extension>"
                "</method>");
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    /*
    <result>
    　　<call>
    　　<channel> SIP/8001-123456
    　　<state> ring/ringing/incall
    　　<peer-channel> 对端通道
    　　<peer-name>
    　　<peer-number>
    　　<duration> 默认为0，表示从通话处于incall状态到返回时持续的时间（秒）
    </result>
    */
    while (!xml.atEnd()) {
        xml.readNext();
    }
    return calls;
}

QMap<QString, PBX::Extension> RPCCommand::getOperators()
{
    QMap<QString, PBX::Extension> ret;
    QString command = QString("<Method><Name>get-operator</Name></Method>");
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    //<Result><Extens>SIP/8012,SIP/8013</Extens></Result>
    QString extens;
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "Extens") {
            extens = xml.readElementText();
            break;
        }
        xml.readNext();
    }
    QStringList extList = extens.split(",", QString::SkipEmptyParts);
    foreach (QString e, extList) {
        QString number = e.right(e.length() - (e.lastIndexOf("/") + 1));
        QMap<QString, PBX::Extension> operators = RPCCommand::getExtensionDetailed(number);
        foreach (QString opNumber, operators.keys())
            ret.insert(opNumber, operators[opNumber]);
    }
    return ret;
}

void RPCCommand::setOperators(QStringList numbers)
{
    QString extens;
    foreach (QString number, numbers) {
        extens += QString("SIP/%1,").arg(number);
    }
    extens = extens.left(extens.lastIndexOf(","));
    QString request = QString(
                "<Method>"
                    "<Name>set-operator</Name>"
                    "<Extens>%1</Extens>"
                "</Method>"
                ).arg(extens);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(request);
    LOG(Logger::Debug, "setOperators result: %s\n", result.toStdString().c_str());
}

QString RPCCommand::getOperatorQueueNumber()
{
    QString queueNumber = "";
    QString cmd = "<Method><Name>get-operator-queue</Name></Method>";
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.name() == "Result")
            queueNumber = xml.readElementText();
        xml.readNext();
    }
    return queueNumber;
}
/*QMap<QString, EventModel::Plan> RPCCommand::getEventPlans(QString planId)备份
QMap<QString, EventModel::Plan> RPCCommand::getEventPlans(QString planId)
{
    QMap<QString, EventModel::Plan> plans;
    QString command = QString (
                "<Method>"
                    "<Name>get-plans</Name>"
                    "<Id>%1</Id>"
                "</Method>"
                ).arg(planId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    /*
<Result>
    <EmergencyPlanList>
        <EmergencyPlan>
            <broadcast_file>xxx-xxx-xxx</broadcast_file>
            <id>2</id>
            <is_emergency>false</is_emergency>
            <name>xxx</name>
            <numbers>通讯录id-E-M-D-H,通讯录id-D-H</numbers>
            <sms_content>xxxx</sms_content>
            <terminals>8005,8006,8007</terminals>
        </EmergencyPlan>
    </EmergencyPlanList>
</Result>
    *//*
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "EmergencyPlanList") {
            xml.readNext();
            while (!xml.isEndElement() && xml.name() != "EmergencyPlanList") {
                if (xml.isStartElement() && xml.name() == "EmergencyPlan") {
                    xml.readNext();
                    EventModel::Plan plan;
                    while (!xml.isEndDocument() && xml.name() != "EmergencyPlan") {
                        if (xml.name() == "id")
                            plan.id = xml.readElementText();
                        if (xml.name() == "is_emergency") {
                            QString value = xml.readElementText();
                            if (value == "false")
                                plan.type = EventModel::eNormal;
                            else
                                plan.type = EventModel::eEmergency;
                        }
                        if (xml.name() == "name")
                            plan.name = xml.readElementText();
                        if (xml.name() == "sms_content")
                            plan.sms = xml.readElementText();
                        if (xml.name() == "terminals") {
                            QString terminals = xml.readElementText();
                            plan.terminals = terminals.split(",", QString::SkipEmptyParts);
                        }
                        if (xml.name() == "numbers") {
                            QStringList numberStrList = xml.readElementText().split(",", QString::SkipEmptyParts);
                            foreach (QString numberStr, numberStrList) {
                                QStringList numbers = numberStr.split("-", QString::SkipEmptyParts);
                                if (numbers.size() < 2) //至少需要 通讯录id+一个号码标识，如 1-M
                                    continue;
                                plan.numbers.insert(numbers[0], numberStr);
                            }
                        }
                        if (xml.name() == "broadcast_file") {
                            QString files = xml.readElementText();
                            plan.broadcastFiles = files.split(",", QString::SkipEmptyParts);
                        }
                        xml.readNext();
                    }
                    plans.insert(plan.id, plan);
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return plans;
}*/

QMap<QString, EventModel::Plan> RPCCommand::getEventPlans(QString planId)
{
    QMap<QString, EventModel::Plan> plans;
    QString command = QString (
                "<Method>"
                    "<Name>get-plans</Name>"
                    "<Id>%1</Id>"
                "</Method>"
                ).arg(planId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    LOG(Logger::Debug, "getEventPlans: request:\n%s\n", command.toStdString().c_str());
    LOG(Logger::Debug, "getEventPlans: response:\n%s\n", result.toStdString().c_str());
    QXmlStreamReader xml(result);
    /*
<Result>
    <PlanList>
        <Plan>
            <event_type>eNormal/eEmergency/eTimedEvent</event_type>
            <model_type>cycleNumberModel/executeTimeModel</model_type>
            <id></id>
            <name>xxx</name>
            <broadcast_file>xxx,oooo,pppp</broadcast_file>
            <numbers>通讯录id-E-D-M-H,通讯录id-M,通讯录id-E-D-H</numbers>
            <date>true,true,true,true,true,true,true</date>
            <execute_everyweeks>true</execute_everyweeks>
            <execute_date>Monday,Tuesday<execute_date>
            <start_time>09:00</start_time>
            <stop_time>10:00</stop_time>
            <ulimited_cycle>true</ulimited_cycle>
            <cycle_number>1</cycle_number>

            <is_emergency></is_emergency>
            <sms_content>xxxx</sms_content>
            <terminals>8001,8002,8003</terminals>
        </Plan>
    </PlanList>
</Result>
    */
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "PlanList") {
            xml.readNext();
            while (!xml.isEndElement() && xml.name() != "PlanList") {
                if (xml.isStartElement() && xml.name() == "Plan") {
                    xml.readNext();
                    EventModel::Plan plan;
                    while (!xml.isEndDocument() && xml.name() != "Plan") {
                        if (xml.name() == "event_type"){
                            QString value = xml.readElementText();
                            if (value == "eEmergency")
                                plan.type = EventModel::eEmergency;
                            else if (value == "eNormal")
                                plan.type = EventModel::eNormal;
                            else if (value == "eTimedEvent")
                                plan.type = EventModel::eTimedEvent;
                        }
                        if (xml.name() == "model_type")
                            plan.mModelSelect = xml.readElementText();
                        if (xml.name() == "id")
                            plan.id = xml.readElementText();
                        if (xml.name() == "name")
                            plan.name = xml.readElementText();
                        if (xml.name() == "broadcast_file") {
                            QString files = xml.readElementText();
                            plan.broadcastFiles = files.split(",", QString::SkipEmptyParts);
                        }
                        if (xml.name() == "numbers") {
                            QStringList numberStrList = xml.readElementText().split(",", QString::SkipEmptyParts);
                            foreach (QString numberStr, numberStrList) {
                                QStringList numbers = numberStr.split("-", QString::SkipEmptyParts);
                                if (numbers.size() < 2) //至少需要 通讯录id+一个号码标识，如 1-M
                                    continue;
                                plan.numbers.insert(numbers[0], numberStr);
                            }
                        }
                        if (xml.name() == "date"){
                            QString value = xml.readElementText();
                            QStringList dateList = value.split(",", QString::SkipEmptyParts);
                            foreach (QString temp, dateList) {
                                if (temp == "")
                                    continue;
                                else if (temp != "")
                                    plan.mDateList << temp;
                            }
                        }
                        if (xml.name() == "execute_everyweeks"){
                            QString value = xml.readElementText();
                            if (value == "true")
                                plan.mIsExecuteEveryWeeks = true;
                            else
                                plan.mIsExecuteEveryWeeks = false;
                        }
                        if (xml.name() == "execute_date"){
                            plan.mExecuteDate = xml.readElementText();
                        }
                        if (xml.name() == "start_time")
                            plan.mStartTime = xml.readElementText();
                        if (xml.name() == "stop_time")
                            plan.mStopTime = xml.readElementText();
                        if (xml.name() == "ulimited_cycle"){
                            QString value = xml.readElementText();
                            if (value == "true")
                                plan.mIsUlimiteCycle = true;
                            else
                                plan.mIsUlimiteCycle = false;
                        }
                        if (xml.name() == "cycle_number")
                            plan.mCycleNumber = xml.readElementText();

                        if (xml.name() == "sms_content")
                            plan.sms = xml.readElementText();
                        if (xml.name() == "terminals") {
                            QString terminals = xml.readElementText();
                            plan.terminals = terminals.split(",", QString::SkipEmptyParts);
                        }
                        xml.readNext();
                    }
                    plans.insert(plan.id, plan);
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return plans;
}

/*QString RPCCommand::createOrUpdateEventPlan(EventModel::Plan &plan)备份
QString RPCCommand::createOrUpdateEventPlan(EventModel::Plan &plan)
{
    /*old version
<Method>
    <Name>create-plan</Name> <Name>update-plans</Name>
    <EmergencyPlan>
        <broadcast_file>xxx,oooo,pppp</broadcast_file>
        <id></id>
        <is_emergency>false</is_emergency>
        <name>xxx</name>
        <numbers>通讯录id-E-D-M-H,通讯录id-M,通讯录id-E-D-H</numbers>
        <sms_content>xxxx</sms_content>
        <terminals>8001,8002,8006</terminals>
    </EmergencyPlan>
</Method>
    *//*
    QString planId;
    QString broadcastFiles = "";
    QString numbers = "";
    QString terminals = "";
    QString isEmergency = "false";
    QString method;

    if (plan.id == "")
        method = "create-plan";
    else
        method = "update-plans";
    if (plan.type == EventModel::eEmergency)
        isEmergency = "true";
    /*
    foreach (QString number, plan.numbers.keys()) {
        QStringList numberStr = plan.numbers.value(number);
        QString storedNumbers;
        foreach (QString storedNumber, numberStr)
            storedNumbers += storedNumber + "-";
        storedNumbers = storedNumbers.left(storedNumbers.lastIndexOf("-"));
        numbers += storedNumbers + ",";
    }
    *//*
    foreach (QString numberStr, plan.numbers.values())
        numbers += numberStr + ",";
    numbers = numbers.left(numbers.lastIndexOf(","));

    foreach (QString file, plan.broadcastFiles)
        broadcastFiles += file + ",";
    broadcastFiles = broadcastFiles.left(broadcastFiles.lastIndexOf(","));

    foreach (QString terminal, plan.terminals)
        terminals += terminal + ",";
    terminals = terminals.left(terminals.lastIndexOf(","));
    QString command = QString(
                "<Method>"
                    "<Name>%7</Name>"
                    "<EmergencyPlan>"
                        "<broadcast_file>%1</broadcast_file>"
                        "<id>%8</id>"
                        "<is_emergency>%2</is_emergency>"
                        "<name>%3</name>"
                        "<numbers>%4</numbers>"
                        "<sms_content>%5</sms_content>"
                        "<terminals>%6</terminals>"
                    "</EmergencyPlan>"
                "</Method>").arg(broadcastFiles).arg(isEmergency).arg(plan.name).arg(numbers).arg(plan.sms).arg(terminals).arg(method).arg(plan.id);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    LOG(Logger::Debug, "create-plan result: %s\n", result.toStdString().c_str());
    QXmlStreamReader xml(result);
    //如果method = create-plan，则得到planid并返回
    if (method == "create-plan") {
        while (!xml.atEnd()) {
            if (xml.isStartElement() && xml.name() == "Result")
                planId = xml.readElementText();
            xml.readNext();
        }
    }
    return planId;
}
*/

QString RPCCommand::createOrUpdateEventPlan(EventModel::Plan &plan)
{
    /*new version
<Method>
    <Plan>
        <Name>create-plan/update-plans</Name>
        <event_type>eNormal/eEmergency/eTimedEvent</event_type>
        <model_type>cycleNumberModel/executeTimeModel</model_type>
        <id></id>
        <name>xxx</name>
        <broadcast_file>xxx,oooo,pppp</broadcast_file>
        <numbers>通讯录id-E-D-M-H,通讯录id-M,通讯录id-E-D-H</numbers>
        <date>true,true,true,true,true,true,true</date>
        <execute_everyweeks>true</execute_everyweeks>
        <start_time>09:00</start_time>
        <stop_time>10:00</stop_time>
        <ulimited_cycle>true</ulimited_cycle>
        <cycle_number>1</cycle_number>

        <is_emergency></is_emergency>
        <sms_content>xxxx</sms_content>
        <terminals>8001,8002,8003</terminals>
    </Plan>
</Method>
    */
    QString planId;
    QString broadcastFiles = "";
    QString numbers = "";
    QString terminals = "";
    //QString isEmergency = "false"; isEmergency标签废弃，但保留不再赋值
    QString method;
    /*********定时广播********/
    QString eventType = "";
    QString modelType = "";
    QString dates = "";
    QString executeEveryWeeks = "false";
    QString executeDate = "";
    QString startTime = "";
    QString stopTime = "";
    QString ulimitedCycle = "false";
    QString cycleNumber = "";
    QString realNumbers = "";
    /***********************/
    QString command;

    if (plan.id == "")
        method = "create-plan";
    else
        method = "update-plans";

    if (plan.type == EventModel::eEmergency)
        eventType = "eEmergency";
    else if (plan.type == EventModel::eNormal)
        eventType = "eNormal";
    else if (plan.type == EventModel::eTimedEvent)
        eventType = "eTimedEvent";

    foreach (QString numberStr, plan.numbers.values()){
        if (numberStr == "")
            continue;
        else
            numbers += numberStr + ",";
    }
    numbers = numbers.left(numbers.lastIndexOf(","));

    foreach (QString file, plan.broadcastFiles){
        if (file == "")
            continue;
        else
            broadcastFiles += file + ",";
    }
    broadcastFiles = broadcastFiles.left(broadcastFiles.lastIndexOf(","));

    //非定时广播任务
    if (plan.type != EventModel::eTimedEvent){
        foreach (QString terminal, plan.terminals)
            terminals += terminal + ",";
        terminals = terminals.left(terminals.lastIndexOf(","));
        command = QString(
                    "<Method>"
                        "<Name>%1</Name>"
                        "<Plan>"
                            "<event_type>%2</event_type>"
                            "<model_type></model_type>"
                            "<id>%3</id>"
                            "<name>%4</name>"
                            "<broadcast_file>%5</broadcast_file>"
                            "<numbers>%6</numbers>"
                            "<date></date>"
                            "<execute_everyweeks></execute_everyweeks>"
                            "<execute_date></execute_date>"
                            "<start_time></start_time>"
                            "<stop_time></stop_time>"
                            "<ulimited_cycle></ulimited_cycle>"
                            "<cycle_number></cycle_number>"
                            "<is_emergency></is_emergency>"
                            "<sms_content>%7</sms_content>"
                            "<terminals>%8</terminals>"
                        "</Plan>"
                    "</Method>").arg(method).arg(eventType).arg(plan.id).arg(plan.name).arg(broadcastFiles).arg(numbers)
                .arg(plan.sms).arg(terminals);
    }
    //定时广播任务
    else if (plan.type == EventModel::eTimedEvent){
        eventType = "eTimedEvent";
        modelType = plan.mModelSelect;

        foreach (QString tempDate, plan.mDateList){
            if (tempDate == "")
                continue;
            else if (tempDate != "")
                dates = dates + tempDate + ",";
        }
        dates = dates.left(dates.lastIndexOf(","));
        if (!plan.mIsExecuteEveryWeeks){
            executeEveryWeeks = "false";
            executeDate = plan.mExecuteDate;
        }
        else
            executeEveryWeeks = "true";
        startTime = plan.mStartTime;

        if (modelType == "cycleNumberModel"){
            if (plan.mIsUlimiteCycle)
                ulimitedCycle = "true";
            else
                ulimitedCycle = "false";
            cycleNumber = plan.mCycleNumber;
            command = QString(
                        "<Method>"
                            "<Name>%1</Name>"
                            "<Plan>"
                                "<event_type>%2</event_type>"
                                "<model_type>%3</model_type>"
                                "<id>%4</id>"
                                "<name>%5</name>"
                                "<broadcast_file>%6</broadcast_file>"
                                "<numbers>%7</numbers>"
                                "<date>%8</date>"
                                "<execute_everyweeks>%9</execute_everyweeks>"
                                "<execute_date>%10</execute_date>"
                                "<start_time>%11</start_time>"
                                "<stop_time></stop_time>"
                                "<ulimited_cycle>%12</ulimited_cycle>"
                                "<cycle_number>%13</cycle_number>"
                                "<is_emergency></is_emergency>"
                                "<sms_content></sms_content>"
                                "<terminals></terminals>"
                            "</Plan>"
                        "</Method>").arg(method).arg(eventType).arg(modelType)
                    .arg(plan.id).arg(plan.name).arg(broadcastFiles).arg(numbers)
                    .arg(dates).arg(executeEveryWeeks).arg(executeDate).arg(startTime).arg(ulimitedCycle)
                    .arg(cycleNumber);
        }
        else if (modelType == "executeTimeModel"){
            stopTime = plan.mStopTime;
            command = QString(
                        "<Method>"
                            "<Name>%1</Name>"
                            "<Plan>"
                                "<event_type>%2</event_type>"
                                "<model_type>%3</model_type>"
                                "<id>%4</id>"
                                "<name>%5</name>"
                                "<broadcast_file>%6</broadcast_file>"
                                "<numbers>%7</numbers>"
                                "<date>%8</date>"
                                "<execute_everyweeks>%9</execute_everyweeks>"
                                "<execute_date>%10</execute_date>"
                                "<start_time>%11</start_time>"
                                "<stop_time>%12</stop_time>"
                                "<ulimited_cycle></ulimited_cycle>"
                                "<cycle_number></cycle_number>"
                                "<is_emergency></is_emergency>"
                                "<sms_content></sms_content>"
                                "<terminals></terminals>"
                            "</Plan>"
                        "</Method>").arg(method).arg(eventType).arg(modelType)
                    .arg(plan.id).arg(plan.name).arg(broadcastFiles).arg(numbers)
                    .arg(dates).arg(executeEveryWeeks).arg(executeDate).arg(startTime)
                    .arg(stopTime);
        }
    }
    QString result = "";
    if (command != ""){
        result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
        LOG(Logger::Debug, "create-plan result: %s\n", result.toStdString().c_str());
        QXmlStreamReader xml(result);
        //如果method = create-plan，则得到planid并返回
        if (method == "create-plan") {
            while (!xml.atEnd()) {
                if (xml.isStartElement() && xml.name() == "Result")
                    planId = xml.readElementText();
                xml.readNext();
            }
        }
    }
    return planId;
}

void RPCCommand::deleteEventPlan(QString planId)
{
    QString command = QString(
                "<Method><Name>delete-plan</Name><Id>%1</Id></Method>").arg(planId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    LOG(Logger::Debug, "deleteEventPlan: %s %s\n", planId.toStdString().c_str(), result.toStdString().c_str());
}

QStringList RPCCommand::getBroadcastFileList()
{
    QStringList list;
    QString cmd = "<Method><Name>get-broadcast-files</Name></Method>";
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    //<Result><File>D:\kankan\upgrade_cache.cfg</File><File>D:\kankan\说明.txt</File></Result>
    LOG(Logger::Debug, "%s", result.toStdString().c_str());
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.name() == "File") {
            QString file = xml.readElementText();
            file = file.right(file.length() - (file.lastIndexOf("/") + 1));
            list.append(file);
            //list.append(FTPClient::_fromSpecialEncoding(file));
        }
        xml.readNext();
    }
    return list;
}

void RPCCommand::delBroadcastFile(QStringList fileList)
{
    QString files;
    foreach (QString file, fileList) {
        files += file + ";";
    }
    files = files.left(files.lastIndexOf(";"));
    if (files.length() == 0)
        return;
    QString cmd = QString("<Method><Name>delete-broadcast-files</Name><Files>%1</Files></Method>").arg(files);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "%s\n", result.toStdString().c_str());
}

QMap<QString, PhoneBook> RPCCommand::getPhonebook(QString ringgroupNumber)
{
    QString command = QString(
                "<Method>"
                    "<Name>get-phonebook</Name>"
                    "<Number>%1</Number>"
                "</Method>"
                ).arg(ringgroupNumber);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    return resolvePhonebookXml(xml);
}

QMap<QString, PhoneBook> RPCCommand::resolvePhonebookXml(QXmlStreamReader& xml)
{
    QMap<QString, PhoneBook> ret;
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "PhoneBookList") {
            xml.readNext();
            while (!xml.isEndElement() && xml.name() != "PhoneBookList") {
                if (xml.isStartElement() && xml.name() == "PhoneBook") {
                    xml.readNext();
                    PhoneBook phonebook;
                    while (!xml.isEndDocument() && xml.name() != "PhoneBook") {
                        if (xml.name() == "company")
                            phonebook.company = xml.readElementText();
                        if (xml.name() == "department")
                            phonebook.department = xml.readElementText();
                        if (xml.name() == "id")
                            phonebook.id = xml.readElementText();
                        if (xml.name() == "mobile")
                            phonebook.mobile = xml.readElementText();
                        if (xml.name() == "extension")
                            phonebook.extenNumber = xml.readElementText();
                        if (xml.name() == "position")
                            phonebook.position = xml.readElementText();
                        if (xml.name() == "name")
                            phonebook.name = xml.readElementText();
                        if (xml.name() == "officephone")
                            phonebook.homeNumber = xml.readElementText();
                        if (xml.name() == "directdid")
                            phonebook.directdid = xml.readElementText();
                        xml.readNext();
                    }
                    ret.insert(phonebook.id, phonebook);
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return ret;
}

QMap<QString, PhoneBook> RPCCommand::getPhonebookById(QStringList& phonbookId)
{
    QString ids;
    foreach (QString id, phonbookId)
        ids += id + ",";
    ids = ids.left(ids.lastIndexOf(","));
    QString command = QString(
                "<Method><Name>get-phonebook-by-id</Name><Ext-id>%1</Ext-id></Method>")
            .arg(ids);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xml(result);
    return resolvePhonebookXml(xml);
}

void RPCCommand::savePhonebook(PhoneBook phonebook)
{
    QString command = QString(
                "<Method>"
                "<Name>update-phonebook</Name>"
                "%1"
                "</Method>").arg(phonebook.toXmlString());
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    LOG(Logger::Debug, "savePhonebook: %s\n", result.toStdString().c_str());
}

QString RPCCommand::updateEvent(QString eventId, QString planId, QString title, QString description, QString recorder)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>create-update-event</Name>"
                    "<Event>"
                        "<id>%1</id>"
                        "<plan_id>%2</plan_id>"
                        "<title>%3</title>"
                        "<description>%4</description>"
                        "<recorder>%5</recorder>"
                        "<create_date></create_date>"
                    "</Event>"
                "</Method>").arg(eventId).arg(planId).arg(title).arg(description).arg(recorder);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    //返回的结果<Result>5</Result>
    QString id;
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "Result") {
            id = xml.readElementText();
            break;
        }
        xml.readNext();
    }
    return id;
}

QList<EventModel::Event> RPCCommand::getEvent(QString eventId)
{
    QList<EventModel::Event> events;
    QString cmd = QString(
                "<Method>"
                    "<Name>get-event</Name>"
                    "<Id>%1</Id>"
                "</Method>").arg(eventId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    QXmlStreamReader xml(result);
    /*
    <Result>
    <EventList>
        <Event>
            <create_date>2013-03-29 11:18:00.0</create_date>
            <description>help mama.</description>
            <id>2</id>
            <plan_id>2</plan_id>
            <recorder>rock</recorder>
            <title>help</title>
        </Event>
    </EventList>
    </Result>
    */
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "EventList") {
            xml.readNext();
            while (!xml.isEndElement() && xml.name() != "EventList") {
                if (xml.isStartElement() && xml.name() == "Event") {
                    xml.readNext();
                    EventModel::Event event;
                    while (!xml.isEndElement() && xml.name() != "EventList") {
                        if (xml.name() == "create_date")
#ifdef EEPBX
                            event.created_date = QDateTime::fromString(xml.readElementText(), "yyyy-MM-dd HH:mm:ss");
#else
                            event.created_date = QDateTime::fromString(xml.readElementText(), "yyyy-MM-dd HH:mm:ss.z");
#endif
                        if (xml.name() == "description")
                            event.description = xml.readElementText();
                        if (xml.name() == "id")
                            event.id = xml.readElementText();
                        if (xml.name() == "plan_id")
                            event.planId = xml.readElementText();
                        if (xml.name() == "recorder")
                            event.recorder = xml.readElementText();
                        if (xml.name() == "title")
                            event.title = xml.readElementText();
                        xml.readNext();
                    }
                    events.append(event);
                }
                xml.readNext();
            }
        }
        xml.readNext();
    }
    return events;
}

QList<PBX::CDR> RPCCommand::getCDR(QDateTime &starttime, QDateTime &endtime)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>get-cdr</Name>"
                    "<Startdate>%1</Startdate>"
                    "<Enddate>%2</Enddate>"
                "</Method>").arg(starttime.toString("yyyy-MM-dd HH:mm:ss")).arg(endtime.toString("yyyy-MM-dd HH:mm:ss"));
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    /*
    <Cdr>
        <calldate>2013-03-11 20:54:04.0</calldate>
        <dst>122037178540110</dst>
        <duration>3</duration>
        <id>433</id>
        <src>8002</src>
        <userfield>OUT8002-20130311205404-538</userfield>
    </Cdr>
    */
    QList<PBX::CDR> cdrs;
    //result = "<Result><CdrList>\n<Cdr>\n <calldate>2015-05-08 19:54:19.0</calldate>\n<clid>\"Operator\" &lt;8001&gt;</clid>\n           <dcontext>from-internal</dcontext>\n                <dst>8002</dst>\n                 <duration>141</duration>\n                <id>1</id>\	n                 <src>8001</src>\n                <userfield>IN-8002-20150508195419-0</userfield>\n              </Cdr>\n              <Cdr>\n                 <calldate>2015-05-08 20:18:22.0</calldate>\n              <clid>\"Operator\" &lt;8001&gt;</clid>\n                <dcontext>from-internal</dcontext>\n             <dst>8002</dst>\n              <duration>18</duration>\n               <id>2</id>\n                 <src>8001</src>\n              <userfield>IN-8002-20150508201822-3</userfield>\n              </Cdr>\n             </CdrList>            </Result>";
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "Cdr") {
            xml.readNext();
            PBX::CDR cdr;
            while (!xml.isEndElement() ) {
                QStringRef name = xml.name();
                QString text = xml.readElementText();
                if( name == "Cdr"){
                    break;
                }


                if (name == "calldate")
#ifdef EEPBX
                    cdr.calldate = QDateTime::fromString(text, "yyyy-MM-dd HH:mm:ss");
#else
                    cdr.calldate = QDateTime::fromString(text, "yyyy-MM-dd HH:mm:ss.z");
#endif
                if (name == "dst")
                    cdr.dst = text;
                if (name == "duration")
                    cdr.duration = text;
                if (name == "src")
                    cdr.src = text;
                if (name == "id")
                    cdr.id = text;
                if (name == "userfield")
                    cdr.userField = text;
                xml.readNext();

            }
            cdrs.append(cdr);
        }
        xml.readNext();
    }
    return cdrs;
}

void RPCCommand::delCDR(QStringList idList)
{
    QString idStr;
    foreach (QString id, idList)
        idStr += id + ",";
    idStr = idStr.left(idStr.lastIndexOf(","));
    QString cmd = QString("<Method><Name>delete-cdr</Name><Id>%1</Id></Method>").arg(idStr);
    MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
}

QString RPCCommand::getIp()
{
    QString ipaddr;
    QString cmd = "<Method><Name>get-ip</Name></Method>";
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.name() == "Result")
            ipaddr = xml.readElementText();
        xml.readNext();
    }
    if (ipaddr == "127.0.0.1" || ipaddr.startsWith("192.168.122."))
        ipaddr = Config::Instance()->mq_host;
    return ipaddr;
}

QMap<int, QString> RPCCommand::getSmsTemplates()
{
    QMap<int, QString> smsTemplates = DbWrapper::getSmsTemplates();
    return smsTemplates;
}

void RPCCommand::saveSmsTemplate(int template_id, QString sms)
{
    LOG(Logger::Debug, "saving %s as template id: %d\n", sms.toStdString().c_str(), template_id);
    DbWrapper::updateSmsTempate(template_id, sms);
}

void RPCCommand::sendSms(QString sender, QStringList receivers, QString content)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>send-sms</Name>"
                    "<From>%1</From>"
                    "<To>%2</To>"
                    "<Content>%3</Content>"
                    "<ActionId>send-sms</ActionId>"
                "</Method>");//.arg(sender).arg(to).arg(content);
    foreach (QString to, receivers) {
        QString command = cmd.arg(sender).arg(to).arg(content);
        MessageClient::Instance()->getRPCClient()->rpcCall(command);
    }
}

QStringList RPCCommand::getMeetmeRooms()
{
    QString cmd = "<Method><Name>get-meetme-list</Name></Method>";
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    //<Result><Meetme>8888;9999</Meetme></Result>
    QXmlStreamReader xml(result);
    QStringList rooms;
    while (!xml.atEnd()) {
        if (xml.name() == "Meetme") {
            QString roomString = xml.readElementText();
            rooms = roomString.split(";", QString::SkipEmptyParts);
        }
        xml.readNext();
    }
    return rooms;
}

QMap< QString, QMap<QString, PBX::MeetmeMember> > RPCCommand::getMeetmeInfo(QString roomId)
{
    QMap< QString, QMap<QString, PBX::MeetmeMember> > rooms;
    QString cmd = QString("<Method><Name>get-meetme</Name><Meetme>%1</Meetme></Method>").arg(roomId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    /*
    <Result>
        <MeetmeList>
            <Meetme>
                <Members>
                    <callerid>8016</callerid>
                    <channel>SIP/8016-1c56cb20</channel>
                    <duration>00:00:19</duration>
                    <is_admin></is_admin>
                    <is_mute></is_mute>
                    <usernum>1</usernum>
                </Members>
                <number>8888</number>
            </Meetme>
        </MeetmeList>
    </Result>
    */
    LOG(Logger::Debug, "result: %s\n", result.toStdString().c_str());
    Config* cfg = Config::Instance();
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.name() == "Meetme") {
            xml.readNext();
            QMap<QString, PBX::MeetmeMember> room;
            QString roomid = "";
            while (!xml.atEnd() && xml.name() != "Meetme") {
                if (xml.name() == "number")
                    roomid = xml.readElementText();
                if (xml.name() == "Members") {
                    xml.readNext();
                    PBX::MeetmeMember member;
                    while (!xml.atEnd() && xml.name() != "Members") {
                        if (xml.name() == "callerid")
                            member.callerid = xml.readElementText();
                        if (xml.name() == "channel")
                            member.channel = xml.readElementText();
                        if (xml.name() == "is_admin")
                            member.isAdmin = (xml.readElementText() == "yes") ? true : false;
                        if (xml.name() == "is_mute")
                            member.isMuted = (xml.readElementText() == "on") ? true : false;
                        if (xml.name() == "usernum")
                            member.userNum = xml.readElementText();
                        xml.readNext();
                    }
                    if (member.userNum != "") {
                        QString callerid;
                        if (member.channel.contains("@"))
                            callerid = member.channel.left(member.channel.lastIndexOf("@"));
                        else
                            callerid = member.channel.left(member.channel.lastIndexOf("-"));
                        callerid = callerid.right(callerid.length() - (callerid.lastIndexOf("/") + 1));
                        if (callerid == cfg->DefaultCallerId || callerid == QueueZone::getQueueNumber())
                            member.callerid = QString("调度员");
                        else if (callerid == cfg->SoundPlayerCallerId)
                            member.callerid = QString("播放机");
                        else
                            member.callerid = callerid;
                        room.insert(member.userNum, member);
                    }
                }
                xml.readNext();
            }
            rooms.insert(roomid, room);
        }
        xml.readNext();
    }
    return rooms;
}

void RPCCommand::meetmeKick(QString roomid, QString userNum)
{
    QString cmd = QString(
                "<Method><Name>meetme-kick</Name><RoomID>%1</RoomID><UserNum>%2</UserNum></Method>"
                ).arg(roomid).arg(userNum);
    MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
}

void RPCCommand::meetmeMute(QString roomid, QString userNum, bool mute)
{
    QString cmd;
    if (mute)
        cmd = QString("<Method><Name>meetme-mute</Name><RoomID>%1</RoomID><UserNum>%2</UserNum></Method>").arg(roomid).arg(userNum);
    else
        cmd = QString("<Method><Name>meetme-unmute</Name><RoomID>%1</RoomID><UserNum>%2</UserNum></Method>").arg(roomid).arg(userNum);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "result: %s\n", result.toStdString().c_str());
}

QStringList RPCCommand::getMonitorFiles(QString fileName)
{
    QStringList files;
    QString cmd = QString("<Method><Name>get-recording-files</Name><File>%1</File></Method>").arg(fileName);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "result: %s\n", result.toStdString().c_str());
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        if (xml.name() == "File") {
            QString file = xml.readElementText();
            files.append(file);
        }
        xml.readNext();
    }
    return files;
}

bool RPCCommand::p2pCall(QString extension, QString callerIdName, QString callerIdNumber, QString calleeNumber, int timeOut)
{
    QString command = QString(
            "<Method>"
              "<Name>p2p-call</Name>"
               "<Channel>%1</Channel>"
               "<Exten>%2</Exten>"
               "<CallerId>%3 &lt;%4&gt;</CallerId>"
                "<Timeout>%5</Timeout>"
                "</Method>").arg(extension).arg(calleeNumber).arg(callerIdName).arg(callerIdNumber).arg(timeOut * 1000);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    QXmlStreamReader xmlReader(result);
    return simpleTrueFalseAndSetErrorMsg(xmlReader);
}

void RPCCommand::makeConferenceCall(QString channel, QString context, QString exten, QString callerid, QString roomid, bool isOperator, int timeout)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>meetme-invite</Name>"
                    "<Channel>%1</Channel>"
                    "<Context>%2</Context>"
                    "<Exten>%3</Exten>"
                    "<Priority>1</Priority>"
                    "<CallerId>%4</CallerId>"
                    "<Timeout>%9</Timeout>"
                    "<Async>1</Async>"
                    "<Variable>CONF_NO=%5|IS_OPERATOR=%6|RECORD_FILE=M-%7-%8</Variable>"
                    "<ActionId></ActionId>"
                "</Method>")
            .arg(channel).arg(context).arg(exten).arg(callerid).arg(roomid).arg(isOperator ? "1" : "0")
            .arg(roomid).arg(Config::Instance()->uptime).arg(timeout);
    usleep(Config::Instance()->SafetyInterval);
    MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
}

void RPCCommand::originate(
        QString channel,
        QString context,
        QString exten,
        QString callerid,
        QMap<QString, QString> variables,
        QString accountcode,
        int timeout,
        int prio,
        QString data,
        int async,
        QString actionid)
{
    QString variable = "";
    if (variables.size() > 0) {
        foreach (QString key, variables.keys())
            variable += key + "=" + variables.value(key) + "|";
        variable = variable.left(variable.lastIndexOf("|"));
    }
    QString asyncStr = "False";
    if (async == 1)
        asyncStr = "True";
    QString cmd = QString(
                "<Method>"
                    "<Name>originate</Name>"
                    "<Channel>%1</Channel>"
                    "<Context>%2</Context>"
                    "<Exten>%3</Exten>"
                    "<Priority>%9</Priority>"
                    "<CallerId>%4</CallerId>"
                    "<Timeout>%5</Timeout>"
                    "<Async>%6</Async>"
                    "<Variable>%7</Variable>"
                    "<ActionId>%8</ActionId>"
                "</Method>").arg(channel).arg(context).arg(exten).arg(callerid).arg(timeout).arg(asyncStr).arg(variable).arg(actionid).arg(prio);
    usleep(Config::Instance()->SafetyInterval);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "result: %s\n", result.toStdString().c_str());
    LOG(Logger::DDebug, "unused variable: data=%s, accountcode=%s\n", data.toStdString().c_str(), accountcode.toStdString().c_str());
}

void RPCCommand::hangup(QString channel)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>hangup</Name>"
                    "<Channel>%1</Channel>"
                "</Method>"
                ).arg(channel);
    MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
}

void RPCCommand::redirectCall(QString chan, QString context, QString target, QString extrachan, int prio, QString actionId)
{
    QString cmd = QString(
                "<Method>"
                    "<Name>redirect-call</Name>"
                    "<Channel>%1</Channel>"
                    "<ExtraChannel>%4</ExtraChannel>"
                    "<Exten>%3</Exten>"
                    "<Context>%2</Context>"
                    "<Priority>%5</Priority>"
                    "<ActionId>%6</ActionId>"
                "</Method>").arg(chan).arg(context).arg(target).arg(extrachan).arg(prio).arg(actionId);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "result: %s\n", result.toStdString().c_str());
}

QString RPCCommand::sipShowPeers()
{
    QString command = QString(
                "<Method>"
                    "<Name>sip-show-peers</Name>"
                "</Method>"
                );
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(command);
    return result;
}

bool RPCCommand::simpleTrueFalseAndSetErrorMsg(QXmlStreamReader &xml)
{
    bool ret = false;
    QString tmp = "";
    while (!xml.atEnd()) {
        if (xml.isStartElement()) {
            if (xml.name() == "Result") {
                tmp = xml.readElementText();
                if (tmp.toUpper() == "SUCCESS")
                    ret = true;
            }
            if (xml.name() == "Reason") {
                tmp = xml.readElementText();
                if (tmp != "") {
                    mLastErrorMessage = tmp;
                    LOG(Logger::Debug, "rpc result error: %s\n", tmp.toStdString().c_str());
                }
            }
        }
        /*
        else if (xmlReader.isEndElement())
            xmlReader.readNext();
        */
        xml.readNext();
    }
    return ret;
}

void RPCCommand::makeRollCall(QList<PBX::Extension> &extens)
{
    /*
    "<Method><Name>rollcall</Name>"  +
           "<Context>app-operator-rollcall</Context>" +
           "<Extens>SIP/208,SIP/802</Extens>" +
           "</Method>";
           */
    QString extenString = "";
    foreach (PBX::Extension e, extens)
        extenString += e.tech + "/" + e.number + ",";
    extenString = extenString.left(extenString.lastIndexOf(","));

    QString cmd = QString(
                "<Method>"
                    "<Name>rollcall</Name>"
                    "<Context>app-operator-rollcall</Context>"
                    "<Extens>%1</Extens>"
                "</Method>").arg(extenString);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "makeRollCall result: %s\n", result.toStdString().c_str());
}

void RPCCommand::makeRotationCall(QList<PBX::Extension> &extens)
{
    /*
    "<Method><Name>rotationcall</Name>"  +
    "<Extens>SIP/801,SIP/208</Extens>" +
    "</Method>";
    */
    QString extenString = "";
    foreach (PBX::Extension e, extens)
        extenString += e.tech + "/" + e.number + ",";
    extenString = extenString.left(extenString.lastIndexOf(","));

    QString cmd = QString(
                "<Method>"
                    "<Name>rotationcall</Name>"
                    "<Extens>%1</Extens>"
                "</Method>").arg(extenString);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "makeRotationCall result: %s\n", result.toStdString().c_str());
}

void RPCCommand::chaseCall(QString channel, QString context, QString exten, QString callerid, QMap<QString, QString> variables)
{
    /*
    String command = "<Method><Name>chasecall</Name>"  +
   "<Channel>Local/208@from-internal/n</Channel>" +
   "<Context>from-internal</Context>" +
   "<Exten>803</Exten>" +
   "<CallerId>208</CallerId>" +
         "</Method>";
         */
    QString variable = "";
    if (variables.size() > 0) {
        foreach (QString key, variables.keys())
            variable += key + "=" + variables.value(key) + "|";
        variable = variable.left(variable.lastIndexOf("|"));
    }
    QString cmd = QString(
                "<Method>"
                    "<Name>chasecall</Name>"
                    "<Channel>%1</Channel>"
                    "<Context>%2</Context>"
                    "<Exten>%3</Exten>"
                    "<CallerId>%4</CallerId>"
                    "<Variable>%5</Variable>"
                "</Method>").arg(channel).arg(context).arg(exten).arg(callerid).arg(variable);
    QString result = MessageClient::Instance()->getRPCClient()->rpcCall(cmd);
    LOG(Logger::Debug, "chaseCall result: %s\n", result.toStdString().c_str());
}

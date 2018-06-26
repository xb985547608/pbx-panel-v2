#ifndef RPCCOMMAND_H
#define RPCCOMMAND_H
#include <QtGui>
#include <pbx/pbx.h>
#include "pbx/EventModel.h"
#include "pbx/PhoneBook.h"

class RPCCommand
{
public:
    RPCCommand() {}
    static QString getLastErrorMessage();

    static void clearCache();
    static bool operatorLogin(QString username, QString password);    
#ifdef EEPBX
    /*
     * 取pbx各挂载点已用容量百分比
     */
    static QMap<QString, int> getStorageState();
    /*
     * 删除pbx上的文件、目录，可使用通配符
     *      如/var/spool/asterisk/monitor/133*
     * 		或/var/spool/asterisk/monitor/133* /var/spool/asterisk/monitor/aaa*
     */
    static void removePath(QString path);
#endif

    /*
     * @return: <ringgroup number, Ringgroup>
     */
    static QMap<QString, PBX::RingGroup> getRinggroups(QString groupNumber = "");
    /*
     * @wait: 发起rpc调用前等待的时间，1 = 0.1秒
     * @return: <extension number, Extension>
     */
    static QMap<QString, PBX::Extension> getExtensionDetailed(QString number = "", int wait = 0);

    /*
     * @extension: 分机，如SIP/8001
     */
    static QList<PBX::CurrentCall> getConcurrentCalls(QString extension = "");


    /*
     * @return: <number, Extension>
     */
    static QMap<QString, PBX::Extension> getOperators();

    static void setOperators(QStringList numbers);
    /*
     * 读取调度员所在队列编号
    */
    static QString getOperatorQueueNumber();

    /*
     * 读取事件模板
     * @return QMap<plan id, plan>
     */
    static QMap<QString, EventModel::Plan> getEventPlans(QString planId = "");
    /*
     *
     */
    static QString createOrUpdateEventPlan(EventModel::Plan&);
    static void deleteEventPlan(QString planId);

    static QString createOrUpdateTimingEventPlan(EventModel::Plan&);

    /*
     * get-broadcast-file-list
     */
    static QStringList getBroadcastFileList();
    static void delBroadcastFile(QStringList fileList);

    /*
     * 读取通讯录
     * QMap<phonebook id, class phonebook>
     */
    static QMap<QString, PhoneBook> getPhonebook(QString ringgroupNumber = "");
    static QMap<QString, PhoneBook> getPhonebookById(QStringList &phonbookId);

    /*
     * 保存通讯录记录
     */
    static void savePhonebook(PhoneBook);

    /*创建、更新紧急/一般事件*/
    static QString updateEvent(QString eventId, QString planId, QString title, QString description = "", QString recorder = "");

    /*读取事件*/
    static QList<EventModel::Event> getEvent(QString eventId = "");

    /*读cdr*/
    static QList<PBX::CDR> getCDR(QDateTime& starttime, QDateTime& endtime);

    /*按id号删除cdr*/
    static void delCDR(QStringList idList);

    /*取pbx ip*/
    static QString getIp();

    /**/
    static QStringList getMeetmeRooms();
    /*
    读取会议室详细信息
    QMap<roomid, QMap<userNum, MeetmeMember>>
    */
    static QMap< QString, QMap<QString, PBX::MeetmeMember> > getMeetmeInfo(QString roomId = "");
    static void meetmeKick(QString roomid, QString userNum);
    static void meetmeMute(QString roomid, QString userNum, bool mute);

    /* 读取通话录音文件 */
    QStringList getMonitorFiles(QString fileName);

    /*
    读短信模板
    QMap<template_id, sms>
    */
    static QMap<int, QString> getSmsTemplates();
    /*保存短信模板*/
    static void saveSmsTemplate(int template_id, QString sms);
    /* 短信发送*/
    static void sendSms(QString sender, QStringList receivers, QString content);
    /*
     * @extension: 分机，如 SIP/8001
     * @timeOut: 振铃时间，单位秒
     */
    static bool p2pCall(QString extension, QString callerIdName, QString callerIdNumber, QString calleeNumber, int timeOut = 30);    
    static void makeConferenceCall(QString channel, QString context, QString exten, QString callerid, QString roomid, bool isOperator, int timeout = 30000);
    static void originate(
            QString channel,
            QString context,
            QString exten,
            QString callerid,
            QMap<QString, QString> variables,
            QString accountcode = "",
            int timeout = 30000,
            int prio = 1,
            QString data = "",
            int async = 1,
            QString actionid = "");
    static void hangup(QString channel);
    static void redirectCall(QString chan, QString context, QString target, QString extrachan = "", int prio = 1, QString actionId = "");

    static QString sipShowPeers();

    static void makeRollCall(QList<PBX::Extension>& extens); //点名
    static void makeRotationCall(QList<PBX::Extension>& extens); //轮询
    static void chaseCall(
            QString channel,
            QString context,
            QString exten,
            QString callerid,
            QMap<QString, QString> variables);
private:
    static QString mLastErrorMessage;
    static bool simpleTrueFalseAndSetErrorMsg(QXmlStreamReader& xml);
    static QString mSessionId; //operator-login成功以后，保存返回的sessionid
    static QMap<QString, PhoneBook> resolvePhonebookXml(QXmlStreamReader& xml); //解析phonebook读回来的xml
};

#endif // RPCCOMMAND_H

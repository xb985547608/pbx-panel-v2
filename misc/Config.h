#ifndef CONFIG_H
#define CONFIG_H

#include <QtGui>
#include <misc/singleton.h>
#include "misc/tools.h"
#include "misc/ftpclient.h"

#define MAX_BUFFER_LEN 8192

class Config : public Singleton<Config>
{
 public :
    Config() : Singleton<Config>("Config") {}
    virtual ~Config(){}
    bool initialise(QString path);
    time_t uptime; //本次调度启动的时间

    int VERSION;
    QString VERSION_NAME;
    int TelnetPort;
    int FtpPort;
    QString TelnetUsername;
    QString TelnetPassword;
    int AmiPort;
    QString AmiUsername;
    QString AmiPassword;
    //"所有分机组"的名字
    const char* AllExtensionGroupName;
    //"所有分机组"的号码(应该是一个不存在的虚拟号码)
    QString AllExtensionGroupNumber;
    //"当前通话"组的名字
    const char* CurrentCallGroupName;
    //"当前通话"组的号码(应该是一个不存在的虚拟号码)
    QString CurrentCallGroupNumber;
    //"当前在线"组的名字
    //"当前在线"组的号码
    QString OnlineExtensionGroupName;
    QString OnlineExtensionGroupNumber;
    //operator分机使用的context，用于发起群呼给LOCAL/XXX@号码和通讯录呼叫
    QString DefaultContext;
    int AppWindowWidth;
    int AppWindowHeight;
    QString OperatorMeetmeRoom;
    QString OperatorBreakinContext;
    //呼叫operator振铃组使用的context
    QString OperatorGroupContext;
    //呼叫operator队列使用的context
    QString OperatorQueueContext;
    //发起群呼所用的context
    QString OperatorGroupCallContext;

    QString getGroupcallNumber(); //每次调用此方法，则返回一个新的会议号 = OperatorGroupcallBaseNumber + CurrentGroupcallNumber

    int RpcTimeout;

    //在一些pbx发起的呼叫中，需要一个默认的主叫号码
    QString DefaultCallerId;
    QString DefaultCallerIdName;

    //紧急呼叫号码前缀
    QString EmergencyNumber;

    //回放声音的通道callerid，须和dialplan中app-operator-page中对应，目前为10000
    QString SoundPlayerCallerId;
    //回放的声音文件所在路径
    QString PbxSoundFilePath;
    //录音文件所在路径
    QString PbxMonitorFilePath;
    //录音文件本地保存路径
    QString LocalMonitorFilePath;

    QString MixerCmd;

    CmdProcessor* cmdProcessor() { return mCmdProcessor; }
    FTPClient* ftpClient() { return mFtpClient; }
    void ftpClientInit(QString username, QString password, QString host, int port = 21, QObject* parent = 0);

    int SafetyInterval; //用于保护不让pbx崩溃的命令间时间间隔
#ifdef EEPBX
    int MaxAllowedCalls; //对小epbx最大通话数限制
    int StorageCheckInterval; //容量检查时间间隔，单位秒
    int StorageLimit; //接近存储上限提示时的容量百分比，0-100
#endif

    /*
     * 样式参数
     */
    QString groupboxStylesheet;
    int RingGroupFixedDisplayLength; //振铃组名字显示的长度

    /*
     * 界面相关参数
     */
    QString Window_Title;
    QString Res_Image_Banner_Header;
    QString Res_Icon_Logo;
    QString Res_Txt_Title;
    QString Res_icon_emergency;
    QString Res_icon_meete_control;
    QString Res_icon_cdr;
    QString Res_icon_settings;
    QString Res_icon_sms;
    QString Res_icon_sms_square;
    QString Res_icon_empty;
    QString Res_btn_makecall;
    QString Res_btn_broadcast;
    QString Res_btn_current_calls;
    QString Res_btn_play_sound;
    QString Res_btn_pickup;
    QString Res_btn_monitor;
    QString Res_btn_override;
    QString Res_btn_transfer;
    QString Res_btn_hangup;
    QString Res_icon_offline;
    QString Res_icon_online;
    QString Res_icon_ring;
    QString Res_icon_ringing;
    QString Res_icon_incall;
    QString Res_image_splash;
    QString Res_image_loading;
    QString Res_btn_alert;
    QString Res_btn_record_query;
    QString Res_btn_sound_record_list;
    QString Res_icon_checked;
    QString Res_icon_check_failed;
    QString Res_icon_unchecked;
    QString Res_icon_calendar;
    QString Res_icon_report;
    QString Res_icon_phone_book;
    QString Res_icon_phone_book_small;
    QString Res_icon_current_call;
    QString Res_icon_make_call;
    QString Res_icon_broadcast;
    QString Res_icon_play_record;
    QString Res_icon_monitor;
    QString Res_icon_hangup;
    QString Res_icon_override;
    QString Res_icon_transfer;
    QString Res_icon_pickup;
    QString Res_icon_rollcall;
    QString Res_icon_poll;
    QString Res_image_banner;
    QString Res_icon_back;
    QString Res_arrow_right;
    QString Res_icon_threeway;

    //misc
    QDate InvalidDate; //无效的日期，用于查询设置初值
    time_t GarbageCleanInterval; //清除无效数据，垃圾回收的时间，单位：秒
    QString TelnetBufferBeginIdentifier; //telnet到pbx发送命令，读取返回内容时，以此值作为本命令返回内容起始标志
    QString TelnetBufferEndIdentifier; //telnet到pbx发送命令，读取返回内容时，以此值作为本命令返回内容结束标志


    //message queue
    QString mq_host;
    QString mq_eventBindingkey;
    QString mq_eventExchange;
    QString mq_rpcRoutingkey;
    QString mq_rpcExchange;
    int mq_port;
    QString mq_user;
    QString mq_passwd;

#ifndef LOCKTEL
    QString OEM_TITLE;
#endif

    QString notifierUDPIP_;
    int     notifierUDPPort_;
    QString notifierTCPServerIP_;
    int     notifierTCPServerPort_;
private :
    CmdProcessor* mCmdProcessor;
    FTPClient* mFtpClient;
    QString mAppPath;
    /*
    强插操作时，使用的meetme room的基本号，比如000100101
    配合拨号方案
    [app-operator-breakin]
    exten => _00010010XXX,1,NoOp(app-operator-breakin)
    exten => _00010010XXX,n,Set(OPT=1Mqd)
    .....
    */
    QString OperatorGroupcallBaseNumber;
    int CurrentGroupcallNumber; //用于发起群呼、强插时，顺序增加的3位号码，从100开始

    void initStyleSheets();
};

#endif // CONFIG_H

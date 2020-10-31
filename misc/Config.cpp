#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "config.h"
#include <QSettings>
#include <QString>
#include <QDir>
#include <time.h>

bool Config::initialise(QString path)
{
    QString archtech;
#ifdef EEPBX
    archtech = "eepbx";
#else
    archtech = "epbx";
#endif
    uptime = time(NULL);
    VERSION_NAME = "(1.0.41-" + archtech + "-beta)";
    VERSION_NAME = "";
    VERSION = 41;
    mAppPath = path + "/";
    QString cfgFile = mAppPath + "config.ini";
    QSettings* settings = new QSettings(cfgFile, QSettings::IniFormat);
    this->TelnetPort = settings->value("telnet/port", 23).toInt();
    this->FtpPort = settings->value("ftp/port", 21).toInt();
#ifdef EEPBX
    this->TelnetUsername = settings->value("telnet/username", "root").toString();
    this->TelnetPassword = settings->value("telnet/password", "1:300wan").toString();
#else
    this->TelnetUsername = settings->value("telnet/username", "dingo").toString();
    this->TelnetPassword = settings->value("telnet/password", "qq11226278").toString();
#endif
    this->AmiPort = settings->value("ami/port", 5038).toInt();
    this->AmiUsername = settings->value("ami/username", "dingo").toString();
    this->AmiPassword = settings->value("ami/password", "dingo").toString();
    this->OperatorMeetmeRoom = settings->value("operator/room", "2000").toString();
    this->DefaultContext = settings->value("operator/default_context", "from-internal").toString();
    this->OperatorBreakinContext = settings->value("operator/breakin_context", "app-operator-breakin").toString();
    this->OperatorGroupcallBaseNumber = settings->value("operator/groupcall_number_base", "00010010").toString();
    this->CurrentGroupcallNumber = 100;
    this->OperatorGroupCallContext = settings->value("operator/groupcall_context", "app-operator-meetme").toString();
    this->OperatorGroupContext = "operator-group";
    this->OperatorQueueContext = "operator-queue";
    this->AllExtensionGroupName = "所有分机";
    this->AllExtensionGroupNumber = "number_of_all_exten";
    this->CurrentCallGroupName = "当前通话";
    this->CurrentCallGroupNumber = "hook";
    this->OnlineExtensionGroupName = "在线分机";
    this->OnlineExtensionGroupNumber = "number_of_online_exten";
    this->DefaultCallerId = "000000";
//    this->EmergencyNumber = "888";
    this->EmergencyNumber = settings->value("emergency/number", "2006").toString();
#ifdef EEPBX
    this->RpcTimeout = 30;
#else
    this->RpcTimeout = 200;
#endif
    this->DefaultCallerIdName = "Operator";
    this->SoundPlayerCallerId = "10000";
    this->PbxSoundFilePath = "/var/lib/asterisk/sounds/broadcast/";
    this->PbxMonitorFilePath = "/var/spool/asterisk/monitor/";
    this->LocalMonitorFilePath = mAppPath + "monitor/";
#ifndef WIN32
    this->MixerCmd = "sox";
#else
    this->MixerCmd = "C:/Program Files (x86)/sox-14-4-0/sox.exe";
#endif
#ifdef EEPBX
    this->MaxAllowedCalls = settings->value("common/maxallowedcalls", 50).toInt();
    this->SafetyInterval = 20 * 1000 * 5000;
    this->StorageCheckInterval = 10;
    this->StorageLimit = settings->value("common/storagelimit", 90).toInt();
#else
    this->SafetyInterval = 100 * 1000;
#endif

    this->RingGroupFixedDisplayLength = 10;

    this->Res_Image_Banner_Header = ":images/banner_head.png";
    this->Res_image_loading = ":images/loading";
    this->Res_Icon_Logo = ":images/logo.png";
    this->Res_Txt_Title = ":images/txt_title.png";
    this->Res_icon_emergency = ":images/icon_emergency.png";
    this->Res_icon_meete_control = ":images/icon_meetme_control.png";
    this->Res_icon_cdr = ":images/icon_cdr.png";
    this->Res_icon_settings = ":images/icon_settings.png";
    this->Res_icon_sms = ":images/icon_sms.png";
    this->Res_icon_sms_square = ":images/icon_sms_square";
    this->Res_icon_empty = ":images/icon_empty.png";
    this->Res_btn_makecall = ":images/btn_makecall.png";
    this->Res_btn_broadcast = ":images/btn_broadcast";
    this->Res_btn_current_calls = ":images/btn_current_calls";
    this->Res_btn_play_sound = ":images/btn_play_sound";
    this->Res_btn_pickup = ":images/btn_pickup_small";
    this->Res_btn_monitor = ":images/btn_monitor_small";
    this->Res_btn_override = ":images/btn_override_small";
    this->Res_btn_transfer = ":images/btn_transfer_small";
    this->Res_btn_hangup = ":images/btn_hangup_small";
    this->Res_icon_online = ":images/icon_online";
    this->Res_icon_offline = ":images/icon_offline";
    this->Res_icon_ring = ":images/icon_ring";
    this->Res_icon_ringing = ":images/icon_ringing";
    this->Res_icon_incall = ":images/icon_incall";
#ifdef LOCKTEL
    this->Res_image_splash = ":images/image_splash";
#else //OEM AND WHITE版本
#ifdef WHITE
//    this->Res_image_splash = ":images/image_splash_white";
    this->Res_image_splash = ":images/image_splash.png";
//    this->Res_image_splash = ":images/image_splash_zdtf.png";

#else
    this->Res_image_splash = ":images/image_splash_oem";
#endif
    //this->OEM_TITLE = "浙江华络通讯设备有限公司";
    //this->OEM_TITLE = "湖南汇亚矿山多媒体调度指挥系统";
    this->OEM_TITLE = "调度指挥系统";
    //this->OEM_TITLE = "云泰调度指挥系统";
#endif
    this->Res_btn_alert = ":images/btn_alert";
    Res_btn_record_query = ":images/btn_record_query";
    Res_btn_sound_record_list = ":images/btn_sound_record_list";
    Res_icon_checked = ":images/icon_checked";
    Res_icon_check_failed = ":images/icon_check_failed";
    Res_icon_unchecked = ":images/icon_unchecked";
    Res_icon_calendar = ":images/icon_calendar";
    Res_icon_report = ":images/icon_report";
    Res_icon_phone_book = ":images/icon_phone_book";
    Res_icon_phone_book_small = ":images/icon_phone_book_small";
    Res_icon_current_call = ":images/icon_current_call";
    Res_icon_make_call = ":images/icon_make_call";
    Res_icon_broadcast = ":images/icon_broadcast";
    Res_icon_play_record = ":images/icon_play_record";
    Res_icon_monitor = ":images/icon_monitor";
    Res_icon_hangup = ":images/icon_hangup";
    Res_icon_override = ":images/icon_override";
    Res_icon_transfer = ":images/icon_transfer";
    Res_icon_pickup = ":images/icon_pickup";
    Res_icon_rollcall = ":images/icon_rollcall";
    Res_icon_poll = ":images/icon_poll";
#ifdef LOCKTEL
    Res_image_banner = ":images/banner";
#endif
#ifdef OEM
    Res_image_banner = ":images/banner-oem";
#endif
#ifdef WHITE
//    Res_image_banner = ":images/banner-zdtf.png";
    Res_image_banner = ":images/banner_jzck.jpg";
//    Res_image_banner = ":images/banner.png";
#endif
#ifdef SZFY//神州飞扬
    Res_image_banner = ":images/banner-szfy";
#endif
#ifdef HBNYJT
    Res_image_banner = ":images/banner-white-hbnyjt";
#endif
#ifdef JXLC
    Res_image_banner =":images/banner_head_juxing";
#endif
    Res_icon_back = ":images/icon_back";
    Res_arrow_right = ":images/arrow_right";
    Res_icon_threeway = ":images/threeway";

    InvalidDate = QDate(1800, 1, 1);
    GarbageCleanInterval = 60;
    TelnetBufferBeginIdentifier = "BEGIN<<";
    TelnetBufferEndIdentifier = "EOF<<";


    mq_host = settings->value("mq/host", "127.0.0.1").toString();
    mq_eventBindingkey = "0.0.0.1";
    mq_eventExchange = "topic_logs";
    mq_rpcRoutingkey = "rpc_queue";
    mq_rpcExchange = "";
    mq_port = settings->value("mq/port", 5672).toInt();
    mq_user = settings->value("mq/username", "guest").toString();
    mq_passwd = settings->value("mq/password", "guest").toString();
#ifndef OEM
    Window_Title = "指挥调度系统: " + VERSION_NAME;
#else
    Window_Title = OEM_TITLE + VERSION_NAME;
#endif

    notifierUDPIP_ = settings->value("notifier/udpip", "127.0.0.1").toString();
    notifierUDPPort_ = settings->value("notifier/udpport", 8888).toInt();

    notifierTCPServerIP_ = settings->value("notifier/tcpip", "127.0.0.1").toString();
    notifierTCPServerPort_ = settings->value("notifier/tcpport", 8888).toInt();

    {
        settings->beginGroup("MeetmeRinggroups");
        for (auto key: settings->allKeys())
            meetmeRinggroups_.insert(key, settings->value(key).toString());
        settings->endGroup();
    }

    initStyleSheets();
    delete settings;
    mFtpClient = NULL;
    mCmdProcessor = new CmdProcessor();
    return mCmdProcessor->initialise();
}

void Config::initStyleSheets() {
     groupboxStylesheet = "QGroupBox { border : 2px solid gray; border-radius: 3px }";
}

void Config::ftpClientInit(QString username, QString password, QString host, int port, QObject *parent)
{
    if (mFtpClient != NULL)
        return;
    mFtpClient = new FTPClient(username, password, host, port, parent);
}

QString Config::getGroupcallNumber()
{
    this->CurrentGroupcallNumber++;
    if (this->CurrentGroupcallNumber > 999)
        this->CurrentGroupcallNumber = 100;
    QString number = QString("%1%2").arg(this->OperatorGroupcallBaseNumber).arg(this->CurrentGroupcallNumber);
    return number;
}

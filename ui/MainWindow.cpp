#include "MainWindow.h"
#include "LoginForm.h"
//#include "base/BaseWidget.h"
#include "mainwidget/MainWidget.h"
#include "settings/SettingsWidget.h"
#include "eventplan/EventPlanEditor.h"
#include "ui/phonebook/UiPhoneBook.h"
#include "ui/event/EventWidget.h"
#include "calllog/CallLogWidget.h"
#include "eventlog/EventLog.h"
#include "misc/logger.h"
#include "misc/Config.h"
#include "message_queue/RPCCommand.h"
#include "ui/meetme/MeetMeWidget.h"
#include "ui/timedbroadcast/TimedBroadcastEditor.h"
#include "VideoLinkage/videolinkagewidget.h"
#include "mainwidget/OperatorZone.h"
#include "mainwidget/ExtensionsZone.h"
#include "CustomWidget/titlebar.h"

static MainWindow* sMainWindow = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sMainWindow != NULL)
        sMainWindow->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void MainWindow::eventMessageReceived(QMap<QString, QString>& msg)
{
    if (msg.contains("system")) {
        emit onAdapterEventReceivedSignal(msg.value("system"), true); //收到来自系统的广播，提示重启客户端
        return;
    }
    if (msg.value("Event") == "MessageSentEvent" || msg.value("From") == "console")
        emit onHAEventReceivedSignal(msg.value("Content"));
    else if (msg.value("Event") == "AdapterEvent")
        emit onAdapterEventReceivedSignal(msg.value("Content"), (msg.value("NeedRestart") == "true") ? true : false);

    //
    Config* cfg = Config::Instance();
    QString emergensyNumber = cfg->EmergencyNumber;
    if (msg.value("Event") == "JoinEvent" && msg.value("CallerIdName") == emergensyNumber){
        QString strCallerIdName = msg.value("CallerIdName");
        QString strChannel = msg.value("Channel");
        QString strQueue = msg.value("Queue");

        if (strQueue != RPCCommand::getOperatorQueueNumber()){
            return ;
        }

        if (!strCallerIdName.startsWith(emergensyNumber)){
            return;
        }

        QMap<QString, PBX::Extension> operatorsMap = RPCCommand::getOperators();
        foreach (QString opNumber, operatorsMap.keys())
        {
            if(strChannel.contains("/"+opNumber)){
                return ;
            }
        }
        emit onEmergencyCallDlgSignal(strCallerIdName,
                                      msg.value("State"),
                                      strChannel,
                                      msg.value("Uniqueid"));
    }
}

void MainWindow::onHAEventReceivedSlot(QString msg)
{

    QStringList contents = msg.split(":");
    if (contents.length() < 3) {
        LOG(Logger::Debug, "received a HA switch msg but its malformed");
        return;
    }
    QString host = contents[0];
    QString action = "";
    if (contents[1] == "0")
        action = tr("从机启动");
    else
        action = tr("服务接管");
    QString reason;
    if ("0" == contents[2])
        reason = tr("心跳线松动");
    else
        reason = tr("正常切换");
    QMessageBox::about(this, tr("双机热备事件"), host + " " + action + "：" + reason);
}


void MainWindow::onAdapterEventReceivedSlot(QString msg, bool needRestart)
{
    if (!needRestart)
        QMessageBox::about(this, tr("epbx故障"), msg);
    else
        this->stopOnErrorSlot();
}

void MainWindow::onEmergencyCallDlgSlot(QString strDisplayName,QString strNumber, QString strChannel, QString strUniqueid)
{
    //m_dlgEmergencyCall.show();
    EmergencyCallInfo info;
    info.m_strDisplayName = strDisplayName;
    info.m_StrNumber = strNumber;
    info.m_StrChannel = strChannel;
    info.m_strUniqueid = strUniqueid;
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

    QString str = time.toString("hh:mm:ss"); //设置显示格式
    info.m_strTime = str;
    info.m_strUuid = Tools::getUuid().c_str();

    m_dlgEmergencyCall->AddEmergencyCallInfo(info);

}

MainWindow::MainWindow(QStringList args, QWidget *parent) :
    QMainWindow(parent)
{
    Config* cfg = Config::Instance();
    bool isRestart = false;
    {
        foreach (QString arg, args) {
            if (arg == RESTART_FLAG) {
                isRestart = true;
                break;
            }
        }
        if (!isRestart) {
            LoginForm* loginForm = new LoginForm(this);
            loginForm->activateWindow();
            loginForm->raise();
            if (loginForm->exec() == QDialog::Rejected)
                exit(2);
        }
    }


    m_dlgEmergencyCall = new DlgEmergencyCall(this);

    //初始化ftp连接
    {
        //因为需要登录以后从rpc读取pbx的ip地址，故此把ftp连接初始化放在这里
        QString host = RPCCommand::getIp();
        cfg->ftpClientInit(cfg->TelnetUsername, cfg->TelnetPassword, host, cfg->FtpPort, this);
    }

    //通知adapter刷新数据
    RPCCommand::clearCache();

    //初始化pbx
    PBX::Instance()->initialise();

    setMinimumSize(1270, 700);
#ifdef LOCKTEL
    setWindowTitle(cfg->VERSION_NAME);
#else
    setWindowTitle(cfg->OEM_TITLE + cfg->VERSION_NAME);
#endif
    QPoint pos(0, 0);
    move(pos);

    mCentralWidgetHolder = new QVBoxLayout;
    mCentralWidgetHolder->setMargin(0);
    mCentralWidgetHolder->setSpacing(0);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(mCentralWidgetHolder);
    setCentralWidget(centralWidget);

    initActivities();

    QMap<QString, QVariant> params;
    loadActivity(BaseWidget::eMainWidget, params);

    /************************ 视频联动 ************************/
    VideoLinkageWidget *v = new VideoLinkageWidget(this);
    MainWidget *mainWidget = dynamic_cast<MainWidget *>(getActivity(BaseWidget::eMainWidget));
    Q_ASSERT_X(mainWidget != NULL, "MainWindow::MainWindow()", "MainWidget create error!!");
    connect(mainWidget->getOperatorZone(), &OperatorZone::operatorExtenStateChagne,
            v, &VideoLinkageWidget::operatorExtenStateChanged);
    connect(mainWidget->getExtensionsZone(), &ExtensionsZone::extenStateChangedSignal,
            v, &VideoLinkageWidget::extenStateChanged);
    v->move(rect().topRight());
    v->show();
    /************************ 视频联动 ************************/

    sMainWindow = this;
    sCallbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
    connect(this, SIGNAL(onHAEventReceivedSignal(QString)), this, SLOT(onHAEventReceivedSlot(QString)));
    connect(this, SIGNAL(onAdapterEventReceivedSignal(QString,bool)), this, SLOT(onAdapterEventReceivedSlot(QString,bool)));
    connect(this, SIGNAL(onEmergencyCallDlgSignal(QString, QString, QString, QString)), this, SLOT(onEmergencyCallDlgSlot(QString, QString, QString, QString)));
    mStopOnError = false;
    connect(this, SIGNAL(stopOnErrorSignal()), this, SLOT(stopOnErrorSlot()));
    errorCheckTimerI = startTimer(10000);
#ifdef EEPBX
    storageCheckTimer = startTimer(30 * 60 * 1000);
#endif
}

void MainWindow::loadActivity(int targetActivityId, QMap<QString, QVariant> &params, int callerActivityId)
{
    params.insert("caller-activity", callerActivityId);
    /*
    foreach (int activity, mActivities.keys()) {
        BaseWidget* activityWidget = mActivities.value(activity);
        if (targetActivityId == activity) {
            if (!activityWidget->isVisible()) {
                activityWidget->onResume(params);
                activityWidget->setVisible(true);
            }
        } else {
            if (activityWidget->isVisible()) {
                activityWidget->onPause(params);
                activityWidget->setVisible(false);
            }
        }
    }
    */
    BaseWidget* widgetToHide = NULL;
    BaseWidget* widgetToShow = NULL;
    foreach (int activity, mActivities.keys()) {
        BaseWidget* activityWidget = mActivities.value(activity);
        if (targetActivityId == activity) {
            if (!activityWidget->isVisible()) {
                //                activityWidget->onResume(params);
                //                activityWidget->setVisible(true);
                widgetToShow = activityWidget;
            }
        } else {
            if (activityWidget->isVisible()) {
                //                activityWidget->onPause(params);
                //                activityWidget->setVisible(false);
                widgetToHide = activityWidget;
            }
        }
    }
    if (widgetToHide != NULL) {
        widgetToHide->onPause(params);
        widgetToHide->setVisible(false);
    }
    if (widgetToShow != NULL) {
        widgetToShow->onResume(params);
        widgetToShow->setVisible(true);
    }
}

BaseWidget *MainWindow::getActivity(int activityId)
{
    if (mActivities.contains(activityId))
        return mActivities.value(activityId);
    return NULL;
}


static bool errorChecking = false;
void MainWindow::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == errorCheckTimerI) {
        if (errorChecking)
            return;
        errorChecking = true;
        if (!MessageClient::Instance()->getHousekeeper()->check())
            emit stopOnErrorSignal();
        errorChecking = false;
    }
#ifdef EEPBX
    if (event->timerId() == storageCheckTimer)
        storageFullNotice();
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}

#ifdef EEPBX
void MainWindow::storageFullNotice()
{
    QMap<QString, int> storage = RPCCommand::getStorageState();
    int percentage = 0;
    if (storage.contains("/sdcard"))
        percentage = storage.value("/sdcard");
    else
        percentage = storage.value("/");
    if (percentage >= Config::Instance()->StorageLimit)
        QMessageBox::about(this, tr("警告"), tr("存储容量已接近上限，请删除部分通话记录以释放空间"));
}
#endif

void MainWindow::stopOnErrorSlot()
{
    LOG(Logger::Error, "MainWindow stopOnError\n");
    /*
    QMessageBox::StandardButton btn = QMessageBox::warning(NULL, tr("与pbx的通讯出现故障"), tr("请确认网络连接是否正常\n点击Yes重新连接，No退出"),
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    QApplication::quit();
    if (btn == QMessageBox::No)
        return;
        */
    QApplication::quit();
    QStringList args;
    args.append(RESTART_FLAG);
    QProcess::startDetached(QApplication::applicationFilePath(), args);
}

void MainWindow::initActivities()
{
    /* 首页 */
    BaseWidget* mainWidget = new MainWidget(this, this);
    mActivities.insert(BaseWidget::eMainWidget, mainWidget);
    /* 系统设置 */
    BaseWidget* settingsWidget = new SettingsWidget(this, this);
    mActivities.insert(BaseWidget::eSettings, settingsWidget);
    BaseWidget* eventPlanEditor = new EventPlanEditor(this, this);
    mActivities.insert(BaseWidget::eEventPlanEditor, eventPlanEditor);
    /* 通讯录 */
    BaseWidget* phoneBook = new UiPhoneBook(this, this);
    mActivities.insert(BaseWidget::ePhoneBook, phoneBook);
    /* 应急预案 */
    BaseWidget* emergencyWidget = new EventWidget(EventModel::eEmergency, this, this);
    mActivities.insert(BaseWidget::eEmergency, emergencyWidget);
    /* 事件处理 */
    BaseWidget* eventWidget = new EventWidget(EventModel::eNormal, this, this);
    mActivities.insert(BaseWidget::eEvent, eventWidget);
    /* 呼叫记录 */
    BaseWidget* cdrWidget = new CallLogWidget(this, this);
    mActivities.insert(BaseWidget::eCDR, cdrWidget);
    /* 事件记录 */
    BaseWidget* eventlogWidget = new EventLog(this, this);
    mActivities.insert(BaseWidget::eEventLog, eventlogWidget);
    /* 会议控制 */
    BaseWidget* meetMeWidget = new MeetMeWidget(this, this);
    mActivities.insert(BaseWidget::eMeetme, meetMeWidget);

    //zll
    BaseWidget* timedBroadcastEditor = new TimedBroadcastEditor(this, this);
    mActivities.insert(BaseWidget::eTimedBroadcastEditor, timedBroadcastEditor);

    foreach (BaseWidget* activity, mActivities.values()) {
        activity->setVisible(false);
        mCentralWidgetHolder->addWidget(activity);
    }


}

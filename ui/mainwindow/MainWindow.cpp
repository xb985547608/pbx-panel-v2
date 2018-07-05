#include "MainWindow.h"
#include "misc/Config.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "ui/LoginForm.h"
#include "ExtensionsZone.h"
#include "RinggroupsZone.h"
#include "QueueZone.h"
#include "SelectedExtensZone.h"
#include "FastFuncZone.h"

const static QString RESTART_FLAG = "dingo restart flag";
static MainWindow* sMainWindow = NULL;

void MainWindow::timerEvent(QTimerEvent *)
{
    if (mStopOnError)
        emit stopOnErrorSignal();
}

void MainWindow::stopOnErrorSlot()
{
    LOG(Logger::Error, "MainWindow stopOnError\n");
    QMessageBox::StandardButton btn = QMessageBox::warning(NULL, tr("与pbx的通讯出现故障"), tr("请确认网络连接是否正常\n点击Yes重新连接，No退出"),
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    QApplication::quit();
    if (btn == QMessageBox::No)
        return;
    QStringList args;
    args.append(RESTART_FLAG);
    QProcess::startDetached(QApplication::applicationFilePath(), args);
}

MainWindow::MainWindow(QStringList args, QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(1280, 768);
    sMainWindow = this;
    mStopOnError = false;
    mOperatorZoneHolder = NULL;
    mOperatorsZone = NULL;
    mRinggroupsZone = NULL;
    mQueueZone = NULL;
    mExtensionsZone = NULL;
    mSelectedExtensZone = NULL;
    mFastFuncZone = NULL;

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
    startTimer(1000);


    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* rootBox = new QVBoxLayout(mainWidget);
    rootBox->setSpacing(0);
    rootBox->setMargin(0);
    rootBox->addWidget(getTopFrame(), 1);
    QHBoxLayout* hbox = new QHBoxLayout;
    rootBox->addLayout(hbox, 6);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 4);

    mainWidget->setLayout(rootBox);
    setCentralWidget(mainWidget);

    connect(this, SIGNAL(stopOnErrorSignal()), this, SLOT(stopOnErrorSlot()));
}

MainWindow::~MainWindow()
{
    
}

QWidget* MainWindow::getTopFrame()
{
    Config* cfg = Config::Instance();
    QWidget* topFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout(topFrame);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getHeader(this));
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);

    QPushButton* btnEmergency = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                btnEmergency,
                cfg->Res_icon_emergency,
                tr("应急预案"),
                "{ background-color : rgb(230,138,112); font: 75 12pt \"黑体\"; color : black;}"
                );
    //connect(mBtnEmergency, SIGNAL(clicked()), this, SLOT(onBtnEmergencyClicked()));
    connect(btnEmergency, SIGNAL(clicked()), this, SLOT(onBtnEmergencyClicked()));
    hbox->addWidget(btnEmergency);

    QPushButton* btnEventProcess = new QPushButton(topFrame);
    MiscComponents::setPushButton(btnEventProcess, cfg->Res_icon_calendar, tr("事件处理"),
                                  "{ background-color : rgb(234,160,115); font: 75 12pt \"黑体\"; color : black;}");
    connect(btnEventProcess, SIGNAL(clicked()), this, SLOT(onBtnEventProcessClicked()));
    hbox->addWidget(btnEventProcess);

    QPushButton* mBtnMeetingRoomCtrl = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                mBtnMeetingRoomCtrl,
                cfg->Res_icon_meete_control,
                tr("会议控制"),
                "{ background-color : rgb(178,207,152); font: 75 12pt \"黑体\"; color : black;}"
                );
    //connect(mBtnMeetingRoomCtrl, SIGNAL(clicked()), this, SLOT(onBtnMeetmeCtrlClicked()));
    hbox->addWidget(mBtnMeetingRoomCtrl);

    QPushButton* btnCdr = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                btnCdr,
                cfg->Res_icon_cdr,
                tr("呼叫记录"),
                "{ background-color : rgb(231,226,160); font: 75 12pt \"黑体\"; color : black;}");
    //connect(btnCdr, SIGNAL(clicked()), this, SLOT(onBtnCdrClicked()));
    hbox->addWidget(btnCdr);

    QPushButton* btnEventLog = new QPushButton(topFrame);
    MiscComponents::setPushButton(btnEventLog, cfg->Res_icon_report, tr("事件记录"),
                                  "{ background-color : rgb(214,125,241); font: 75 12pt \"黑体\"; color : black;}");
    //connect(btnEventLog, SIGNAL(clicked()), this, SLOT(onBtnEventLogClicked()));
    hbox->addWidget(btnEventLog);

    //通讯录按钮
    QPushButton* mBtnPhoneBook = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                mBtnPhoneBook,
                cfg->Res_icon_phone_book_small,
                tr("通讯录"),
                "{ background-color : rgb(172,211,217); font: 75 12pt \"黑体\"; color : black;}");
    //TODO: connect signal
    hbox->addWidget(mBtnPhoneBook);

    QPushButton* mBtnSettings = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                mBtnSettings,
                cfg->Res_icon_settings,
                tr("系统设置"),
                "{ background-color : rgb(172,211,217); font: 75 12pt \"黑体\"; color : black;}");
    //connect(mBtnSettings, SIGNAL(clicked()), this, SLOT(onBtnSettingsClicked()));
    hbox->addWidget(mBtnSettings);

    rootBox->addLayout(hbox);
    topFrame->setLayout(rootBox);
    return topFrame;
}

QWidget* MainWindow::getLeftFrame()
{
    QWidget* leftFrame = new QWidget(this);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    leftFrame->setLayout(rootBox);

    refreshOperatorsZone();
    rootBox->addLayout(mOperatorZoneHolder, 1);

    mRinggroupsZone = new RinggroupsZone(this);
    rootBox->addWidget(mRinggroupsZone, 6);

    mQueueZone = new QueueZone(this);
    rootBox->addWidget(mQueueZone, 3);

    return leftFrame;
}

QWidget* MainWindow::getRightFrame()
{
    QWidget* rightFrame = new QWidget(this);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rightFrame->setLayout(rootBox);

    mExtensionsZone = new ExtensionsZone(this, this);
    mFastFuncZone = new FastFuncZone(this, this);
    mSelectedExtensZone = new SelectedExtensZone(this, this);

    rootBox->addWidget(mExtensionsZone, 8);
    rootBox->addWidget(mSelectedExtensZone, 1);
    rootBox->addWidget(mFastFuncZone, 1);

    return rightFrame;
}

void MainWindow::onBtnEmergencyClicked()
{
    RPCCommand::p2pCall("SIP/8015", "8015", "8015", "9999");
}

void MainWindow::onBtnEventProcessClicked()
{
    refreshOperatorsZone();
}

void MainWindow::refreshOperatorsZone()
{
    if (mOperatorZoneHolder == NULL)
        mOperatorZoneHolder = new QVBoxLayout;
    if (mOperatorsZone != NULL)
        delete mOperatorsZone;
    mOperatorsZone = new OperatorZone(this);
    mOperatorZoneHolder->addWidget(mOperatorsZone);
}

ExtensionsZone* MainWindow::getExtensionsZone()
{
    return mExtensionsZone;
}

SelectedExtensZone* MainWindow::getSelectedExtensZone()
{
    return mSelectedExtensZone;
}

FastFuncZone* MainWindow::getFastFuncZone()
{
    return mFastFuncZone;
}

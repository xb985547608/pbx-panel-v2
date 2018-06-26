#include "MainWidget.h"
#include "ui/MainWindow.h"
#include "ui/base/misccomponents.h"
#include "ui/base/BaseWidget.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/Config.h"
#include "ExtensionsZone.h"
#include "RinggroupsZone.h"
#include "QueueZone.h"
#include "SelectedExtensZone.h"
#include "FastFuncZone.h"
#include "OperatorZone.h"
#include "ui/event/EventChooser.h"
#include "ui/meetme/MeetMeWidget.h"

static MainWidget* sMainWidget = NULL;

MainWidget::MainWidget(MainWindow* mw, QWidget *parent) :
    BaseWidget(tr(""), Config::Instance()->Res_icon_settings, QColor(172,211,217),
               BaseWidget::eSettings, BaseWidget::eNone, mw, parent)
{
    mOperatorZoneHolder = NULL;
    mOperatorsZone = NULL;
    mRinggroupsZone = NULL;
    mQueueZone = NULL;
    mExtensionsZone = NULL;
    mSelectedExtensZone = NULL;
    mFastFuncZone = NULL;
    initFrames();
}

MainWidget::MainWidget(
        QString title,
        QString titleImage,
        QColor titleBgColor,
        Activity activityBack,
        MainWindow* mainWindow,
        QWidget *parent) :
    BaseWidget(title, titleImage, titleBgColor, BaseWidget::ePhoneBook, activityBack, mainWindow, parent)
{
    setMinimumSize(1280, 768);
    sMainWidget = this;
    mOperatorZoneHolder = NULL;
    mOperatorsZone = NULL;
    mRinggroupsZone = NULL;
    mQueueZone = NULL;
    mExtensionsZone = NULL;
    mSelectedExtensZone = NULL;
    mFastFuncZone = NULL;

    QVBoxLayout* rootBox = new QVBoxLayout(this);
    rootBox->setSpacing(0);
    rootBox->setMargin(0);
    rootBox->addWidget(getTopFrame(), 1);
    QHBoxLayout* hbox = new QHBoxLayout;
    rootBox->addLayout(hbox, 6);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 3);

    setLayout(rootBox);
}

void MainWidget::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    mFrameRootBox->addLayout(hbox);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 4);
}

MainWidget::~MainWidget()
{
    LOG(Logger::Debug, "MainWidget deleting\n");
}

QWidget* MainWidget::getTopFrame()
{
    Config* cfg = Config::Instance();
    QWidget* topFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout(topFrame);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    mHeaderWidget = MiscComponents::getHeader(this);
    rootBox->addWidget(mHeaderWidget);
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
    connect(mBtnMeetingRoomCtrl, SIGNAL(clicked()), this, SLOT(onBtnMeetmeCtrlClicked()));
    hbox->addWidget(mBtnMeetingRoomCtrl);

    QPushButton* btnCdr = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                btnCdr,
                cfg->Res_icon_cdr,
                tr("呼叫记录"),
                "{ background-color : rgb(231,226,160); font: 75 12pt \"黑体\"; color : black;}");
    connect(btnCdr, SIGNAL(clicked()), this, SLOT(onBtnCdrClicked()));
    hbox->addWidget(btnCdr);

    QPushButton* btnEventLog = new QPushButton(topFrame);
    MiscComponents::setPushButton(btnEventLog, cfg->Res_icon_report, tr("事件记录"),
                                  "{ background-color : rgb(214,125,241); font: 75 12pt \"黑体\"; color : black;}");
    connect(btnEventLog, SIGNAL(clicked()), this, SLOT(onBtnEventLogClicked()));
    hbox->addWidget(btnEventLog);

    //通讯录按钮
    QPushButton* mBtnPhoneBook = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                mBtnPhoneBook,
                cfg->Res_icon_phone_book_small,
                tr("通讯录"),
                "{ background-color : rgb(172,211,217); font: 75 12pt \"黑体\"; color : black;}");
    connect(mBtnPhoneBook, SIGNAL(clicked()), this, SLOT(onBtnPhonebookClicked()));
    hbox->addWidget(mBtnPhoneBook);

    QPushButton* mBtnSettings = new QPushButton(topFrame);
    MiscComponents::setPushButton(
                mBtnSettings,
                cfg->Res_icon_settings,
                tr("系统设置"),
                "{ background-color : rgb(172,211,217); font: 75 12pt \"黑体\"; color : black;}");
    connect(mBtnSettings, SIGNAL(clicked()), this, SLOT(onBtnSettingsClicked()));
    hbox->addWidget(mBtnSettings);

    rootBox->addLayout(hbox);
    topFrame->setLayout(rootBox);
    return topFrame;
}

QWidget* MainWidget::getLeftFrame()
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

QWidget* MainWidget::getRightFrame()
{
    QWidget* rightFrame = new QWidget(this);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rightFrame->setLayout(rootBox);

    mFastFuncZone = new FastFuncZone(this, this);
    mSelectedExtensZone = new SelectedExtensZone(this, this);
    mExtensionsZone = new ExtensionsZone(this, this);

    rootBox->addWidget(mExtensionsZone, 8);
    rootBox->addWidget(mSelectedExtensZone, 1);
    rootBox->addWidget(mFastFuncZone, 1);
    return rightFrame;
}

void MainWidget::onBtnEmergencyClicked()
{
    QMap<QString, QVariant> params;
    mMainWindow->loadActivity(BaseWidget::eEmergency, params);
}

void MainWidget::onBtnCdrClicked()
{
    QMap<QString, QVariant> params;
    mMainWindow->loadActivity(BaseWidget::eCDR, params);
}

void MainWidget::onBtnEventProcessClicked()
{
    EventChooser* chooser = new EventChooser(this);
    if (chooser->exec() == QDialog::Accepted) {
        QMap<QString, QVariant> params;
        QString which = chooser->whichSelected();
        QString id = chooser->selectedId();
        if (which != "" && id != "") {
            LOG(Logger::Debug, "onBtnEventProcessClicked: %s selected and id = %s\n", which.toStdString().c_str(), id.toStdString().c_str());
            if (which == "plan")
                params.insert("planid", id);
            else if (which == "event") {
                QList<EventModel::Event> events = RPCCommand::getEvent(id);
                if (events.size() > 0) {
                    EventModel::Event event = events.at(0);
                    params.insert("planid", event.planId);
                    params.insert("eventid", id);
                }
            }
        }
        mMainWindow->loadActivity(BaseWidget::eEvent, params);
    }
    delete chooser;
}

void MainWidget::onBtnEventLogClicked()
{
    QMap<QString, QVariant> params;
    mMainWindow->loadActivity(BaseWidget::eEventLog, params);
}

void MainWidget::onBtnMeetmeCtrlClicked()
{
    QMap<QString, QVariant> params;
    mMainWindow->loadActivity(BaseWidget::eMeetme, params);
}

void MainWidget::refreshOperatorsZone()
{
    if (mOperatorZoneHolder == NULL)
        mOperatorZoneHolder = new QVBoxLayout;
    if (mOperatorsZone != NULL)
        delete mOperatorsZone;
    mOperatorsZone = new OperatorZone(this, this);
    mOperatorZoneHolder->addWidget(mOperatorsZone);
}

void MainWidget::onBtnSettingsClicked()
{
    QMap<QString, QVariant> params;
    if (mMainWindow != NULL)
        mMainWindow->loadActivity(BaseWidget::eSettings, params);
}

void MainWidget::onBtnPhonebookClicked()
{
    ShowPhonebook();
}
void MainWidget::ShowPhonebook(bool isEmergency)
{
    QMap<QString, QVariant>params;
    params.insert("ringgroup-number", getRinggroupsZone()->getCurrentSelectedGrpNumber());
    params.insert("ringgroup-name", getRinggroupsZone()->getCurrentSelectedGrpName());
    if (isEmergency)
        params.insert("method", "emergency-call");
    else
        params.insert("method", "normal-call");
    if (mMainWindow != NULL)
        mMainWindow->loadActivity(BaseWidget::ePhoneBook, params);
}

ExtensionsZone* MainWidget::getExtensionsZone()
{
    return mExtensionsZone;
}

SelectedExtensZone* MainWidget::getSelectedExtensZone()
{
    return mSelectedExtensZone;
}

FastFuncZone* MainWidget::getFastFuncZone()
{
    return mFastFuncZone;
}

RinggroupsZone* MainWidget::getRinggroupsZone()
{
    return mRinggroupsZone;
}

void MainWidget::onResume(QMap<QString, QVariant>& params)
{
    if (params.contains("caller-activity")) {
        int activityId = params.value("caller-activity").toInt();
        if (activityId == BaseWidget::eSettings)
            refreshOperatorsZone();
    }
    BaseWidget::onResume(params);
}

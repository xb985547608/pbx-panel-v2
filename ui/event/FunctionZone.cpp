#include "FunctionZone.h"
#include "ui/base/misccomponents.h"
#include "misc/Config.h"
#include "DescriptionZone.h"
#include "unistd.h"

FunctionZone::FunctionZone(EventWidget* eventWidget, QWidget *parent) :
    QWidget(parent),
    mEventWidget(eventWidget)
{
    mContentHolder = new TableWidget(1, this);
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(mContentHolder);
    setLayout(rootBox);
}

FunctionZoneEmergency::FunctionZoneEmergency(EventWidget *ew, QWidget *parent)
    : FunctionZone(ew, parent)
{
    initFunctions();
}

void FunctionZoneEmergency::initFunctions()
{
    mContentHolder->setHeader(MiscComponents::getSectionLabel(tr("应急报警"), mContentHolder));
    mContentHolder->setFooter(MiscComponents::getSectionLabel(tr(""), mContentHolder));
    QToolButton* btnAlert = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnAlert, Config::Instance()->Res_btn_alert, tr("报警"), QSize(128, 128), Qt::ToolButtonTextUnderIcon, "{ background-color: yellow; }");
    mContentHolder->appendItem(btnAlert);
    connect(btnAlert, SIGNAL(clicked()), this, SLOT(onBtnAlertClickedSlot()));
}

void FunctionZoneEmergency::onBtnAlertClickedSlot()
{
    if (!mEventWidget->saveEvent())
        return;
    mEventWidget->hangupAll();
    //mEventWidget->broadcast();
    //mEventWidget->makeCall(true);
    mEventWidget->sendSms();
    usleep(500 * 1000);
    mEventWidget->makeCall();
    mEventWidget->playSound();
}

FunctionZoneNormal::FunctionZoneNormal(EventWidget* ew, QWidget *parent)
    : FunctionZone(ew, parent)
{
    initFunctions();
}

void FunctionZoneNormal::initFunctions()
{
    Config* cfg = Config::Instance();
    QToolButton* btnCall = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnCall, cfg->Res_btn_alert, tr("呼叫"), QSize(32, 32));
    connect(btnCall, SIGNAL(clicked()), mEventWidget, SLOT(makeCall()));
    /*
    QToolButton* btnListRecords = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnListRecords, cfg->Res_btn_sound_record_list, tr("录音列表"), QSize(32, 32));
    */
    QToolButton* btnBroadcast = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnBroadcast, cfg->Res_icon_broadcast, tr("广播"), QSize(32, 32));
    connect(btnBroadcast, SIGNAL(clicked()), mEventWidget, SLOT(broadcast()));
    QToolButton* btnPlaySnd = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnPlaySnd, cfg->Res_icon_play_record, tr("录音播放"), QSize(32, 32));
    connect(btnPlaySnd, SIGNAL(clicked()), mEventWidget, SLOT(playSound()));
    QToolButton* btnSendSms = new QToolButton(mContentHolder);
    MiscComponents::setToolButton(btnSendSms, cfg->Res_icon_sms_square, tr("发送短信"), QSize(32, 32));
    connect(btnSendSms, SIGNAL(clicked()), mEventWidget, SLOT(sendSms()));
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(btnCall);
    //hbox->addWidget(btnListRecords);
    hbox->addWidget(btnBroadcast);
    hbox->addWidget(btnPlaySnd);
    hbox->addWidget(btnSendSms);
    QWidget* funcWidget = new QWidget(mContentHolder);
    funcWidget->setLayout(hbox);
    mContentHolder->setHeader(MiscComponents::getSectionLabel(tr("快速功能键"), mContentHolder));
    mContentHolder->setFooter(MiscComponents::getSectionLabel(tr(""), mContentHolder));
    mContentHolder->appendItem(funcWidget);
}

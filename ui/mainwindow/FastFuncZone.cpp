#include "FastFuncZone.h"
#include "misc/Config.h"
#include "ui/base/misccomponents.h"

FastFuncZone::FastFuncZone(MainWindow* mainWindow, QWidget *parent) :
    QWidget(parent), mMainwindow(mainWindow)
{
    Config* cfg = Config::Instance();

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setSpacing(0);
    hbox->setMargin(0);
    QGroupBox* groupbox = MiscComponents::getGroupBox(hbox, parent);


    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setSpacing(0);
    rootBox->setMargin(0);
    rootBox->addWidget(groupbox);
    setLayout(rootBox);

    //呼叫按钮
    {
        mBtnGroupCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnGroupCall, cfg->Res_icon_make_call, tr("呼叫"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnGroupCall, SIGNAL(clicked()), this, SLOT(onBtnGroupCallClicked()));
        hbox->addWidget(mBtnGroupCall);
    }
    //短信按钮
    {
        mBtnSms = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnSms, cfg->Res_icon_sms_square, tr("短信"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnSms, SIGNAL(clicked()), this, SLOT(onBtnSmsClicked()));
        hbox->addWidget(mBtnSms);
    }
    //"当前通话"按钮和checkbox
    {
        mBtnCurrentCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnCurrentCall, cfg->Res_icon_current_call, tr("当前通话"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnCurrentCall, SIGNAL(clicked()), this, SLOT(onBtnCurrentCallClicked()));
        hbox->addWidget(mBtnCurrentCall);
    }
    /*
    //通讯录按钮
    {
        mBtnPhoneBook = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPhoneBook, cfg->Res_icon_phone_book, tr("通讯录"), Qt::ToolButtonTextUnderIcon);
        //TODO: connect signal
        hbox->addWidget(mBtnPhoneBook);
    }
    */
    //广播
    {
        mBtnBroadcast = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnBroadcast, cfg->Res_icon_broadcast, tr("广播"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnBroadcast, SIGNAL(clicked()), this, SLOT(onBtnBroadcastClicked()));
        hbox->addWidget(mBtnBroadcast);
    }
    //扩音
    {
        mBtnPaging = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPaging, cfg->Res_icon_play_record, tr("录音播放"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnPaging, SIGNAL(clicked()), this, SLOT(onBtnPagingClicked()));
        hbox->addWidget(mBtnPaging);
    }
    //监听
    {
        mBtnMonitor = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnMonitor, cfg->Res_icon_monitor, tr("监听"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnMonitor, SIGNAL(clicked()), this, SLOT(onBtnMonitorClicked()));
        hbox->addWidget(mBtnMonitor);
    }
    //强拆
    {
        mBtnHangup = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnHangup, cfg->Res_icon_hangup, tr("强拆"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnHangup, SIGNAL(clicked()), this, SLOT(onBtnHangupClicked()));
        hbox->addWidget(mBtnHangup);
    }
    //强插
    {
        mBtnOverride = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnOverride, cfg->Res_icon_override, tr("强插"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnOverride, SIGNAL(clicked()), this, SLOT(onBtnOverrideClicked()));
        hbox->addWidget(mBtnOverride);
    }
    //转接
    {
        mBtnTransfer = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnTransfer, cfg->Res_icon_transfer, tr("转接"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnTransfer, SIGNAL(clicked()), this, SLOT(onBtnTransferClicked()));
        hbox->addWidget(mBtnTransfer);
    }
    //代接
    {
        mBtnPickup = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPickup, cfg->Res_icon_pickup, tr("代接"), Qt::ToolButtonTextUnderIcon);
        connect(mBtnPickup, SIGNAL(clicked()), this, SLOT(onBtnPickupClicked()));
        hbox->addWidget(mBtnPickup);
    }
    //点名
    {
        mBtnRollCall = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnRollCall, cfg->Res_icon_rollcall, tr("点名"), Qt::ToolButtonTextUnderIcon);
        //TODO: connect signal
        hbox->addWidget(mBtnRollCall);
    }
    //轮询
    {
        mBtnPoll = new QToolButton(groupbox);
        MiscComponents::setToolButton(mBtnPoll, cfg->Res_icon_poll, tr("轮询"), Qt::ToolButtonTextUnderIcon);
        //todo: CONNECT signal
        hbox->addWidget(mBtnPoll);
    }
}

void FastFuncZone::selectedExtenCntChanged(int count)
{
    switch (count) {
    case 0:
        //mBtnSms
        //mBtnPhoneBook
        //mBtnCurrentCal
        mBtnGroupCall->setEnabled(false);
        mBtnBroadcast->setEnabled(false);
        mBtnPaging->setEnabled(false);
        mBtnMonitor->setEnabled(false);
        mBtnHangup->setEnabled(false);
        mBtnOverride->setEnabled(false);
        mBtnTransfer->setEnabled(false);
        mBtnPickup->setEnabled(false);
        mBtnRollCall->setEnabled(false);
        mBtnPoll->setEnabled(false);
        break;
    case 1:
        //mBtnSms
        //mBtnPhoneBook
        //mBtnCurrentCal
        mBtnGroupCall->setEnabled(true);
        mBtnBroadcast->setEnabled(true);
        mBtnPaging->setEnabled(true);
        mBtnMonitor->setEnabled(true);
        mBtnHangup->setEnabled(true);
        mBtnOverride->setEnabled(true);
        mBtnTransfer->setEnabled(true);
        mBtnPickup->setEnabled(true);
        mBtnRollCall->setEnabled(false);
        mBtnPoll->setEnabled(false);
        break;
    default:
        //mBtnSms
        //mBtnPhoneBook
        //mBtnCurrentCal
        mBtnGroupCall->setEnabled(true);
        mBtnBroadcast->setEnabled(true);
        mBtnPaging->setEnabled(true);
        mBtnMonitor->setEnabled(false);
        mBtnHangup->setEnabled(true);
        mBtnOverride->setEnabled(false);
        mBtnTransfer->setEnabled(false);
        mBtnPickup->setEnabled(false);
        mBtnRollCall->setEnabled(false);
        mBtnPoll->setEnabled(false);
        break;
    }
}

void FastFuncZone::onBtnSmsClicked()
{
}

/*
void FastFuncZone::onBtnPhoneBookClicked()
{
}
*/

void FastFuncZone::onBtnCurrentCallClicked()
{
}

void FastFuncZone::onBtnGroupCallClicked()
{
}

void FastFuncZone::onBtnBroadcastClicked()
{
}

void FastFuncZone::onBtnPagingClicked()
{
}

void FastFuncZone::onBtnMonitorClicked()
{
}

void FastFuncZone::onBtnHangupClicked()
{
}

void FastFuncZone::onBtnOverrideClicked()
{
}

void FastFuncZone::onBtnTransferClicked()
{
}

void FastFuncZone::onBtnPickupClicked()
{
}

/*
void FastFuncZone::onBtnRollCallClicked();
void FastFuncZone::onBtnPollClicked();
*/

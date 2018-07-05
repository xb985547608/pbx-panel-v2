#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "UiPhoneBook.h"
#include "misc/Config.h"
#include "ui/base/BaseWidget.h"
#include "UiPhoneBookList.h"
#include "message_queue/RPCCommand.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"

UiPhoneBook::UiPhoneBook(
        QString title,
        QString titleImage,
        QColor titleBgColor,
        Activity activityBack,
        MainWindow* mainWindow,
        QWidget *parent) :
    BaseWidget(title, titleImage, titleBgColor, BaseWidget::ePhoneBook, activityBack, mainWindow, parent)
{
    initFrames();
}

UiPhoneBook::UiPhoneBook(
        MainWindow* mainWindow,
        QWidget *parent) :
    BaseWidget(tr("通讯录"),
               Config::Instance()->Res_icon_phone_book_small,
               QColor(172,211,217),
               BaseWidget::ePhoneBook,
               BaseWidget::eMainWidget, mainWindow, parent)
{
    initFrames();
}

void UiPhoneBook::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    mUiPhoneBookList = new UiPhoneBookList(this);
    mUiPhoneBookList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(1);
    hbox->setSpacing(20);
    mBtnSave = new QPushButton(tr("保存修改"), this);
    connect(mBtnSave, SIGNAL(clicked()), this, SLOT(save()));
    mBtnCall = new QPushButton(tr("呼叫"), this);
    connect(mBtnCall, SIGNAL(clicked()), this, SLOT(call()));

    hbox->addWidget(mBtnSave);
    hbox->addWidget(mBtnCall);
    mFrameRootBox->addLayout(hbox);
    mFrameRootBox->addWidget(mUiPhoneBookList);
}

void UiPhoneBook::onResume(QMap<QString, QVariant>& params)
{
    mRinggroupNumber = "";
    mRinggroupName = "";
    if (params.contains("ok-btn-name")) {
        mBtnCall->setText(params.value("ok-btn-name").toString());
    } else
        mBtnCall->setText(tr("呼叫"));
    if (params.contains("ringgroup-number"))
        mRinggroupNumber = params.value("ringgroup-number").toString();
    if (params.contains("ringgroup-name"))
        mRinggroupName = params.value("ringgroup-name").toString();
    int mode = UiPhoneBookList::eEdit;
    if (params.contains("list-mode"))
        mode = params.value("list-mode").toInt();
    if (params.contains("method")){
        if (params.value("method").toString()=="emergency-call"){
            mBtnCall->setText(tr("紧急呼叫"));
        }
    }
    /*
    QString roomid = "";
    if (params.contains("roomid"))
        roomid = params.value("roomid").toString();
        */
    PhonebookUserData* userdata = new PhonebookUserData(params);
    mBtnCall->setUserData(0, userdata);
    mUiPhoneBookList->loadData(mRinggroupNumber, mode);
    switch (mode) {
    case UiPhoneBookList::ePickup :
        mBtnSave->setVisible(false);
        mBtnCall->setVisible(false);
        break;
    case UiPhoneBookList::eSelect :
    case UiPhoneBookList::eSelectSingle :
        mBtnSave->setVisible(false);
        mBtnCall->setVisible(true);
        break;
    default:
        mBtnSave->setVisible(true);
        mBtnCall->setVisible(true);
    }
    BaseWidget::onResume(params);
}

void UiPhoneBook::save()
{
    mUiPhoneBookList->save();
    mUiPhoneBookList->loadData(mRinggroupNumber);
}

void UiPhoneBook::call()
{
    PhonebookUserData* phonebookUserData = (PhonebookUserData*)mBtnCall->userData(0);
    QMap<QString, QVariant> data = phonebookUserData->getData();
    if (data.contains("roomid")) { ////切入窗口时，如果需要在本窗口发起呼叫，roomid为带入的会议房间号
        QString roomid = data.value("roomid").toString();
        mUiPhoneBookList->call(roomid);
        return;
    }
    if (!data.contains("method"))
        return;
    QString method = data.value("method").toString();
    if (method == "redirect-call") {
        if (!data.contains("channel") || !data.contains("context")) {
            LOG(Logger::Debug, "redirect-call not contains channel or context? failed");
            return;
        }
        QString channel = data.value("channel").toString();
        QString context = data.value("context").toString();
        QString extraChan = "";
        if (data.contains("extra-channel"))
            extraChan = data.value("extra-channel").toString();
        mUiPhoneBookList->redirectCall(channel, context, extraChan);
        return;
    }
    if (method == "3way-call") {
        QString channel = data.value("channel").toString();
        QString context = data.value("context").toString();
        QString extraChan = data.value("extra-channel").toString();
        mUiPhoneBookList->threeWayCall(channel, extraChan, context);
    }

    if (method == "emergency-call"){
        mUiPhoneBookList->emergencyCall();
    }
    if (method == "normall-call");
}


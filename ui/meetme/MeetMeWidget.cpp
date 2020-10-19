#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "MeetMeWidget.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include "misc/logger.h"
#include "misc/Config.h"
#include "message_queue/MessageClient.h"
#include "ui/base/misccomponents.h"
#include "ui/dialogs/DialPad.h"
#include "ui/MainWindow.h"
#include "ui/phonebook/UiPhoneBookList.h"
#include <QSoundEffect>
#include <QMovie>
#include <QBoxLayout>

static MeetMeWidget* sMeetMeWidget = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sMeetMeWidget != NULL)
        sMeetMeWidget->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void MeetMeWidget::eventMessageReceived(QMap<QString, QString>& msg)
{
    if (!msg.contains("Event"))
        return;
    QString event = msg.value("Event");
    if (event != "MeetMeJoin" && event != "MeetMeLeave" && event != "MeetMeMute")
        return;

    if (event == "MeetMeJoin" || event == "MeetMeLeave") {

        /// ================================
        Config* cfg = Config::Instance();
        QMap<QString, QString> meetmeRinggroups = cfg->meetmeRinggroups_;
        QString roomNumber = msg.value("Meetme");

        qDebug() << __FUNCTION__ << cfg->meetmeRinggroups_;

        if (meetmeRinggroups.contains(roomNumber) && event == "MeetMeJoin") {
            auto rooms = RPCCommand::getMeetmeInfo(roomNumber);
            QMap<QString, PBX::MeetmeMember> roomInfo;
            if (rooms.size() == 1)
                roomInfo = *rooms.begin();

            if (roomInfo.size() == 1 && roomInfo.begin()->userNum == "1") {
                /// 呼叫会议室对应的振铃组成员
                QString ringgroupNumber = meetmeRinggroups.value(roomNumber);
                QMap<QString, PBX::RingGroup> ringgroups = RPCCommand::getRinggroups(ringgroupNumber);
                if (ringgroups.contains(ringgroupNumber)) {
                    QStringList extens = ringgroups[ringgroupNumber].extenNumbers;
                    qDebug() << __FUNCTION__ << "alert call:";
                    for (auto exten: extens) {
                        RPCCommand::makeConferenceCall("SIP/"+exten, cfg->OperatorGroupCallContext,
                                                       exten, roomNumber, roomNumber, false);

                        qDebug() <<"\t" << exten;
                    }

                    QMetaObject::invokeMethod(this, "showAlertInfo", Qt::QueuedConnection, Q_ARG(QString, roomInfo.begin()->callerid));
                }
            } else {
                /// 静音非首位成员
                for (auto member: roomInfo) {
                    if (member.userNum != "1" && !member.isMuted)
                        RPCCommand::meetmeMute(roomNumber, member.userNum, true);
                }
            }
        }
        /// ===============================!

        emit roomStatusChangeSignal(msg.value("Meetme"));
        return;
    }
    if (event == "MeetMeMute") {
        QString roomid = msg.value("Meetme");
        QString userNum = msg.value("Usernum");
        bool isMuted = false;
        if (msg.value("Status") == "true")
            isMuted = true;
        if (roomid != mCurrentRoom)
            return;
        if (!mMemberBtns.contains(userNum))
            return;
        mMemberBtns.value(userNum)->muteStatusChanged(isMuted);
    }
}

void MeetMeWidget::roomStatusChangeSlot(QString roomid)
{
    QStringList rooms = RPCCommand::getMeetmeRooms();
    reloadRoomWidget(rooms);
    if (mCurrentRoom == roomid)
        reloadMembersWidget(roomid); //TODO: 这样比较简单粗暴，有需要可以改为精细一点的控制，直接处理join/leave消息，对membersWidget做增减
}

MeetMeWidget::MeetMeWidget(MainWindow* mw, QWidget *parent) :
    BaseWidget(
        tr("会议控制"),
        Config::Instance()->Res_icon_meete_control,
        QColor(178,207,152),
        eMeetme,
        eMainWidget,
        mw,
        parent)
{
    sMeetMeWidget = this;
    initFrames();
    connect(this, SIGNAL(roomStatusChangeSignal(QString)), this, SLOT(roomStatusChangeSlot(QString)));
    sCallbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
}

MeetMeWidget::~MeetMeWidget()
{
    if (sCallbackUuid != "")
        MessageClient::Instance()->getEventMonitor()->removeCallback(sCallbackUuid);
}

void MeetMeWidget::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    QHBoxLayout* rootbox = new QHBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    mFrameRootBox->addLayout(rootbox);
    rootbox->addWidget(getLeftFrame(), 3);
    rootbox->addWidget(getRightFrame(), 7);
}

QWidget* MeetMeWidget::getLeftFrame()
{
    mRoomWidget = new MultiTableWidgets(12, 1, this);
    mRoomWidget->setHeader(MiscComponents::getSectionLabel(tr("会议室")));
    return mRoomWidget;
}

QWidget* MeetMeWidget::getRightFrame()
{
    mMembersWidget = new MultiTableWidgets(36, 6, this);
    mMembersWidget->showBtns(false);
    QGridLayout* functionBar = new QGridLayout;
    mLbRoomId = new QLabel(this);
    mLbUserCount = new QLabel(this);
    QPushButton* btnInvite = new QPushButton(tr("邀请新成员"), this);
    connect(btnInvite, SIGNAL(clicked()), this, SLOT(onBtnInviteClicked()));
    QPushButton* btnNext = new QPushButton(tr("下一页"), this);
    connect(btnNext, SIGNAL(clicked()), mMembersWidget, SLOT(onBtnNextClicked()));
    QPushButton* btnPrev = new QPushButton(tr("上一页"), this);
    connect(btnPrev, SIGNAL(clicked()), mMembersWidget, SLOT(onBtnPrevClicked()));
    functionBar->addWidget(mLbRoomId, 0, 0, 1, 1);
    functionBar->addWidget(mLbUserCount, 0, 1, 1, 1);
    functionBar->addWidget(btnInvite, 0, 2, 1, 1);
    functionBar->addWidget(btnPrev, 0, 3, 1, 1);
    functionBar->addWidget(btnNext, 0, 4, 1, 1);
    QWidget* header = new QWidget(mMembersWidget);
    header->setLayout(functionBar);
    mMembersWidget->setHeader(header);
    return mMembersWidget;
}

void MeetMeWidget::reloadRoomWidget(QStringList &rooms)
{
    mRoomWidget->clear(false, true);
    foreach (QString room, rooms) {
        QPushButton* btnRoom = new QPushButton(room.right(room.length() - 8));
        UserData* data = new UserData(room);
        btnRoom->setUserData(0, data);
        connect(btnRoom, SIGNAL(clicked()), this, SLOT(onRoomBtnClicked()));
        mRoomWidget->appendWidget(btnRoom);
    }
    mRoomWidget->refresh();
}

void MeetMeWidget::reloadMembersWidget(QString roomid)
{
    mCurrentRoom = roomid;
    mMemberBtns.clear();
    mMembersWidget->clear(false, true);
    QMap< QString, QMap<QString, PBX::MeetmeMember> > roomsInfo = RPCCommand::getMeetmeInfo(mCurrentRoom);
    if (roomsInfo.contains(roomid)) {
        QMap<QString, PBX::MeetmeMember> room = roomsInfo.value(roomid);
        foreach (PBX::MeetmeMember member, room.values()) {
            PBX::Extension e;
            Member* memberBtn = new Member(roomid, member, e);
            mMemberBtns.insert(member.userNum, memberBtn);
            mMembersWidget->appendWidget(memberBtn);
        }
    }
    mMembersWidget->refresh();
}

void MeetMeWidget::onResume(QMap<QString, QVariant>& params)
{
    //读取会议室列表，更新左侧mRoomWidget
    QStringList rooms = RPCCommand::getMeetmeRooms();
    reloadRoomWidget(rooms);
    //如果有会议，读取第1个会议详细信息，更新右侧mMembersWidget
    if (rooms.size() > 0) {
        QString roomid = rooms.at(0);
        reloadMembersWidget(roomid);
    }
    BaseWidget::onResume(params);
}

void MeetMeWidget::onRoomBtnClicked()
{
    QObject* object = QObject::sender();
    QPushButton* btnRoom = (QPushButton*)qobject_cast<QPushButton*>(object);
    UserData* data = (UserData*)btnRoom->userData(0);
    QString roomid = data->getData().toString();
    reloadMembersWidget(roomid);
}

void MeetMeWidget::onBtnInviteClicked()
{
    if (mCurrentRoom == "")
        return;
    Config* cfg = Config::Instance();
    //弹出dialpad让用户输入号码或从通讯录中选择
    QString number = "";
    DialPad* dp = new DialPad();
    if (dp->exec() == QDialog::Accepted) {
        if (dp->isPhonebookSelected()) {
            QMap<QString, QVariant> param;
            param.insert("roomid", mCurrentRoom);
            param.insert("ringgroup-number", cfg->AllExtensionGroupNumber);
            param.insert("ringgroup-name", cfg->AllExtensionGroupName);
            param.insert("list-mode", UiPhoneBookList::eSelect);
            mMainWindow->loadActivity(BaseWidget::ePhoneBook, param, BaseWidget::eMeetme);
        } else {
            number = dp->getNumber();
            Config* cfg = Config::Instance();
            QString channel = QString("LOCAL/%1@%2/n").arg(number).arg(cfg->DefaultContext);
            RPCCommand::makeConferenceCall(channel, cfg->OperatorGroupCallContext, cfg->DefaultCallerId, cfg->DefaultCallerId, mCurrentRoom, false);
        }
    }
    delete dp;
}

void MeetMeWidget::showAlertInfo(QString number)
{
    static QMap<QString, PhoneBook> phonebooks;
    if (phonebooks.isEmpty()) {
        phonebooks = RPCCommand::getPhonebook("");
    }

    PhoneBook phonebook;
    for (auto val: phonebooks){
        if (val.extenNumber == number) {
            phonebook = val;
            break;
        }
    }

    if (alertDialog.isNull()) {
        QSoundEffect effect;
        effect.setSource(QUrl::fromLocalFile(":/alert.wav"));
        //    effect.setLoopCount(1);
        effect.setLoopCount(QSoundEffect::Infinite);
        effect.setVolume(0.8f);
        effect.play();

        QDialog *dig = new QDialog(this);
        dig->setWindowTitle("紧急报警");
        dig->setStyleSheet("background: white; font-size: 16px; color: #333333");

        QLabel *left = new QLabel(dig);
        QMovie *movie = new QMovie(":/alert.gif", QByteArray(), left);
        left->setMovie(movie);
        movie->setScaledSize({200,200});
        movie->start();

        QLabel *right = new QLabel(number+" 触发报警", dig);
        right->setTextFormat(Qt::RichText);
        right->setText(QString("<p style=\"font-family: ’微软雅黑‘; font-size: 24px; font-weight: bold; text-align: center; color: red\">··· 报警分机 ···</p>"
                               "<p style=\"font-family: ’微软雅黑‘; font-size: 20px; font-weight: bold; text-align: center; line-height: 18px\">%1(%2)</p>")
                       .arg(phonebook.name, number));
        right->setObjectName("AlertText");

        QHBoxLayout *row = new QHBoxLayout(dig);
        row->setContentsMargins(20,10,20,20);
        row->setSpacing(12);
        row->addWidget(left, 0, Qt::AlignCenter);
        row->addWidget(right, 0, Qt::AlignCenter);

        alertDialog = dig;
        dig->setFixedSize(640, 320);
        dig->exec();
        alertDialog = nullptr;

        dig->deleteLater();
        effect.stop();
    } else {
        QLabel *right = alertDialog.data()->findChild<QLabel*>("AlertText");
        if (right)
            right->setText(right->text().append(QString("<p style=\"font-family: ’微软雅黑‘; font-size: 16px; font-weight: bold; text-align: center; line-height: 14px\">%1(%2)</p>")
                                                .arg(phonebook.name, number)));
    }
}

Member::Member(QString roomId, PBX::MeetmeMember m, PBX::Extension e, QWidget *parent) :
    UiExtensionBase(e, parent), mMember(m), mRoomId(roomId)
{
    mBtnMute = new QPushButton(tr("静音"), this);
    mBtnMute->setCheckable(true);
    connect(mBtnMute, SIGNAL(clicked()), this, SLOT(onBtnMuteClicked()));
    if (mMember.isMuted) {
        mBtnMute->setChecked(true);
        mBtnMute->setText(tr("恢复"));
    }
    QPushButton* btnClick = new QPushButton(tr("踢出"), this);
    connect(btnClick, SIGNAL(clicked()), this, SLOT(onBtnKickClicked()));
    QLabel* lbCallerId = new QLabel(m.callerid, this);
    QLabel* lbCallerName = new QLabel(m.callerid, this);
    lbCallerName->setAlignment(Qt::AlignCenter);

    QVBoxLayout* infobox = new QVBoxLayout;
    infobox->setMargin(0);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    mIcon->setPixmap(QPixmap(mStateIconsRes[PBX::eIncall]));
    hbox->addWidget(mIcon);
    hbox->addWidget(lbCallerId);
    infobox->addLayout(hbox);
    infobox->addWidget(lbCallerName);

    QVBoxLayout* actionbox = new QVBoxLayout;
    actionbox->setMargin(0);
    actionbox->addWidget(mBtnMute);
    actionbox->addWidget(btnClick);

    QHBoxLayout* rootbox = new QHBoxLayout;
    rootbox->setMargin(0);
    setLayout(rootbox);
    rootbox->addLayout(infobox);
    rootbox->addLayout(actionbox);
    connect(this, SIGNAL(muteStatusSignal(bool)), this, SLOT(muteStatusSlot(bool)));
}

Member::~Member()
{
    LOG(Logger::Debug, "meber(%s) deleting\n", getPbxExten().number.toStdString().c_str());
}

void Member::onBtnKickClicked()
{
    RPCCommand::meetmeKick(mRoomId, mMember.userNum);
}

void Member::onBtnMuteClicked()
{
    RPCCommand::meetmeMute(mRoomId, mMember.userNum, !mMember.isMuted);
}

void Member::muteStatusSlot(bool muted)
{
    mMember.isMuted = muted;
    mBtnMute->setChecked(muted);
    if (muted)
        mBtnMute->setText(tr("恢复"));
    else
        mBtnMute->setText(tr("静音"));
}

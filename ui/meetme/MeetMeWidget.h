#ifndef MEETMEWIDGET_H
#define MEETMEWIDGET_H

#include <QtGui>
#include "ui/base/BaseWidget.h"
#include "ui/base/multitablewidgets.h"
#include "ui/base/uiextensionbase.h"

class Member;
class MeetMeWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit MeetMeWidget(MainWindow*, QWidget* parent = 0);
    ~MeetMeWidget();
    void onResume(QMap<QString, QVariant> &);
    void eventMessageReceived(QMap<QString, QString>& msg);
protected:
    void initFrames();
signals:
    void roomStatusChangeSignal(QString roomId);
private slots:
    void onRoomBtnClicked(); //左侧会议室列表的会议室按钮被点击后，触发此slot
    void roomStatusChangeSlot(QString roomid);
    void onBtnInviteClicked();
private:
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QLabel* mLbRoomId;
    QLabel* mLbUserCount;
    MultiTableWidgets* mMembersWidget;
    /* <userNum, Member*> */
    QMap<QString, Member*> mMemberBtns; //当前显示的会议的成员

    MultiTableWidgets* mRoomWidget;

    int mUserCount; //当前显示的会议室的人数
    QString mCurrentRoom; //当前显示的会议室的id

    void reloadRoomWidget(QStringList& rooms);
    void reloadMembersWidget(QString roomid);
};

class Member : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit Member(QString roomId, PBX::MeetmeMember m, PBX::Extension e, QWidget* parent = 0);
    PBX::MeetmeMember getMeetmeMember() { return mMember; }
    ~Member();
    void muteStatusChanged(bool muted) { emit muteStatusSignal(muted); }
signals:
    void muteStatusSignal(bool muted);
private slots:
    void onBtnMuteClicked();
    void onBtnKickClicked();
    void muteStatusSlot(bool muted);
private:
    QPushButton* mBtnMute;
    bool mMuted;
    PBX::MeetmeMember mMember;
    QString mRoomId;
};

#endif // MEETMEWIDGET_H

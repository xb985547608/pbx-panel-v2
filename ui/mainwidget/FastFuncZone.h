#ifndef FASTFUNCZONE_H
#define FASTFUNCZONE_H

#include <QtGui>
#include "MainWidget.h"

class FastFuncZone : public QWidget
{
    Q_OBJECT
public:
    explicit FastFuncZone(MainWidget* mainWidget, QWidget *parent = 0);
    void selectedExtenCntChanged(int count);
    void enable3WayBtn(bool enable) { mBtn3Way->setEnabled(enable); }
signals:
    
private slots:
    //fast function
    void onBtnSmsClicked();
    //void onBtnPhoneBookClicked();
    void onBtnCurrentCallClicked();
    void onBtnGroupCallClicked();
    void onBtnBroadcastClicked();
    void onBtnPagingClicked();
    void onBtnMonitorClicked();
    void onBtnHangupClicked();
    void onBtnOverrideClicked();
    void onBtnTransferClicked();
    void onBtnPickupClicked();
    void onBtn3WayClicked();
    void onBtnRollCallClicked();
    void onBtnEmergensyCallClicked();
    /*
    void onBtnPollClicked();
    */

private:
    MainWidget* mMainWidget;
    ////快速功能面板按钮:
    QToolButton* mBtnSms; //短信
    //QToolButton* mBtnPhoneBook; //通讯录
    QToolButton* mBtnCurrentCall; //当前通话
    QToolButton* mBtnGroupCall; //群呼、呼叫
    QToolButton* mBtnBroadcast; //广播
    QToolButton* mBtnPaging; //扩音
    QToolButton* mBtnMonitor;//监听
    QToolButton* mBtnHangup;//强拆
    QToolButton* mBtnOverride;//强插
    QToolButton* mBtnTransfer;//转接
    QToolButton* mBtnPickup;//代接
    QToolButton* mBtnRollCall;//点名
    QToolButton* mBtnPoll;//轮询
    QToolButton* mBtn3Way;

    QToolButton* mBtnEmergensyCall;//紧急呼叫

    void chanSpy(bool whisper); //whisper = true-强插，false-监听
};

#endif // FASTFUNCZONE_H

#ifndef FASTFUNCZONE_H
#define FASTFUNCZONE_H

#include <QtGui>
#include "MainWindow.h"

class FastFuncZone : public QWidget
{
    Q_OBJECT
public:
    explicit FastFuncZone(MainWindow* mainWindow, QWidget *parent = 0);
    void selectedExtenCntChanged(int count);
    
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
    /*
    void onBtnRollCallClicked();
    void onBtnPollClicked();
    */

private:
    MainWindow* mMainwindow;
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
};

#endif // FASTFUNCZONE_H

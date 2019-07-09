#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui/base/BaseWidget.h"
#include "ui/dialogs/dlgemergencycall.h"
#include "misc/UDPNotifier.h"
#include "misc/TCPNotifier.hpp"

const QString RESTART_FLAG = "dingo restart flag";
class MainWidget;
class SettingsWidget;
//class BaseWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QStringList args, QWidget *parent = 0);
    /*
     * 切换界面函数
     * 如果需要在targetActivity被显示后，点击“返回”按钮能返回到前一个界面，则可在第3个参数中指定界面id
     */
    void loadActivity(int targetActivityId, QMap<QString, QVariant> &params, int callerActivityId = BaseWidget::eNone);
    BaseWidget* getActivity(int);
    void eventMessageReceived(QMap<QString, QString>& msg);
signals:
    void stopOnErrorSignal();
    void onHAEventReceivedSignal(QString msg);
    void onAdapterEventReceivedSignal(QString msg, bool needRestart);
    void onEmergencyCallDlgSignal(QString strDisplayName,QString strNumber, QString strChannel, QString strUniqueid);
    
public slots :
    void stopOnErrorSlot();
    void onHAEventReceivedSlot(QString msg);
    void onAdapterEventReceivedSlot(QString msg, bool needRestart);

    void onEmergencyCallDlgSlot(QString strDisplayName,QString strNumber, QString strChannel, QString strUniqueid);
protected:
    void timerEvent(QTimerEvent* event);
    virtual void closeEvent(QCloseEvent *event);


private:
    bool mStopOnError;
    QVBoxLayout* mCentralWidgetHolder;
    //<enum activity, BaseWidget*>
    QMap<int, BaseWidget*> mActivities;
    void initActivities();
    int errorCheckTimerI;

    DlgEmergencyCall *m_dlgEmergencyCall;

    UDPNotifier *udpNotifier_;
    TCPNotifier *tcpNotifier_;

#ifdef EEPBX
    int storageCheckTimer;
    void storageFullNotice(); //判断并在存储容量接近上限时弹出提示
#endif
};

#endif // MAINWINDOW_H

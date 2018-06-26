#ifndef EXTENSIONSZONE_H
#define EXTENSIONSZONE_H

#include <QtGui>
#include "ui/base/multitablewidgets.h"
#include "ui/base/uiextensionbase.h"
#include "pbx/pbx.h"
#include "MainWidget.h"

class UiExtension;
class TextArea;
class ExtensionsZone : public QWidget
{
    Q_OBJECT
public:
    explicit ExtensionsZone(MainWidget* mainWidget, QWidget *parent = 0);
    ~ExtensionsZone();
    void loadExtensFromRinggroup(QString grpNumber);
    //extNumber == ""，表示toggle本zone内所有分机
    void toggleExten(bool, QString extNumber = "");
    void eventMessageReceived(QMap<QString, QString>& msg);
signals:
    void extenStateChangedSignal(QString extenNumber, int state);
    void extenRollCallResultSignal(QString extenNumber, int rollCallResult); //当收到分机点名结果消息时触发
public slots:
    void extenStateChangedSlot(QString number, int state);
    void extenRollCallResultSlot(QString number, int rollCallResult);
private:
    /*
    * 创建UiExtension并添加到mExtensions和mExtensPanel
    * appendToExists，不删除已有内容，追加模式
    */
    void loadExtensions(QList<PBX::Extension> &, bool appendToExists = false);
    void removeExtension(QString number);
    MainWidget* mMainWidget;
    MultiTableWidgets* mExtensPanel;
    TextArea* mTextArea;
    //QMap<number, UiExtension*>
    QMap<QString, UiExtension*> mExtensions;
    QString mRinggroupNumber; //当前widget中的分机，属于哪个振铃组
};

class UiExtension : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit UiExtension(MainWidget*, PBX::Extension e, QWidget* parent = 0);
    ~UiExtension();
signals:
    void stateChangedSignal(int state);
public slots:
    void stateChangedSlot(int state);
    void clickedSlot();
public:
    void onStateChanged(int state);
    void setRollCallState(int result); //开始点名或收到点名结果时被调用result为PBX::RollCallState，以改变分机上点名结果图标
private:
    QLabel* mLableArrow;
    QLabel* mLabelPeer;
    QLabel* mLabelDuration;
    QLabel* mRollCallIcon; //点名图标
    int mCallDuration; //当前通话时间
    MainWidget* mMainWidget;
protected:
    void timerEvent(QTimerEvent *);
};

class TextArea : public QWidget
{
    Q_OBJECT
public:
    explicit TextArea(QWidget* parent = 0);
    void setExtensCount(int);
    void setOnlineExtensCount(int);
    void setConferenceRoomCount(int);
private:
    QLabel* mExtensCount;
    QLabel* mOnlineExtensCount;
    QLabel* mConferenceRoomCount;
};

#endif // EXTENSIONSZONE_H

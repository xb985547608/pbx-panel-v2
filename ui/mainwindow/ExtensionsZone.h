#ifndef EXTENSIONSZONE_H
#define EXTENSIONSZONE_H

#include <QtGui>
#include "ui/base/multitablewidgets.h"
#include "ui/base/uiextensionbase.h"
#include "pbx/pbx.h"
#include "MainWindow.h"

class UiExtension;
class TextArea;
class ExtensionsZone : public QWidget
{
    Q_OBJECT
public:
    explicit ExtensionsZone(MainWindow* mainWindow, QWidget *parent = 0);
    void loadExtensFromRinggroup(QString grpNumber);
    void loadExtensFromCurrentCall();
    //extNumber == ""，表示toggle本zone内所有分机
    void toggleExten(bool, QString extNumber = "");
signals:
    
public slots:
private:
    void loadExtensions(QList<PBX::Extension> &);
    MainWindow* mMainwindow;
    MultiTableWidgets* mExtensPanel;
    TextArea* mTextArea;
    QList<UiExtension*> mExtensions;
};

class UiExtension : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit UiExtension(MainWindow*, PBX::Extension e, QWidget* parent = 0);
signals:
    void stateChangedSignal(int state);
public slots:
    void stateChangedSlot(int state);
    void clickedSlot();
public:
    void onStateChanged(int state);
private:
    QLabel* mLabelPeer;
    QLabel* mLabelDuration;
    QLabel* mRollCallIcon; //点名图标
    int mCallDuration; //当前通话时间
    MainWindow* mMainWindow;
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

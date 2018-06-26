#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "message_queue/MessageClient.h"
#include "OperatorZone.h"

class ExtensionsZone;
class SelectedExtensZone;
class FastFuncZone;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QStringList args, QWidget *parent = 0);
    ~MainWindow();
    void refreshOperatorsZone();
    ExtensionsZone* getExtensionsZone();
    SelectedExtensZone* getSelectedExtensZone();
    FastFuncZone* getFastFuncZone();
signals:
    void stopOnErrorSignal();

private slots:
    void onBtnEmergencyClicked();
    void onBtnEventProcessClicked();

public slots :
    void stopOnErrorSlot();
protected:
    void timerEvent(QTimerEvent *);

private:
    QWidget* getTopFrame();
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QVBoxLayout* mOperatorZoneHolder;
    OperatorZone* mOperatorsZone;
    QWidget* mRinggroupsZone;
    QWidget* mQueueZone;
    ExtensionsZone* mExtensionsZone;
    SelectedExtensZone* mSelectedExtensZone;
    FastFuncZone* mFastFuncZone;

    bool mStopOnError;
};

#endif // MAINWINDOW_H

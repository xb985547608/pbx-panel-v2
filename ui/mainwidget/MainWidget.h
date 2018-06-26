#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui>
#include "message_queue/MessageClient.h"
#include "ui/base/BaseWidget.h"

class MainWindow;
class ExtensionsZone;
class SelectedExtensZone;
class FastFuncZone;
class OperatorZone;
class RinggroupsZone;

class MainWidget : public BaseWidget
{
    Q_OBJECT
    
public:
    MainWidget(QString title,
               QString titleImage,
               QColor titleBgColor,
               Activity activityBack,
               MainWindow* mainWindow,
               QWidget *parent = 0);
    MainWidget(MainWindow*, QWidget *parent = 0);
    ~MainWidget();
    void refreshOperatorsZone();
    OperatorZone* getOperatorZone() { return mOperatorsZone; }
    ExtensionsZone* getExtensionsZone();
    SelectedExtensZone* getSelectedExtensZone();
    FastFuncZone* getFastFuncZone();
    RinggroupsZone* getRinggroupsZone();
    virtual void onResume(QMap<QString, QVariant> &);

    void ShowPhonebook(bool isEmergency=false);
signals:

public slots:
    void onBtnPhonebookClicked();
private slots:
    void onBtnEmergencyClicked();
    void onBtnEventProcessClicked();
    void onBtnSettingsClicked();
    void onBtnCdrClicked();
    void onBtnEventLogClicked();
    void onBtnMeetmeCtrlClicked();

protected:
    virtual void initFrames();

private:
    QWidget* getTopFrame();
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QVBoxLayout* mOperatorZoneHolder;
    OperatorZone* mOperatorsZone;
    RinggroupsZone* mRinggroupsZone;
    QWidget* mQueueZone;
    ExtensionsZone* mExtensionsZone;
    SelectedExtensZone* mSelectedExtensZone;
    FastFuncZone* mFastFuncZone;
};

#endif // MAINWIDGET_H

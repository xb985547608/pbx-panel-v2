#ifndef TIMEDBROADCASTEDITOR_H
#define TIMEDBROADCASTEDITOR_H

#include <QtGui>
#include "ui/base/BaseWidget.h"
#include "pbx/EventModel.h"
#include "ui/phonebook/UiPhoneBookList.h"

class SettingsWidget;
class TimedBroadcastEditor : public BaseWidget
{
    Q_OBJECT
public:
    explicit TimedBroadcastEditor(MainWindow *mainWindow, QWidget *parent = 0);
    void loadData(EventModel::Plan&);
    virtual void onResume(QMap<QString, QVariant> &);
    virtual void onPause(QMap<QString, QVariant> &);

signals:

private slots:
    //保存按钮点击时出发
//    void onBtnTimedBroadcastEditorSaveClicked();
    //定时设置编辑按钮被点击时触发
    void onBtnTimeSettingEditClicked();
    //录音文件list被点击时触发
    void onFileListViewClicked(const QModelIndex&);
    //添加分机成员点击时触发
    void onBtnAddMemberClicked();
    //删除分机成员点击时触发
    void onBtnDeleteMemberClicked();
    //选择播放文件按钮点击时触发
    void onBtnSelectRecordFileClicked();
    //播放文件上移按钮点击时触发
    void onBtnMoveUpClicked();
    //播放文件下移按钮点击时触发
    void onBtnMoveDownClicked();

protected:
    virtual void initFrames();
private:
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QStandardItemModel* mRecordingFileListModel;
    void loadRecordingFileList();

    UiPhoneBookList* mPhonebookList;
    void loadPhonebookList();

    EventModel::Plan mPlan;

    void loadData();
    void saveAndReload();
    void refreshTimeSettingDisplay();
//    void isEnableSelectBroadcastFiles(bool enable);
    void isEnableSelectNumbers(bool enable);
    void loadTimeSetting();
    /**********时间设置显示***********/
    QLineEdit* modelLine;
    QLineEdit* startTimeLine;
    QLineEdit* stopTimeLine;
    QLineEdit* dateLine;
    QLineEdit* dateExecuteWeeksLine;
    QLineEdit* cycleNumberLine;
    QLineEdit* ulimitedCycleLine;
    /*******************************/

    /**************按钮**************/
    QPushButton* btnMoveUp;
    QPushButton* btnMoveDown;
    QPushButton* selectRecordFileBtn;
    QPushButton* btnAddMember;
    QPushButton* btnDeleteMember;
    /*******************************/
};

class TimedBroadcastFileEditDialog : public QDialog
{
    Q_OBJECT
public:
    TimedBroadcastFileEditDialog(QStringList &broadFileList, QWidget* parent = 0);
    void loadAllBroadFiles();

    QStandardItemModel* mRecordingFileListModel;
    QStringList broadFileList;//=TimedBroadcastEditor.mPlan.broadcastFiles

private slots:
    void onFilesViewClicked(const QModelIndex &);
    void onBtnRecordFileSaveClicked();
    void onBtnRecordFileCannelClicked();
};

class TimedSettingEditDialog : public QDialog
{
    Q_OBJECT
public:
    TimedSettingEditDialog(EventModel::Plan *, QWidget* parent = 0);
    void initFromParentPlan();
    EventModel::Plan *temporaryPlan;//临时Plan
    /************模式设置***********************/
    QWidget* modelSelectRootWidget;
    QCheckBox* cycleNumberModel;
    QCheckBox* executeTimeModel;
    /******************************************/

    /************时间设置************************/
    QWidget* timeSelectRootWidget;
    QTimeEdit* mStartTimeEditor;
    QTimeEdit* mStopTimeEditor;
    int mStartHours;
    int mStartMinutes;
    int mStopHours;
    int mStopMinutes;
    void setTimeSelectRootWidgetEnable(bool);
    void setTimeSelectRootWidgetClear();
    /******************************************/

    /************日期设置************************/
    QWidget* dateSelectRootWidget;
    QCheckBox* sunday;
    QCheckBox* monday;
    QCheckBox* tuesday;
    QCheckBox* wednesday;
    QCheckBox* thursday;
    QCheckBox* friday;
    QCheckBox* saturday;
    QCheckBox* isExecuteEveryWeeks;
    void setDateSelectRootWidgetEnable(bool);
    void setDateSelectRootWidgetClear();
    /******************************************/

    /************循环次数设置************************/
    QWidget* cycleSelectRootWidget;
    QLabel* mcycleNumberDisplay;
    int mCycleNumber;
    QCheckBox* ulimitedCycle;
    QPushButton* btnCycleNumberIncrease;
    QPushButton* btnCycleNumberDecrease;
    void setCycleSelectRootWidgetEnable(bool);
    void setCycleSelectRootWidgetClear();
    /******************************************/
private slots:
    //循环模式选择时触发
    void onBtnCycleNumberModelSelected();
    //时间模式选择时触发
    void onBtnExecuteTimeModelSelected();
    //循环次数按钮+点击时触发
    void onBtnCycleNumberIncreaseClicked();
    //循环次数按钮-点击时触发
    void onBtnCycleNumberDecreaseClicked();
    //确定按钮点击时触发
    void onBtnOkClicked();
    //取消按钮点击时触发
    void onBtnCannelClicked();
    //每周执行被点击
    void onCboxExecuteEveryWeeksClicked();
    //星期一被点击
    void onCbox1Clicked();
    //星期二被点击
    void onCbox2Clicked();
    //星期三被点击
    void onCbox3Clicked();
    //星期四被点击
    void onCbox4Clicked();
    //星期五被点击
    void onCbox5Clicked();
    //星期六被点击
    void onCbox6Clicked();
    //星期天被点击
    void onCbox7Clicked();
};

class TimedSettingEditWarnDialog : public QDialog
{
    Q_OBJECT
public:
    TimedSettingEditWarnDialog(QString &content, QWidget* parent = 0);

private slots:
    void onBtnOkCliecked();
};

#endif // TIMEDBROADCASTEDITOR_H

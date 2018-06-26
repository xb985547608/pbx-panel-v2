#ifndef EVENTPLANEDITOR_H
#define EVENTPLANEDITOR_H

#include <QtGui>
#include "ui/base/BaseWidget.h"
#include "pbx/EventModel.h"
#include "ui/phonebook/UiPhoneBookList.h"

class SettingsWidget;
class EventPlanEditor : public BaseWidget
{
    Q_OBJECT
public:
    explicit EventPlanEditor(MainWindow *mainWindow, QWidget *parent = 0);
    void loadData(EventModel::Plan&);
    
signals:
    
private slots:
    void onBtnSmsSaveClicked();
    void onBtnBroadcastFileSaveClicked();
    void onBtnBroadcastTerminalAddClicked();
    void onBtnBroadcastTerminalDelClicked();

    //录音文件list被点击时触发
    void onFileListViewClicked(const QModelIndex&);

    //广播终端列表被点击时触发
    void onTerminalListClicked(const QModelIndex&);

    void onBtnAddMemberClicked();
    void onBtnDeleteMemberClicked();

    virtual void onResume(QMap<QString, QVariant> &);
protected:
    virtual void initFrames();
private:
    QWidget* getLeftFrame();
    QWidget* getRightFrame();

    QPlainTextEdit* mSmsEditor;

    QStandardItemModel* mRecordingFileListModel;
    void loadRecordingFileList();

    QStandardItemModel* mBroadcastTerminalListModel;
    void loadBroadcastTerminalList();

    UiPhoneBookList* mPhonebookList;
    void loadPhonebookList();

    EventModel::Plan mPlan;

    void loadData();
    void saveAndReload();
};

//添加广播终端对话框
class NewBroadcastTerminalDialog : public QDialog
{
    Q_OBJECT
public:
    NewBroadcastTerminalDialog(QWidget *parent = 0);
    QString getName() { return mTxtName->text(); }
signals:
private slots:
    void onOk();
    void onCancel();
private:
    QLineEdit* mTxtName;
};
#endif // EVENTPLANEDITOR_H

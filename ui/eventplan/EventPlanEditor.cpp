#include "EventPlanEditor.h"
#include "ui/MainWindow.h"
#include "ui/base/SectionZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "ui/phonebook/UiPhoneBook.h"
#include "ui/phonebook/UiPhoneBookList.h"
#include "misc/ftpclient.h"

EventPlanEditor::EventPlanEditor(MainWindow* mainWindow, QWidget *parent) :
    BaseWidget(
        "",
        "",
        QColor(172,211,217),
        eEventPlanEditor,
        eSettings,
        mainWindow,
        parent)
{
    EventModel::Plan plan;
    mPlan = plan;
    initFrames();
}

void EventPlanEditor::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 4);
    mFrameRootBox->addLayout(hbox);
}

void EventPlanEditor::loadData()
{
    setTitle(mPlan.name);
    mSmsEditor->setPlainText(mPlan.sms);
    loadRecordingFileList();
    loadBroadcastTerminalList();
    loadPhonebookList();
}

void EventPlanEditor::loadData(EventModel::Plan& plan)
{
    mPlan = plan;
    loadData();
}

QWidget* EventPlanEditor::getLeftFrame()
{
    QVBoxLayout* leftBox = new QVBoxLayout;
    leftBox->setMargin(0);
    leftBox->setSpacing(0);
    {
        //短信编辑
        mSmsEditor = new QPlainTextEdit(this);
        QPushButton* btnSmsSave = new QPushButton(tr("保存"), this);
        connect(btnSmsSave, SIGNAL(clicked()), this, SLOT(onBtnSmsSaveClicked()));
        SectionZone* smsZone = new SectionZone(tr("短信内容"), mSmsEditor, btnSmsSave, NULL, this);
        leftBox->addWidget(smsZone, 1);
    }
    {
        //录音文件选择
        mRecordingFileListModel = new QStandardItemModel(0, 2, this);
        QTreeView* fileList = new QTreeView(this);
        fileList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        fileList->setFocusPolicy(Qt::NoFocus);
        fileList->setRootIsDecorated(false);
        fileList->setAlternatingRowColors(true);
        fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        fileList->setHeaderHidden(true);
        fileList->setStyleSheet(
                    "QTreeView::item {height:20px; show-decoration-selected:0; font: 75 10pt;}"
                    "QTreeView::item::selected {color: green;}"
                    );
        fileList->setModel(mRecordingFileListModel);
        fileList->setColumnWidth(0, 20);
        fileList->setColumnWidth(1, 50);
        connect(fileList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onFileListViewClicked(const QModelIndex&)));
        fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);

        QPushButton* btnBroadcastFileSave = new QPushButton(tr("保存"), this);
        connect(btnBroadcastFileSave, SIGNAL(clicked()), this, SLOT(onBtnBroadcastFileSaveClicked()));
        SectionZone* zone = new SectionZone(tr("录音播放文件"), fileList, btnBroadcastFileSave, NULL, this);
        leftBox->addWidget(zone, 1);
    }
    {
        //广播终端添加
        mBroadcastTerminalListModel = new QStandardItemModel(0, 2, this);
        QTreeView* terminalList = new QTreeView(this);
        terminalList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        terminalList->setFocusPolicy(Qt::NoFocus);
        terminalList->setRootIsDecorated(false);
        terminalList->setAlternatingRowColors(true);
        terminalList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        terminalList->setHeaderHidden(true);
        terminalList->setStyleSheet(
                    "QTreeView::item {height:20px; show-decoration-selected:0; font: 75 10pt;}"
                    "QTreeView::item::selected {color: green;}"
                    );
        terminalList->setModel(mBroadcastTerminalListModel);
        terminalList->setColumnWidth(0, 20);
        terminalList->setColumnWidth(1, 50);
        connect(terminalList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTerminalListClicked(const QModelIndex&)));
        terminalList->setEditTriggers(QAbstractItemView::NoEditTriggers);

        QPushButton* btnTerminalAdd = new QPushButton(tr("添加"), this);
        connect(btnTerminalAdd, SIGNAL(clicked()), this, SLOT(onBtnBroadcastTerminalAddClicked()));
        QPushButton* btnTerminalDel = new QPushButton(tr("删除"), this);
        connect(btnTerminalDel, SIGNAL(clicked()), this, SLOT(onBtnBroadcastTerminalDelClicked()));
        SectionZone* zone = new SectionZone(tr("广播终端"), terminalList, btnTerminalAdd, btnTerminalDel, this);
        leftBox->addWidget(zone, 1);
    }
    QGroupBox* leftFrame = MiscComponents::getGroupBox(leftBox, this);
    leftFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    return leftFrame;
}

QWidget* EventPlanEditor::getRightFrame()
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    mPhonebookList = new UiPhoneBookList(this, UiPhoneBookList::ePickup);
    rootBox->addWidget(mPhonebookList, 9);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    QPushButton* btnAddMember = new QPushButton(tr("添加成员"), this);
    connect(btnAddMember, SIGNAL(clicked()), this, SLOT(onBtnAddMemberClicked()));
    QPushButton* btnDeleteMember = new QPushButton(tr("删除成员"), this);
    connect(btnDeleteMember, SIGNAL(clicked()), this, SLOT(onBtnDeleteMemberClicked()));
    hbox->addWidget(btnAddMember);
    hbox->addWidget(btnDeleteMember);
    QGroupBox* box = MiscComponents::getGroupBox(hbox, this);
    rootBox->addWidget(box, 1);
    QWidget* rightFrame = new QWidget(this);
    rightFrame->setLayout(rootBox);
    return rightFrame;
}

void EventPlanEditor::loadRecordingFileList()
{
    mRecordingFileListModel->removeRows(0, mRecordingFileListModel->rowCount());
    QStringList files = RPCCommand::getBroadcastFileList();

    foreach (QString file, files) {
        mRecordingFileListModel->insertRow(0);
        //mCdrListModel->setData(mCdrListModel->index(0, 0), cdr.uniqueId, Qt::UserRole);
        if (mPlan.broadcastFiles.contains(FTPClient::_fromSpecialEncoding(file)))
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), true, Qt::CheckStateRole);
        else
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 1), FTPClient::_fromSpecialEncoding(file));
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), file, Qt::UserRole);
        /*
        mCdrListModel->setData(mCdrListModel->index(0, 3), sndFile, Qt::UserRole);
        mCdrListModel->setData(mCdrListModel->index(0, 3), QColor(Qt::yellow), Qt::BackgroundColorRole);
        */
    }
}

void EventPlanEditor::onFileListViewClicked(const QModelIndex &index)
{
    //选中、取消选中
    if (index.column() == 0 || 1) { //不强求一定要点到checkbox
        int row = index.row();
        bool currentValue = mRecordingFileListModel->data(
                    mRecordingFileListModel->index(row, 0), Qt::CheckStateRole).toBool();
        mRecordingFileListModel->setData(
                    mRecordingFileListModel->index(row, 0), !currentValue, Qt::CheckStateRole);
        return;
    }
}

void EventPlanEditor::loadBroadcastTerminalList()
{
    mBroadcastTerminalListModel->removeRows(0, mBroadcastTerminalListModel->rowCount());
    foreach (QString terminalNumber, mPlan.terminals) {
        mBroadcastTerminalListModel->insertRow(0);
        mBroadcastTerminalListModel->setData(mBroadcastTerminalListModel->index(0, 0), false, Qt::CheckStateRole);
        mBroadcastTerminalListModel->setData(mBroadcastTerminalListModel->index(0, 1), terminalNumber);
    }
}

void EventPlanEditor::onTerminalListClicked(const QModelIndex &index)
{
    int row = index.row();
    bool checkState = !mBroadcastTerminalListModel->data(mBroadcastTerminalListModel->index(row, 0), Qt::CheckStateRole).toBool();
    mBroadcastTerminalListModel->setData(mBroadcastTerminalListModel->index(row, 0), checkState, Qt::CheckStateRole);
}

void EventPlanEditor::loadPhonebookList()
{
    mPhonebookList->clearData();
    //读取通讯录中被选中的记录
    QStringList phonebookIds;
    foreach (QString phonebookId, mPlan.numbers.keys())
        phonebookIds.append(phonebookId);
    QMap<QString, PhoneBook> records = RPCCommand::getPhonebookById(phonebookIds);
    //从records中找出对应的条目及所选号码，并生成phonebook插入到mPhonebookList
    foreach (QString id, mPlan.numbers.keys()) {
        if (!records.contains(id))
            continue;
        PhoneBook recordInPhonebook = records.value(id);
        PhoneBook record2Insert;
        record2Insert.id = id;
        record2Insert.company = recordInPhonebook.company;
        record2Insert.department = recordInPhonebook.department;
        record2Insert.position = recordInPhonebook.position;
        record2Insert.name = recordInPhonebook.name;
        QString numbers = mPlan.numbers.value(id);
        if (numbers.contains("E"))
            record2Insert.extenNumber = recordInPhonebook.extenNumber;
        if (numbers.contains("D"))
            record2Insert.directdid = recordInPhonebook.directdid;
        if (numbers.contains("M"))
            record2Insert.mobile = recordInPhonebook.mobile;
        if (numbers.contains("H"))
            record2Insert.homeNumber = recordInPhonebook.homeNumber;
        mPhonebookList->addRecord(record2Insert);
    }
}

void EventPlanEditor::onBtnSmsSaveClicked()
{
    QString sms = mSmsEditor->toPlainText();
    if (sms == mPlan.sms)
        return;
    mPlan.sms = sms;
    saveAndReload();
}

void EventPlanEditor::onBtnBroadcastFileSaveClicked()
{
    mPlan.broadcastFiles.clear();
    for (int i=0;i<mRecordingFileListModel->rowCount();i++) {
        bool checkState = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (!checkState)
            continue;
        QString fileName = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 1)).toString();
        mPlan.broadcastFiles.append(fileName);
    }
    saveAndReload();
}

void EventPlanEditor::onBtnBroadcastTerminalAddClicked()
{
    NewBroadcastTerminalDialog* dialog = new NewBroadcastTerminalDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        QString terminal = dialog->getName();
        if (mPlan.terminals.contains(terminal))
            return;
        mPlan.terminals.append(terminal);
        saveAndReload();
    }
    delete dialog;
}

void EventPlanEditor::onBtnBroadcastTerminalDelClicked()
{
    bool haveDeleteAction = false;
    for (int i=0;i<mBroadcastTerminalListModel->rowCount();i++) {
        bool checkState = mBroadcastTerminalListModel->data(mBroadcastTerminalListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (!checkState)
            continue;
        QString number = mBroadcastTerminalListModel->data(mBroadcastTerminalListModel->index(i, 1)).toString();
        if (!mPlan.terminals.contains(number)) {
            LOG(Logger::Notice, "%s is requested to remove from plan but not exists\n", number.toStdString().c_str());
            continue;
        }
        int pos = -1;
        for (pos=0; pos<mPlan.terminals.size();pos++) {
            if (mPlan.terminals.at(pos) == number)
                break;
        }
        if (pos != -1) {
            mPlan.terminals.removeAt(pos);
            haveDeleteAction = true;
        }
    }
    saveAndReload();
}

void EventPlanEditor::saveAndReload()
{
    QString planId = RPCCommand::createOrUpdateEventPlan(mPlan);
    if (mPlan.id == "") {
        mPlan.id = planId;
    }
    loadData();
}

void EventPlanEditor::onBtnAddMemberClicked()
{
    QMap<QString, QVariant> params;
    params.insert("list-mode", UiPhoneBookList::ePickup);
    mMainWindow->loadActivity(BaseWidget::ePhoneBook, params, getActivityId());
}

void EventPlanEditor::onBtnDeleteMemberClicked()
{
    bool dataChanged = false;
    QList<PhoneBook> records = mPhonebookList->getSelectedRecords();
    foreach (PhoneBook record, records) {
        if (mPlan.numbers.contains(record.id)) {
            mPlan.numbers.remove(record.id);
            dataChanged = true;
        }
    }
    if (dataChanged)
        saveAndReload();
}

void EventPlanEditor::onResume(QMap<QString, QVariant>& params)
{
    QMap<QString, QVariant> p;
    BaseWidget::onResume(p);
    int fromActivity = params.value("back-from").toInt();
    if (fromActivity != BaseWidget::ePhoneBook)
        return;
    UiPhoneBookList* phonebookList = ((UiPhoneBook*)mMainWindow->getActivity(BaseWidget::ePhoneBook))->getPhonbookList();
    //读取通讯录中选中的记录
    QList<PhoneBook> records = phonebookList->getSelectedRecords();
    if (records.size() == 0)
        return;
    //保存记录到mPlan
    foreach (PhoneBook record, records) {
        QString numbers = record.id;
        if (record.extenNumber != "")
            numbers += "-E";
        if (record.mobile != "")
            numbers += "-M";
        if (record.directdid != "")
            numbers += "-D";
        if (record.homeNumber != "")
            numbers += "-H";
        mPlan.numbers[record.id] = numbers;
    }
    saveAndReload();
}

NewBroadcastTerminalDialog::NewBroadcastTerminalDialog(QWidget *parent) :
    QDialog(parent)
{
    QString title = tr("请输入广播终端分机号");
    setWindowTitle(title);
    mTxtName = new QLineEdit(this);
    QPushButton* btnOk = new QPushButton(tr("确定"));
    QPushButton* btnCancel = new QPushButton(tr("取消"));

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->addWidget(mTxtName);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(btnOk, 1);
    hbox->addWidget(btnCancel, 1);
    rootBox->addLayout(hbox);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    setLayout(rootBox);
}

void NewBroadcastTerminalDialog::onCancel()
{
    reject();
}

void NewBroadcastTerminalDialog::onOk()
{
    QString name = mTxtName->text();
    if (name == "")
        return;
    accept();
}

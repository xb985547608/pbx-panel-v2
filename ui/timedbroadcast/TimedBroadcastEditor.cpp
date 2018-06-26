#include "TimedBroadcastEditor.h"
#include "ui/MainWindow.h"
#include "ui/base/SectionZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "ui/phonebook/UiPhoneBook.h"
#include "ui/phonebook/UiPhoneBookList.h"
#include "misc/ftpclient.h"
#include <QTimeEdit>
#include <QHBoxLayout>

TimedBroadcastEditor::TimedBroadcastEditor(MainWindow* mainWindow, QWidget *parent) :
    BaseWidget(
        "",
        "",
        QColor(172,211,217),
        eTimedBroadcastEditor,
        eSettings,
        mainWindow,
        parent)
{
    EventModel::Plan plan;
    mPlan = plan;
    initFrames();
}

void TimedBroadcastEditor::initFrames()
{
    mHeaderBox->addWidget(getTopFrame());
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 4);

    //保存修改按钮
//    QHBoxLayout* saveBtnBox = new QHBoxLayout;
//    saveBtnBox->setMargin(0);
//    saveBtnBox->setSpacing(0);
//    QPushButton* saveBtn = new QPushButton;
//    saveBtn->setText("保存");
//    connect(saveBtn, SIGNAL(clicked()), SLOT(onBtnTimedBroadcastEditorSaveClicked()));
//    saveBtnBox->addWidget(saveBtn);

    mFrameRootBox->addLayout(hbox);
//    mFrameRootBox->addLayout(saveBtnBox);
}

QWidget* TimedBroadcastEditor::getLeftFrame()
{
    QWidget* timeSelectRootWidget = new QWidget;
    QVBoxLayout* leftBox = new QVBoxLayout;
    leftBox->setMargin(0);
    leftBox->setSpacing(0);
    QVBoxLayout* timeRootBox = new QVBoxLayout;
    {
        //任务模式显示
        QHBoxLayout* modelDisplayBox = new QHBoxLayout;
        QLabel* modelLabel = new QLabel(tr("定时模式："), timeSelectRootWidget);
        modelLine = new QLineEdit(timeSelectRootWidget);
        //connect(modelLine, SIGNAL(textChanged(QString)), this, SLOT(modelLineTextChanged()));
        modelLine->setEnabled(false);
        modelDisplayBox->addWidget(modelLabel, 1);
        modelDisplayBox->addWidget(modelLine, 4);

        timeRootBox->addLayout(modelDisplayBox);
    }
    {
        //定时时间显示
        QVBoxLayout* timeDisplayBox = new QVBoxLayout;
        QHBoxLayout* startTimeDisplayBox = new QHBoxLayout;
        QHBoxLayout* stopTimeDisplayBox = new QHBoxLayout;
        QLabel* startTimeLabel = new QLabel(tr("起始时间："), timeSelectRootWidget);
        startTimeLine = new QLineEdit(timeSelectRootWidget);
        startTimeLine->setEnabled(false);
        QLabel* stopTimeLabel = new QLabel(tr("结束时间："), timeSelectRootWidget);
        stopTimeLine = new QLineEdit(timeSelectRootWidget);
        stopTimeLine->setEnabled(false);
        //stopTimeLine->setFixedWidth(50);设置固定宽度
        startTimeDisplayBox->addWidget(startTimeLabel, 1);
        startTimeDisplayBox->addWidget(startTimeLine, 3);
        stopTimeDisplayBox->addWidget(stopTimeLabel, 1);
        stopTimeDisplayBox->addWidget(stopTimeLine, 3);
        timeDisplayBox->addLayout(startTimeDisplayBox);
        timeDisplayBox->addLayout(stopTimeDisplayBox);

        timeRootBox->addLayout(timeDisplayBox);
    }
    {
        //定时日期显示
        QVBoxLayout* dateDisplayBox = new QVBoxLayout;
        QHBoxLayout* dateNumberDisplayBox = new QHBoxLayout;
        QHBoxLayout* dateExecuteWeeksDisoplayBox = new QHBoxLayout;
        QLabel* dateLabel = new QLabel(tr("定时日期："), timeSelectRootWidget);
        dateLine = new QLineEdit(timeSelectRootWidget);    //显示日期、每周循环or1次
        dateLine->setEnabled(false);
        dateNumberDisplayBox->addWidget(dateLabel, 1);
        dateNumberDisplayBox->addWidget(dateLine, 3);
        QLabel* dateExecuteWeeksLabel = new QLabel(tr("每周执行："), timeSelectRootWidget);
        dateExecuteWeeksLine = new QLineEdit(timeSelectRootWidget);
        dateExecuteWeeksLine->setEnabled(false);
        dateExecuteWeeksDisoplayBox->addWidget(dateExecuteWeeksLabel, 1);
        dateExecuteWeeksDisoplayBox->addWidget(dateExecuteWeeksLine, 3);
        dateDisplayBox->addLayout(dateNumberDisplayBox);
        dateDisplayBox->addLayout(dateExecuteWeeksDisoplayBox);

        timeRootBox->addLayout(dateDisplayBox);
    }
    {
        //循环次数显示
        QVBoxLayout* cycleDisplayBox = new QVBoxLayout;
        QHBoxLayout* cycleNumberDisplayBox = new QHBoxLayout;
        QHBoxLayout* ulimitedCycleDisplayBox = new QHBoxLayout;
        QLabel* cycleNumberLabel = new QLabel(tr("循环次数："), timeSelectRootWidget);
        //QSpinBox* cycleNumberSpinBox = new QSpinBox;
        cycleNumberLine = new QLineEdit(timeSelectRootWidget);
        cycleNumberLine->setEnabled(false);
        cycleNumberDisplayBox->addWidget(cycleNumberLabel, 1);
        cycleNumberDisplayBox->addWidget(cycleNumberLine, 3);
        QLabel* ulimitedCycleLabel = new QLabel(tr("无限循环："), timeSelectRootWidget);
        ulimitedCycleLine = new QLineEdit(timeSelectRootWidget);
        ulimitedCycleLine->setEnabled(false);
        ulimitedCycleDisplayBox->addWidget(ulimitedCycleLabel, 1);
        ulimitedCycleDisplayBox->addWidget(ulimitedCycleLine, 3);
        cycleDisplayBox->addLayout(cycleNumberDisplayBox);
        cycleDisplayBox->addLayout(ulimitedCycleDisplayBox);

        timeRootBox->addLayout(cycleDisplayBox);
    }
    {
        //编辑按钮
        QHBoxLayout* timeSettingEditBtnLayout = new QHBoxLayout;
        QPushButton* timeSettingEditBtn = new QPushButton(tr("编辑"), timeSelectRootWidget);
        connect(timeSettingEditBtn, SIGNAL(clicked()), this, SLOT(onBtnTimeSettingEditClicked()));
        timeSettingEditBtnLayout->addStretch(1);
        timeSettingEditBtnLayout->addWidget(timeSettingEditBtn);
        timeSettingEditBtnLayout->addStretch(1);

        timeRootBox->addLayout(timeSettingEditBtnLayout);
    }

    timeSelectRootWidget->setLayout(timeRootBox);
    SectionZone* timeZone = new SectionZone(tr("定时设置"), timeSelectRootWidget, NULL, NULL, this);
    leftBox->addWidget(timeZone, 1);
    {
        //录音文件选择
        QWidget* recordFileListRootWidget = new QWidget(this);
        QVBoxLayout* rootBox = new QVBoxLayout;
        QHBoxLayout* fileListLayout = new QHBoxLayout;
        mRecordingFileListModel = new QStandardItemModel(0, 2, recordFileListRootWidget);//初始化为0行2列
        QTreeView* fileList = new QTreeView(recordFileListRootWidget);
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
        fileListLayout->addWidget(fileList);

        QHBoxLayout* moveFileLayout = new QHBoxLayout;
        btnMoveUp = new QPushButton(tr("上移"), recordFileListRootWidget);
        connect(btnMoveUp, SIGNAL(clicked()), this, SLOT(onBtnMoveUpClicked()));
        btnMoveDown = new QPushButton(tr("下移"), recordFileListRootWidget);
        connect(btnMoveDown, SIGNAL(clicked()), this, SLOT(onBtnMoveDownClicked()));
        moveFileLayout->addWidget(btnMoveUp);
        moveFileLayout->addWidget(btnMoveDown);

        QHBoxLayout* selectFileLayout = new QHBoxLayout;
        selectRecordFileBtn = new QPushButton(tr("选择文件"), recordFileListRootWidget);
        connect(selectRecordFileBtn, SIGNAL(clicked()), this, SLOT(onBtnSelectRecordFileClicked()));
        selectFileLayout->addWidget(selectRecordFileBtn);

        rootBox->addLayout(fileListLayout);
        rootBox->addLayout(moveFileLayout);
        rootBox->addLayout(selectFileLayout);
        recordFileListRootWidget->setLayout(rootBox);

        SectionZone* zone = new SectionZone(tr("录音播放文件"), recordFileListRootWidget, NULL, NULL, this);
        leftBox->addWidget(zone, 2);
    }
    QGroupBox* leftFrame = MiscComponents::getGroupBox(leftBox, this);
    leftFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    return leftFrame;
}
/*
//因为在phonebookList选择号码点击返回时会向rpc提交一次plan
//如果没有配置好plan模式等会提交失败，所以界面上要求先配置这些
//再配置录音文件，最后配置号码
void TimedBroadcastEditor::modelLineTextChanged()
{
    if (modelLine->text() != "")
        isEnableSelectBroadcastFiles(true);
    else if (modelLine->text() == "")
        isEnableSelectBroadcastFiles(false);
}*/

void TimedBroadcastEditor::loadData()
{
    setTitle(mPlan.name);
    loadTimeSetting();
    loadRecordingFileList();
    loadPhonebookList();
}

void TimedBroadcastEditor::loadData(EventModel::Plan& plan)
{
    mPlan = plan;
    loadData();
}

void TimedBroadcastEditor::refreshTimeSettingDisplay()
{
    //清空显示
    modelLine->clear();
    startTimeLine->clear();
    stopTimeLine->clear();
    dateLine->clear();
    dateExecuteWeeksLine->clear();
    cycleNumberLine->clear();
    ulimitedCycleLine->clear();

    QString dates = "星期：";
    foreach (QString temp, mPlan.mDateList){
        if (temp == "")
            continue;
        else if (temp != ""){
            //dates = dates + temp + ",";
            if (temp == "Monday")
                dates = dates + "一" + ",";
            else if (temp == "Tuesday")
                dates = dates + "二" + ",";
            else if (temp == "Wednesday")
                dates = dates + "三" + ",";
            else if (temp == "Thursday")
                dates = dates + "四" + ",";
            else if (temp == "Friday")
                dates = dates + "五" + ",";
            else if (temp == "Saturday")
                dates = dates + "六" + ",";
            else if (temp == "Sunday")
                dates = dates + "日" + ",";
        }
    }
    dates = dates.left(dates.lastIndexOf(","));
    if (mPlan.mModelSelect != ""){
        if (mPlan.mModelSelect == "executeTimeModel"){
            modelLine->setText("时间模式");
            stopTimeLine->setText(mPlan.mStopTime);
        }
        else if (mPlan.mModelSelect == "cycleNumberModel"){
            modelLine->setText("循环模式");
            if (mPlan.mIsUlimiteCycle)
                ulimitedCycleLine->setText("是");
            else{
                ulimitedCycleLine->setText("否");
                cycleNumberLine->setText(mPlan.mCycleNumber);
            }
        }
        startTimeLine->setText(mPlan.mStartTime);
        dateLine->setText(dates);
        if (mPlan.mIsExecuteEveryWeeks)
            dateExecuteWeeksLine->setText("是");
        else
            dateExecuteWeeksLine->setText("否");
    }
}
/*
void TimedBroadcastEditor::isEnableSelectBroadcastFiles(bool enable)
{
    btnMoveUp->setEnabled(enable);
    btnMoveDown->setEnabled(enable);
    selectRecordFileBtn->setEnabled(enable);
}*/
/*
void TimedBroadcastEditor::isEnableSelectNumbers(bool enable)
{
    btnAddMember->setEnabled(enable);
    btnDeleteMember->setEnabled(enable);
}*/

void TimedBroadcastEditor::onBtnMoveUpClicked()
{
    //只能选中一个的限制在onFileListViewClicked中已处理，此处无需处理
    int rowsCount = mRecordingFileListModel->rowCount();
    for (int i = 0; i < rowsCount; i++){//上移必须从0向最后一行遍历
        if (mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::CheckStateRole).toBool()){
            //不是第一个
            if (i > 0) {
                //mRecordingFileListModel->takeRow(i);//将当前行剪切，行数也减1
                mRecordingFileListModel->insertRow(i - 1, mRecordingFileListModel->takeRow(i));//当前行上移
            }
        }
    }
    //刷新broadcastFiles list变量中文件名的顺序，服务端会按照0-N的顺序播放文件
    mPlan.broadcastFiles.clear();
    for (int j = 0; j < rowsCount; j++){
        QString fileName = mRecordingFileListModel->data(mRecordingFileListModel->index(j, 1)).toString();
        mPlan.broadcastFiles << fileName;
    }
}

void TimedBroadcastEditor::onBtnMoveDownClicked()
{
    int rowsCount = mRecordingFileListModel->rowCount();
    for (int i = rowsCount - 1; i >= 0; i--){//下移必须方向遍历,否则有问题,问题原因待研究
        if (mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::CheckStateRole).toBool()){
            //不是最后一个
            if (i < rowsCount - 1) {
                //mRecordingFileListModel->takeRow(i);//将当前行剪切，行数也减1
                mRecordingFileListModel->insertRow(i+1, mRecordingFileListModel->takeRow(i));//当前行下移
            }
        }
    }
    //刷新broadcastFiles list变量中文件名的顺序，服务端会按照0-N的顺序播放文件
    mPlan.broadcastFiles.clear();
    for (int j = 0; j < rowsCount; j++){
        QString fileName = mRecordingFileListModel->data(mRecordingFileListModel->index(j, 1)).toString();
        mPlan.broadcastFiles << fileName;
    }
}

QWidget* TimedBroadcastEditor::getRightFrame()
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    mPhonebookList = new UiPhoneBookList(this, UiPhoneBookList::ePickup);
    rootBox->addWidget(mPhonebookList, 9);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    btnAddMember = new QPushButton(tr("添加成员"), this);
    connect(btnAddMember, SIGNAL(clicked()), this, SLOT(onBtnAddMemberClicked()));
    btnDeleteMember = new QPushButton(tr("删除成员"), this);
    connect(btnDeleteMember, SIGNAL(clicked()), this, SLOT(onBtnDeleteMemberClicked()));
    hbox->addWidget(btnAddMember);
    hbox->addWidget(btnDeleteMember);
    QGroupBox* box = MiscComponents::getGroupBox(hbox, this);
    rootBox->addWidget(box, 1);
    QWidget* rightFrame = new QWidget(this);
    rightFrame->setLayout(rootBox);
    return rightFrame;
}

void TimedBroadcastEditor::loadRecordingFileList()
{
    mRecordingFileListModel->removeRows(0, mRecordingFileListModel->rowCount());
    QStringList broadcastFiles = RPCCommand::getBroadcastFileList();

//    foreach (QString file, files) {
//        //mCdrListModel->setData(mCdrListModel->index(0, 0), cdr.uniqueId, Qt::UserRole);
//        if (!mPlan.broadcastFiles.contains(file))
//            continue;
//        mRecordingFileListModel->insertRow(0);
//        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
//            //mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
//        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 1), FTPClient::_fromSpecialEncoding(file));
//        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), file, Qt::UserRole);
//        /*
//        mCdrListModel->setData(mCdrListModel->index(0, 3), sndFile, Qt::UserRole);
//        mCdrListModel->setData(mCdrListModel->index(0, 3), QColor(Qt::yellow), Qt::BackgroundColorRole);
//        */
//    }
    for (int i=mPlan.broadcastFiles.size()-1;i>=0;i--) {
        QString file = mPlan.broadcastFiles.at(i);
        if (broadcastFiles.contains(FTPClient::_toSpecialEncoding(file))) {
            mRecordingFileListModel->insertRow(0);
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 1), file);
            //mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), file, Qt::UserRole);
        }
    }
}

void TimedBroadcastEditor::onFileListViewClicked(const QModelIndex &index)
{
    //选中、取消选中
    if (index.column() == 0 || 1) { //不强求一定要点到checkbox
        int row = index.row();
        bool currentValue = mRecordingFileListModel->data(
                    mRecordingFileListModel->index(row, 0), Qt::CheckStateRole).toBool();
        mRecordingFileListModel->setData(
                    mRecordingFileListModel->index(row, 0), !currentValue, Qt::CheckStateRole);
        //使其它文件都没有选中，既只能选中其中一个文件
        int rowsCount = mRecordingFileListModel->rowCount();
        for (int i = 0; i < rowsCount; i++) {
            if (i == row)
                continue;
            mRecordingFileListModel->setData(mRecordingFileListModel->index(i, 0), false, Qt::CheckStateRole);
        }
        return;
    }
}

void TimedBroadcastEditor::loadTimeSetting()
{
    //清空显示
    modelLine->clear();
    startTimeLine->clear();
    stopTimeLine->clear();
    dateLine->clear();
    dateExecuteWeeksLine->clear();
    cycleNumberLine->clear();
    ulimitedCycleLine->clear();

    QString dates = "星期：";
    foreach (QString temp, mPlan.mDateList){
        if (temp == "")
            continue;
        else if (temp != ""){
            //dates = dates + temp + ",";
            if (temp == "Monday")
                dates = dates + "一" + ",";
            else if (temp == "Tuesday")
                dates = dates + "二" + ",";
            else if (temp == "Wednesday")
                dates = dates + "三" + ",";
            else if (temp == "Thursday")
                dates = dates + "四" + ",";
            else if (temp == "Friday")
                dates = dates + "五" + ",";
            else if (temp == "Saturday")
                dates = dates + "六" + ",";
            else if (temp == "Sunday")
                dates = dates + "日" + ",";
        }
    }
    dates = dates.left(dates.lastIndexOf(","));
    if (mPlan.mModelSelect != ""){
        if (mPlan.mModelSelect == "cycleNumberModel")
            modelLine->setText("循环模式");
        else if (mPlan.mModelSelect == "executeTimeModel")
            modelLine->setText("时间模式");
        startTimeLine->setText(mPlan.mStartTime);
        stopTimeLine->setText(mPlan.mStopTime);
        dateLine->setText(dates);
        dateExecuteWeeksLine->setText(((mPlan.mIsExecuteEveryWeeks) ? "是" : "否"));
        cycleNumberLine->setText(mPlan.mCycleNumber);
        ulimitedCycleLine->setText((mPlan.mIsUlimiteCycle) ? "是" : "否");
    }
}

void TimedBroadcastEditor::loadPhonebookList()
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
        if (numbers.contains("E")){
            record2Insert.extenNumber = recordInPhonebook.extenNumber;
        }
        if (numbers.contains("D")){
            record2Insert.directdid = recordInPhonebook.directdid;
        }
        if (numbers.contains("M")){
            record2Insert.mobile = recordInPhonebook.mobile;
        }
        if (numbers.contains("H")){
            record2Insert.homeNumber = recordInPhonebook.homeNumber;
        }
        mPhonebookList->addRecord(record2Insert);
    }
}

void TimedBroadcastEditor::saveAndReload()
{
    //return;
    //QString planId = RPCCommand::createOrUpdateTimingTaskPlan(mPlan);//参考createOrUpdateEventPlan方法
    QString planId = RPCCommand::createOrUpdateEventPlan(mPlan);//参考createOrUpdateEventPlan方法
    //QString planId = "27";
    if (mPlan.id == "") {
        mPlan.id = planId;
    }
    loadData();
}

void TimedBroadcastEditor::onBtnAddMemberClicked()
{
    QMap<QString, QVariant> params;
    params.insert("list-mode", UiPhoneBookList::ePickup);
    mMainWindow->loadActivity(BaseWidget::ePhoneBook, params, getActivityId());
}

void TimedBroadcastEditor::onBtnDeleteMemberClicked()
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

void TimedBroadcastEditor::onPause(QMap<QString, QVariant> &param)
{
//    QMessageBox::StandardButton btn = QMessageBox::warning(NULL, tr("温馨提示："),
//                                                           tr("是否保存当前任务配置"),
//                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//    if (btn == QMessageBox::Yes)
//        saveAndReload();
    saveAndReload();
    BaseWidget::onPause(param);
}

void TimedBroadcastEditor::onResume(QMap<QString, QVariant>& params)
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
        if (record.extenNumber != ""){
            numbers += "-E";
        }
        if (record.mobile != ""){
            numbers += "-M";
        }
        if (record.directdid != ""){
            numbers += "-D";
        }
        if (record.homeNumber != ""){
            numbers += "-H";
        }
        mPlan.numbers[record.id] = numbers;
    }
    saveAndReload();
}

//void TimedBroadcastEditor::onBtnTimedBroadcastEditorSaveClicked()
//{
//    saveAndReload();
//}

void TimedBroadcastEditor::onBtnTimeSettingEditClicked()
{
    EventModel::Plan *ptrPlan = &mPlan;
    TimedSettingEditDialog* timedSettingEditDialog = new TimedSettingEditDialog(ptrPlan, this);
    //将timedSettingEditDialog配置的参数记录到mPlan中，然后会delete timedSettingEditDialog
    //数据提交是在onBtnTimedBroadcastEditorSaveClicked方法中处理
    if (timedSettingEditDialog->exec() == QDialog::Accepted) {
        //记录星期日期
        mPlan.mDateList.clear();
        /*
        mPlan.mDateList << timedSettingEditDialog->monday->isChecked() << timedSettingEditDialog->tuesday->isChecked()
                        << timedSettingEditDialog->wednesday->isChecked() << timedSettingEditDialog->thursday->isChecked()
                        << timedSettingEditDialog->friday->isChecked() << timedSettingEditDialog->saturday->isChecked()
                        << timedSettingEditDialog->sunday->isChecked();*/
        mPlan.mDateList << ((timedSettingEditDialog->monday->isChecked()) ? "Monday" : "")
                        << ((timedSettingEditDialog->tuesday->isChecked()) ? "Tuesday" : "")
                        << ((timedSettingEditDialog->wednesday->isChecked()) ? "Wednesday" : "")
                        << ((timedSettingEditDialog->thursday->isChecked()) ? "Thursday" : "")
                        << ((timedSettingEditDialog->friday->isChecked()) ? "Friday" : "")
                        << ((timedSettingEditDialog->saturday->isChecked()) ? "Saturday" : "")
                        << ((timedSettingEditDialog->sunday->isChecked()) ? "Sunday" : "");
        //是否每周执行，否则执行一次
        mPlan.mIsExecuteEveryWeeks = timedSettingEditDialog->isExecuteEveryWeeks->isChecked();
        //只执行一次才记录日期
        if (!mPlan.mIsExecuteEveryWeeks){
            //获取当前系统时间的年、月份、日期、星期几，其中星期几为中文还是英文取决于系统
            QString currentYear = QDateTime::currentDateTime().toString("yyyy");
            QString currentMonth = QDateTime::currentDateTime().toString("MM");
            QString currentDay = QDateTime::currentDateTime().toString("dd");
            QString currentWeekDay = QDateTime::currentDateTime().toString("dddd");
            int currentWeekDayInt = 0;
            //将当前星期一或者Monday转化为int型1
            if (currentWeekDay == "星期一" || currentWeekDay == "Monday")
                currentWeekDayInt = 1;
            else if (currentWeekDay == "星期二" || currentWeekDay == "Tuesday")
                currentWeekDayInt = 2;
            else if (currentWeekDay == "星期三" || currentWeekDay == "Wednesday")
                currentWeekDayInt = 3;
            else if (currentWeekDay == "星期四" || currentWeekDay == "Thursday")
                currentWeekDayInt = 4;
            else if (currentWeekDay == "星期五" || currentWeekDay == "Friday")
                currentWeekDayInt = 5;
            else if (currentWeekDay == "星期六" || currentWeekDay == "Saturday")
                currentWeekDayInt = 6;
            else if (currentWeekDay == "星期日" || currentWeekDay == "Sunday")
                currentWeekDayInt = 7;
            QString executeWeekDay = "";
            int executeWeekDayInt = 0;
            foreach (QString temp, mPlan.mDateList) {
                if (temp == "")
                    continue;
                else if (temp != "")
                    executeWeekDay = temp;
            }
            //将任务执行的Monday转化为int型1
            if (executeWeekDay == "星期一" || executeWeekDay == "Monday")
                executeWeekDayInt = 1;
            else if (executeWeekDay == "星期二" || executeWeekDay == "Tuesday")
                executeWeekDayInt = 2;
            else if (executeWeekDay == "星期三" || executeWeekDay == "Wednesday")
                executeWeekDayInt = 3;
            else if (executeWeekDay == "星期四" || executeWeekDay == "Thursday")
                executeWeekDayInt = 4;
            else if (executeWeekDay == "星期五" || executeWeekDay == "Friday")
                executeWeekDayInt = 5;
            else if (executeWeekDay == "星期六" || executeWeekDay == "Saturday")
                executeWeekDayInt = 6;
            else if (executeWeekDay == "星期日" || executeWeekDay == "Sunday")
                executeWeekDayInt = 7;
            int dateDiffrence = executeWeekDayInt - currentWeekDayInt;
            //同一周执行
            QString tempday;
            QString tempmonth;
            QString tempyear;
            QString executeYear;
            QString executeMonth;
            QString executeDay;
            if (dateDiffrence >= 0){
                tempday = QDateTime::currentDateTime().addDays(dateDiffrence).toString("dd");
            }
            //下一周执行
            else if (dateDiffrence < 0){
                tempday = QDateTime::currentDateTime().addDays(7 + dateDiffrence).toString("dd");
            }

            if (tempday.toInt() < currentDay.toInt()) {//执行时间为下个月了
                tempmonth = QDateTime::currentDateTime().addMonths(1).toString("MM");
                if (tempmonth.toInt() < currentMonth.toInt())//执行时间为明年
                    tempyear = QDateTime::currentDateTime().addYears(1).toString("yyyy");
                executeDay = tempday;
                executeMonth = tempmonth;
                executeYear = tempyear;
            }
            else{//本月执行
                executeDay = tempday;
                executeMonth = currentMonth;
                executeYear = currentYear;
            }

            mPlan.mExecuteDate = executeYear + "-" + executeMonth + "-" + executeDay;
        }

        //循环模式
        if (timedSettingEditDialog->cycleNumberModel->isChecked()) {
            mPlan.mModelSelect = "cycleNumberModel";
            mPlan.mStartTime = timedSettingEditDialog->mStartTimeEditor->text();
            mPlan.mCycleNumber = QString::number(timedSettingEditDialog->mCycleNumber, 10);
            mPlan.mIsUlimiteCycle = timedSettingEditDialog->ulimitedCycle->isChecked();
        }
        //时间模式
        else if (timedSettingEditDialog->executeTimeModel->isChecked()) {
            mPlan.mModelSelect = "executeTimeModel";
            mPlan.mStartTime = timedSettingEditDialog->mStartTimeEditor->text();
            mPlan.mStopTime = timedSettingEditDialog->mStopTimeEditor->text();
        }
        refreshTimeSettingDisplay();
    }
    delete timedSettingEditDialog;
}

void TimedBroadcastEditor::onBtnSelectRecordFileClicked()
{
    TimedBroadcastFileEditDialog* timedBroadcastFileEditDialog = new TimedBroadcastFileEditDialog(mPlan.broadcastFiles, this);
    if (timedBroadcastFileEditDialog->exec() == QDialog::Accepted){
        mPlan.broadcastFiles.clear();
        mPlan.broadcastFiles = timedBroadcastFileEditDialog->broadFileList;
        //mRecordingFileListModel = timedBroadcastFileEditDialog->mRecordingFileListModel;
        loadRecordingFileList();
    }
    delete timedBroadcastFileEditDialog;
}

////////////////////////////////////////////////////////////////////
//TimedBroadcastFileEditDialog

TimedBroadcastFileEditDialog::TimedBroadcastFileEditDialog(QStringList &files, QWidget *parent) :
    broadFileList(files), QDialog(parent)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    mRecordingFileListModel = new QStandardItemModel(0, 2, this);

    loadAllBroadFiles();//通过rpc取播放文件

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
    connect(fileList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onFilesViewClicked(const QModelIndex&)));
    fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton* recordFileSaveBtn = new QPushButton(tr("保存"), this);
    connect(recordFileSaveBtn, SIGNAL(clicked()), this, SLOT(onBtnRecordFileSaveClicked()));
    QPushButton* recordFileCannelBtn = new QPushButton(tr("取消"), this);
    connect(recordFileCannelBtn, SIGNAL(clicked()), this, SLOT(onBtnRecordFileCannelClicked()));
    SectionZone* zone = new SectionZone(tr("请选择播放文件"), fileList, recordFileSaveBtn, recordFileCannelBtn, this);
    rootBox->addWidget(zone);

    setLayout(rootBox);
}

void TimedBroadcastFileEditDialog::onBtnRecordFileSaveClicked()
{
    int rowsCount = mRecordingFileListModel->rowCount();//取共有多少行数据
    broadFileList.clear();
    for (int i = 0; i < rowsCount; i++){
        //检查该文件是否选中
        if (mRecordingFileListModel->data(
                    mRecordingFileListModel->index(i, 0), Qt::CheckStateRole).toBool()) {
            //将文件名取出来
            //QString fileName = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::UserRole).toString();
            QString fileName = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 1)).toString();
            //若该文件名已在broadFileList中，则不处理
            if (broadFileList.contains(fileName))
                continue;
            broadFileList << fileName;
        }
    }
    accept();
}

void TimedBroadcastFileEditDialog::onBtnRecordFileCannelClicked()
{
    reject();
}

void TimedBroadcastFileEditDialog::onFilesViewClicked(const QModelIndex &index)
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

void TimedBroadcastFileEditDialog::loadAllBroadFiles()
{
    mRecordingFileListModel->removeRows(0, mRecordingFileListModel->rowCount());
    QStringList files = RPCCommand::getBroadcastFileList();

    foreach (QString file, files) {
        mRecordingFileListModel->insertRow(0);
        //mCdrListModel->setData(mCdrListModel->index(0, 0), cdr.uniqueId, Qt::UserRole);
        if (broadFileList.contains(file))
            //如果broadFileList中包含了file名，就将该播放文件显示出来的时候置为位选中
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), true, Qt::CheckStateRole);
        else
            mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 1), FTPClient::_fromSpecialEncoding(file));
        //mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), file, Qt::UserRole);
        /*
        mCdrListModel->setData(mCdrListModel->index(0, 3), sndFile, Qt::UserRole);
        mCdrListModel->setData(mCdrListModel->index(0, 3), QColor(Qt::yellow), Qt::BackgroundColorRole);
        */
    }
}

//////////////////////////////////////////////////////////////////////////////////////
//TimedSettingEditDialog

TimedSettingEditDialog::TimedSettingEditDialog(EventModel::Plan *parentPlan, QWidget* parent) :
    QDialog(parent), temporaryPlan(parentPlan)
{
    QString title = tr("定时设置");
    setWindowTitle(title);
    QVBoxLayout* rootBox = new QVBoxLayout;
    QHBoxLayout* topRootBox = new QHBoxLayout;
    QHBoxLayout* centerRootBox = new QHBoxLayout;
    QHBoxLayout* bottomRootBox = new QHBoxLayout;
    {
        //模式编辑
        modelSelectRootWidget = new QWidget(this);
        QHBoxLayout* modelSelectRootBox = new QHBoxLayout;

        cycleNumberModel = new QCheckBox(tr("循环模式"), modelSelectRootWidget);
        connect(cycleNumberModel, SIGNAL(stateChanged(int)), this, SLOT(onBtnCycleNumberModelSelected()));
        executeTimeModel = new QCheckBox(tr("时间模式"), modelSelectRootWidget);
        connect(executeTimeModel, SIGNAL(stateChanged(int)), this, SLOT(onBtnExecuteTimeModelSelected()));
        modelSelectRootBox->addWidget(cycleNumberModel);
        modelSelectRootBox->addWidget(executeTimeModel);
        modelSelectRootWidget->setLayout(modelSelectRootBox);

        SectionZone* modelSelectZone = new SectionZone(tr("模式选择"), modelSelectRootWidget, NULL, NULL, this);
        topRootBox->addWidget(modelSelectZone, 1);
    }
    {
        //定时日期编辑
        dateSelectRootWidget = new QWidget(this);
        QVBoxLayout* dateSelectRootBox = new QVBoxLayout;

        monday = new QCheckBox(tr("星期一"), dateSelectRootWidget);
        connect(monday, SIGNAL(clicked()), this, SLOT(onCbox1Clicked()));
        tuesday = new QCheckBox(tr("星期二"), dateSelectRootWidget);
        connect(tuesday, SIGNAL(clicked()), this, SLOT(onCbox2Clicked()));
        wednesday = new QCheckBox(tr("星期三"), dateSelectRootWidget);
        connect(wednesday, SIGNAL(clicked()), this, SLOT(onCbox3Clicked()));
        thursday = new QCheckBox(tr("星期四"), dateSelectRootWidget);
        connect(thursday, SIGNAL(clicked()), this, SLOT(onCbox4Clicked()));
        QHBoxLayout* dateSelectBox1 = new QHBoxLayout;
        dateSelectBox1->addWidget(monday, 1);
        dateSelectBox1->addWidget(tuesday, 1);
        dateSelectBox1->addWidget(wednesday, 1);
        dateSelectBox1->addWidget(thursday, 1);

        friday = new QCheckBox(tr("星期五"), dateSelectRootWidget);
        connect(friday, SIGNAL(clicked()), this, SLOT(onCbox5Clicked()));
        saturday = new QCheckBox(tr("星期六"), dateSelectRootWidget);
        connect(saturday, SIGNAL(clicked()), this, SLOT(onCbox6Clicked()));
        sunday = new QCheckBox(tr("星期日"), dateSelectRootWidget);
        connect(sunday, SIGNAL(clicked()), this, SLOT(onCbox7Clicked()));
        isExecuteEveryWeeks = new QCheckBox(tr("每周循环"), dateSelectRootWidget);
        connect(isExecuteEveryWeeks, SIGNAL(clicked()), this, SLOT(onCboxExecuteEveryWeeksClicked()));
        QHBoxLayout* dateSelectBox2 = new QHBoxLayout;
        dateSelectBox2->addWidget(friday, 1);
        dateSelectBox2->addWidget(saturday, 1);
        dateSelectBox2->addWidget(sunday, 1);
        dateSelectBox2->addWidget(isExecuteEveryWeeks, 1);

        dateSelectRootBox->addLayout(dateSelectBox1);
        dateSelectRootBox->addLayout(dateSelectBox2);

        dateSelectRootWidget->setLayout(dateSelectRootBox);
        dateSelectRootWidget->setEnabled(false);

        SectionZone* dateSelectZone = new SectionZone(tr("定时日期"), dateSelectRootWidget, NULL, NULL, this);
        topRootBox->addWidget(dateSelectZone, 2);
    }
    {
        //定时时间编辑
        timeSelectRootWidget = new QWidget(this);
        QVBoxLayout* timeSelectRootBox = new QVBoxLayout;
        //起始时间
        QLabel* startTimeLabel = new QLabel(tr("起始时间："), timeSelectRootWidget);
        mStartTimeEditor = new QTimeEdit(timeSelectRootWidget);
        mStartTimeEditor->setDisplayFormat("hh:mm");
        mStartTimeEditor->setAlignment(Qt::AlignCenter);
        //mStartTimeEditor->setReadOnly(true);//设置只读
        QHBoxLayout* startTimeHBox = new QHBoxLayout;
        startTimeHBox->addWidget(startTimeLabel, 3);
        startTimeHBox->addWidget(mStartTimeEditor, 7);
        //终止时间
        QLabel* stopTimeLabel = new QLabel(tr("结束时间："), timeSelectRootWidget);
        mStopTimeEditor = new QTimeEdit(timeSelectRootWidget);
        mStopTimeEditor->setDisplayFormat("hh:mm");
        mStopTimeEditor->setAlignment(Qt::AlignCenter);
        //mTimeEditor->setReadOnly(true);//设置只读
        QHBoxLayout* stopTimeHBox = new QHBoxLayout;
        stopTimeHBox->addWidget(stopTimeLabel, 3);
        stopTimeHBox->addWidget(mStopTimeEditor, 7);

        timeSelectRootBox->addLayout(startTimeHBox);
        timeSelectRootBox->addLayout(stopTimeHBox);

        timeSelectRootWidget->setLayout(timeSelectRootBox);
        timeSelectRootWidget->setEnabled(false);
        SectionZone* timeSelectZone = new SectionZone(tr("定时时间"), timeSelectRootWidget, NULL, NULL, this);
        centerRootBox->addWidget(timeSelectZone, 2);
    }
    {
        //循环次数选择
        cycleSelectRootWidget = new QWidget(this);
        QVBoxLayout* cycleSelectRootBox = new QVBoxLayout;

        btnCycleNumberIncrease = new QPushButton(tr("+"), cycleSelectRootWidget);
        connect(btnCycleNumberIncrease, SIGNAL(clicked()), this, SLOT(onBtnCycleNumberIncreaseClicked()));
        mcycleNumberDisplay = new QLabel(cycleSelectRootWidget);
//        mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
        mcycleNumberDisplay->setAlignment(Qt::AlignCenter);
        btnCycleNumberDecrease = new QPushButton(tr("-"), cycleSelectRootWidget);
        connect(btnCycleNumberDecrease, SIGNAL(clicked()), this, SLOT(onBtnCycleNumberDecreaseClicked()));
        QHBoxLayout* hcycleControlBox = new QHBoxLayout;
        hcycleControlBox->addWidget(btnCycleNumberIncrease);
//        hcycleControlBox->addStretch(1);
        hcycleControlBox->addWidget(mcycleNumberDisplay);
//        hcycleControlBox->addStretch(1);
        hcycleControlBox->addWidget(btnCycleNumberDecrease);

        ulimitedCycle = new QCheckBox(tr("无限循环"), cycleSelectRootWidget);
        QHBoxLayout* ulimitedCycleControlBox = new QHBoxLayout;
        ulimitedCycleControlBox->addStretch();
        ulimitedCycleControlBox->addWidget(ulimitedCycle);
        ulimitedCycleControlBox->addStretch();

        cycleSelectRootBox->addLayout(hcycleControlBox);
        cycleSelectRootBox->addLayout(ulimitedCycleControlBox);

        cycleSelectRootWidget->setLayout(cycleSelectRootBox);
        cycleSelectRootWidget->setEnabled(false);

        SectionZone* cycleSelectRootZone = new SectionZone(tr("循环次数"), cycleSelectRootWidget, NULL, NULL, this);
        centerRootBox->addWidget(cycleSelectRootZone, 3);
    }
    {
        QPushButton* btnOk = new QPushButton(tr("确定"), this);
        connect(btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkClicked()));
        QPushButton* btnCannel = new QPushButton(tr("取消"), this);
        connect(btnCannel, SIGNAL(clicked()), this, SLOT(onBtnCannelClicked()));
        QHBoxLayout* btnSelectBox = new QHBoxLayout;
        btnSelectBox->addWidget(btnOk);
        btnSelectBox->addWidget(btnCannel);

        bottomRootBox->addLayout(btnSelectBox);
    }
    rootBox->addLayout(topRootBox);
    rootBox->addLayout(centerRootBox);
    rootBox->addLayout(bottomRootBox);
    //rootBox->addLayout(bottomRootBox);

    initFromParentPlan();
    setLayout(rootBox);
}

void TimedSettingEditDialog::onCboxExecuteEveryWeeksClicked()
{
    //每周执行未被选中，清空星期一到星期日的选项
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        monday->setChecked(false);
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox1Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox2Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        monday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox3Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        monday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox4Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        monday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox5Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        monday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox6Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        monday->setChecked(false);
        sunday->setChecked(false);
    }
}

void TimedSettingEditDialog::onCbox7Clicked()
{
    if (isExecuteEveryWeeks->checkState() != Qt::Checked){
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        monday->setChecked(false);
    }
}

void TimedSettingEditDialog::initFromParentPlan()
{
    if (temporaryPlan->mModelSelect == "cycleNumberModel"){
        //初始化模式
        cycleNumberModel->setChecked(true);
        //初始化日期
        /*
        monday->setChecked(temporaryPlan->mDateList.at(0));
        tuesday->setChecked(temporaryPlan->mDateList.at(1));
        wednesday->setChecked(temporaryPlan->mDateList.at(2));
        thursday->setChecked(temporaryPlan->mDateList.at(3));
        friday->setChecked(temporaryPlan->mDateList.at(4));
        saturday->setChecked(temporaryPlan->mDateList.at(5));
        sunday->setChecked(temporaryPlan->mDateList.at(6));*/
        monday->setChecked(false);
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
        foreach (QString temp, temporaryPlan->mDateList){
            if (temp == "")
                continue;
            else if (temp == "Monday")
                monday->setChecked(true);
            else if (temp == "Tuesday")
                tuesday->setChecked(true);
            else if (temp == "Wednesday")
                wednesday->setChecked(true);
            else if (temp == "Thursday")
                thursday->setChecked(true);
            else if (temp == "Friday")
                friday->setChecked(true);
            else if (temp == "Saturday")
                saturday->setChecked(true);
            else if (temp == "Sunday")
                sunday->setChecked(true);
        }
        isExecuteEveryWeeks->setChecked(temporaryPlan->mIsExecuteEveryWeeks);
        //初始化起始时间
        mStartHours = temporaryPlan->mStartTime.left(temporaryPlan->mStartTime.lastIndexOf(":")).toInt();
        mStartMinutes = temporaryPlan->mStartTime.right(temporaryPlan->mStartTime.lastIndexOf(":")).toInt();
        QTime* startInitTime = new QTime(mStartHours, mStartMinutes);
        mStartTimeEditor->setTime(*startInitTime);
        mStartTimeEditor->setAlignment(Qt::AlignCenter);//字体居中对齐
//        mStopHours = temporaryPlan->mStopTime.left(temporaryPlan->mStopTime.lastIndexOf(":")).toInt();
//        mStopMinutes = temporaryPlan->mStopTime.right(temporaryPlan->mStopTime.lastIndexOf(":")).toInt();
//        QTime* stopInitTime = new QTime(mStopHours, mStopMinutes);
//        mStopTimeEditor->setTime(*stopInitTime);
//        mStopTimeEditor->setAlignment(Qt::AlignCenter);
        //初始化循环次数
        if (temporaryPlan->mIsUlimiteCycle)
            ulimitedCycle->setChecked(temporaryPlan->mIsUlimiteCycle);
        else{
            if (temporaryPlan->mCycleNumber == "")
                mCycleNumber = 1;
            else
                mCycleNumber = temporaryPlan->mCycleNumber.toInt();
            mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
        }
    }
    else if (temporaryPlan->mModelSelect == "executeTimeModel"){
        //初始化模式
        executeTimeModel->setChecked(true);
        //初始化日期
        /*
        monday->setChecked(temporaryPlan->mDateList.at(0));
        tuesday->setChecked(temporaryPlan->mDateList.at(1));
        wednesday->setChecked(temporaryPlan->mDateList.at(2));
        thursday->setChecked(temporaryPlan->mDateList.at(3));
        friday->setChecked(temporaryPlan->mDateList.at(4));
        saturday->setChecked(temporaryPlan->mDateList.at(5));
        sunday->setChecked(temporaryPlan->mDateList.at(6));
        */
        monday->setChecked(false);
        tuesday->setChecked(false);
        wednesday->setChecked(false);
        thursday->setChecked(false);
        friday->setChecked(false);
        saturday->setChecked(false);
        sunday->setChecked(false);
        foreach (QString temp, temporaryPlan->mDateList){
            if (temp == "")
                continue;
            else if (temp == "Monday")
                monday->setChecked(true);
            else if (temp == "Tuesday")
                tuesday->setChecked(true);
            else if (temp == "Wednesday")
                wednesday->setChecked(true);
            else if (temp == "Thursday")
                thursday->setChecked(true);
            else if (temp == "Friday")
                friday->setChecked(true);
            else if (temp == "Saturday")
                saturday->setChecked(true);
            else if (temp == "Sunday")
                sunday->setChecked(true);
        }
        isExecuteEveryWeeks->setChecked(temporaryPlan->mIsExecuteEveryWeeks);
        //初始化起始结束时间
        mStartHours = temporaryPlan->mStartTime.left(temporaryPlan->mStartTime.lastIndexOf(":")).toInt();
        mStartMinutes = temporaryPlan->mStartTime.right(temporaryPlan->mStartTime.lastIndexOf(":")).toInt();
        QTime* startInitTime = new QTime(mStartHours, mStartMinutes);
        mStartTimeEditor->setTime(*startInitTime);
        mStartTimeEditor->setAlignment(Qt::AlignCenter);//字体居中对齐
        mStopHours = temporaryPlan->mStopTime.left(temporaryPlan->mStopTime.lastIndexOf(":")).toInt();
        mStopMinutes = temporaryPlan->mStopTime.right(temporaryPlan->mStopTime.lastIndexOf(":")).toInt();
        QTime* stopInitTime = new QTime(mStopHours, mStopMinutes);
        mStopTimeEditor->setTime(*stopInitTime);
        mStopTimeEditor->setAlignment(Qt::AlignCenter);
    }
    else{
        mCycleNumber = 1;
        mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
    }
}

void TimedSettingEditDialog::onBtnCycleNumberIncreaseClicked()
{
    mCycleNumber++;
    mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
}

void TimedSettingEditDialog::onBtnCycleNumberDecreaseClicked()
{
    if (mCycleNumber == 1)
        return;
    mCycleNumber--;
    mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
}

void TimedSettingEditDialog::setDateSelectRootWidgetEnable(bool isEnable)
{
    dateSelectRootWidget->setEnabled(isEnable);
    monday->setEnabled(isEnable);
    tuesday->setEnabled(isEnable);
    wednesday->setEnabled(isEnable);
    thursday->setEnabled(isEnable);
    friday->setEnabled(isEnable);
    saturday->setEnabled(isEnable);
    sunday->setEnabled(isEnable);
    isExecuteEveryWeeks->setEnabled(isEnable);
}

void TimedSettingEditDialog::setDateSelectRootWidgetClear()
{
    monday->setChecked(false);
    tuesday->setChecked(false);
    wednesday->setChecked(false);
    thursday->setChecked(false);
    friday->setChecked(false);
    saturday->setChecked(false);
    sunday->setChecked(false);
    isExecuteEveryWeeks->setChecked(false);
}

void TimedSettingEditDialog::setTimeSelectRootWidgetEnable(bool isEnable)
{
    timeSelectRootWidget->setEnabled(isEnable);
    mStartTimeEditor->setEnabled(isEnable);
    mStopTimeEditor->setEnabled(isEnable);
    btnCycleNumberIncrease->setEnabled(isEnable);
    btnCycleNumberDecrease->setEnabled(isEnable);
}

void TimedSettingEditDialog::setTimeSelectRootWidgetClear()
{
    mStartHours = 0;
    mStartMinutes = 0;
    mStopHours = 0;
    mStopMinutes = 0;
    QTime* startInitTime = new QTime(mStartHours, mStartMinutes);
    mStartTimeEditor->setTime(*startInitTime);
    QTime* stopInitTime = new QTime(mStopHours, mStopMinutes);
    mStartTimeEditor->setTime(*stopInitTime);
}

void TimedSettingEditDialog::setCycleSelectRootWidgetEnable(bool isEnable)
{
    cycleSelectRootWidget->setEnabled(isEnable);
    ulimitedCycle->setEnabled(isEnable);
}

void TimedSettingEditDialog::setCycleSelectRootWidgetClear()
{
    mCycleNumber = 1;
    mcycleNumberDisplay->setText(QString("%1").arg(mCycleNumber));
    ulimitedCycle->setChecked(false);
}

void TimedSettingEditDialog::onBtnCycleNumberModelSelected()
{
    //循环模式选中
    if (cycleNumberModel->isChecked()){
        executeTimeModel->setChecked(false);//取消时间模式选中状态
        setDateSelectRootWidgetEnable(true);
        setTimeSelectRootWidgetEnable(true);
        setCycleSelectRootWidgetEnable(true);
        mStopTimeEditor->setEnabled(false);
    }
    else if ((!cycleNumberModel->isChecked()) && (!executeTimeModel->isChecked())){
        setDateSelectRootWidgetEnable(false);
        setTimeSelectRootWidgetEnable(false);
        setCycleSelectRootWidgetEnable(false);
    }

    setDateSelectRootWidgetClear();
    setTimeSelectRootWidgetClear();
    setCycleSelectRootWidgetClear();
}

void TimedSettingEditDialog::onBtnExecuteTimeModelSelected()
{
    //时间模式选中
    if (executeTimeModel->isChecked()){
        cycleNumberModel->setChecked(false);//取消循环模式选中状态
        setDateSelectRootWidgetEnable(true);
        setTimeSelectRootWidgetEnable(true);
        setCycleSelectRootWidgetEnable(false);
    }
    else if ((!executeTimeModel->isChecked()) && (!cycleNumberModel->isChecked())){
        setDateSelectRootWidgetEnable(false);
        setTimeSelectRootWidgetEnable(false);
        setCycleSelectRootWidgetEnable(false);
    }
    setDateSelectRootWidgetClear();
    setTimeSelectRootWidgetClear();
    setCycleSelectRootWidgetClear();
}

void TimedSettingEditDialog::onBtnOkClicked()
{
    QString warnContent;
    TimedSettingEditWarnDialog* timedSettingEditWarnDialog;
    //没有选择模式
    if ((!cycleNumberModel->isChecked()) && (!executeTimeModel->isChecked())) {
        warnContent = "请选择任务模式！";
        timedSettingEditWarnDialog = new TimedSettingEditWarnDialog(warnContent, this);
        if (timedSettingEditWarnDialog->exec() == QDialog::Accepted) {
            delete timedSettingEditWarnDialog;
            return;
        }
    }
    //日期没有选择
    if ((!monday->isChecked()) && (!tuesday->isChecked()) && (!wednesday->isChecked()) && (!thursday->isChecked())
            && (!friday->isChecked()) && (!saturday->isChecked()) && (!sunday->isChecked())) {
        warnContent = "请选择任务日期！";
        timedSettingEditWarnDialog = new TimedSettingEditWarnDialog(warnContent, this);
        if (timedSettingEditWarnDialog->exec() == QDialog::Accepted) {
            delete timedSettingEditWarnDialog;
            return;
        }
    }

    //时间模式才有结束时间
    if (executeTimeModel->isChecked()) {
        //每个任务必须于当天结束
        QString tempStartTime = mStartTimeEditor->text();
        QString tempStopTime = mStopTimeEditor->text();
        QStringList tempStartTimeList = tempStartTime.split(":", QString::SkipEmptyParts);
        QStringList tempStopTimeList = tempStopTime.split(":", QString::SkipEmptyParts);
        if (tempStopTimeList.at(0).toInt() < tempStartTimeList.at(0).toInt()) {//结束时间小时小于起始时间小时
            warnContent = "结束时间在起始时间之前！(提示：任务必须一天内执行且停止！)";
            timedSettingEditWarnDialog = new TimedSettingEditWarnDialog(warnContent, this);
            if (timedSettingEditWarnDialog->exec() == QDialog::Accepted) {
                delete timedSettingEditWarnDialog;
                return;
            }
        }
        else if (tempStopTimeList.at(0).toInt() == tempStartTimeList.at(0).toInt()) {//结束时间小时等于起始时间小时
            if (tempStopTimeList.at(1).toInt() < tempStartTimeList.at(1).toInt()) {//结束时间分钟小于起始时间分钟
                warnContent = "结束时间在起始时间之前！(提示：任务必须一天内执行且停止！)";
                timedSettingEditWarnDialog = new TimedSettingEditWarnDialog(warnContent, this);
                if (timedSettingEditWarnDialog->exec() == QDialog::Accepted) {
                    delete timedSettingEditWarnDialog;
                    return;
                }
            }else if (tempStopTimeList.at(1).toInt() == tempStartTimeList.at(1).toInt()) {//结束时间分钟等于起始时间分钟
                warnContent = "结束时间不能和起始时间相同！";
                timedSettingEditWarnDialog = new TimedSettingEditWarnDialog(warnContent, this);
                if (timedSettingEditWarnDialog->exec() == QDialog::Accepted) {
                    delete timedSettingEditWarnDialog;
                    return;
                }
            }
        }
    }

    //界面close，但没有delete，扔可以从中取数据，返回true
    accept();//if (timedSettingEditDialog->exec() == QDialog::Accepted)返回true
}

void TimedSettingEditDialog::onBtnCannelClicked()
{
    //界面close，但没有delete，返回true
    reject();//if (timedSettingEditDialog->exec() == QDialog::Accepted)返回false
}

//////////////////////////////////////////////////////////////////////////////////////
TimedSettingEditWarnDialog::TimedSettingEditWarnDialog(QString &content, QWidget* parent) :
    QDialog(parent)
{
    QString title = "温馨提示";
    setWindowTitle(title);
    QVBoxLayout* rootBox = new QVBoxLayout;

    QLabel* contentLabel = new QLabel(tr(content.toStdString().c_str()), this);
    rootBox->addWidget(contentLabel);
    QPushButton* btnOk = new QPushButton(tr("确定"), this);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkCliecked()));
    rootBox->addWidget(btnOk);
    setLayout(rootBox);
}

void TimedSettingEditWarnDialog::onBtnOkCliecked()
{
    accept();
}

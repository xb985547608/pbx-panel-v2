#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "CallLogWidget.h"
#include "misc/Config.h"
#include "misc/logger.h"
#include "pbx/pbx.h"
#include "message_queue/RPCCommand.h"


static QString time_display_format = "yyyy-MM-dd hh:mm:ss";
CallLogWidget::CallLogWidget(MainWindow* mw, QWidget* parent) :
    BaseWidget(tr("呼叫记录"), Config::Instance()->Res_icon_cdr, QColor(231,226,160),
               BaseWidget::eCDR, BaseWidget::eMainWidget, mw, parent)
{
    initFrames();
}

void CallLogWidget::initFrames()
{
    mHeaderBox->addWidget(getTopFrame(), 1);

    //查询框
    QString format = time_display_format;
    QDateTime now = QDateTime::currentDateTime();
    QHBoxLayout* hbox = new QHBoxLayout;
    QLabel* label = new QLabel(tr("起始时间"), this);
    hbox->addWidget(label);
    mStartDateTimeEdit = new QDateTimeEdit(now, this);
    mStartDateTimeEdit->setDisplayFormat(format);
    hbox->addWidget(mStartDateTimeEdit);
    hbox->addStretch();
    label = new QLabel(tr("终止时间"), this);
    hbox->addWidget(label);
    mEndDateTimeEdit = new QDateTimeEdit(now, this);
    mEndDateTimeEdit->setDisplayFormat(format);
    hbox->addWidget(mEndDateTimeEdit);
    hbox->addStretch();
    QPushButton* btnQuery = new QPushButton(tr("查询"), this);
    connect(btnQuery, SIGNAL(clicked()), this, SLOT(onBtnQueryClicked()));
    hbox->addWidget(btnQuery);
    QPushButton* btnEmergencyQuery = new QPushButton(tr("紧急呼叫查询"), this);
    connect(btnEmergencyQuery, SIGNAL(clicked()), this, SLOT(onBtnEmergencyQueryClicked()));
    hbox->addWidget(btnEmergencyQuery);
    QPushButton* btnQueryAll = new QPushButton(tr("显示所有"), this);
    connect(btnQueryAll, SIGNAL(clicked()), this, SLOT(onBtnShowAllClicked()));
    hbox->addWidget(btnQueryAll);
    hbox->addStretch();
    mFrameRootBox->addLayout(hbox);

    //全选、全否、删除
    hbox = new QHBoxLayout;
    QPushButton* btnSelectAll = new QPushButton(tr("全选"), this);
    QPushButton* btnSelectNone = new QPushButton(tr("全否"), this);
    QPushButton* btnDelete = new QPushButton(tr("删除"), this);
    QPushButton* btnExport = new QPushButton(tr("导出"), this);

    connect(btnSelectAll, SIGNAL(clicked()), this, SLOT(onBtnSelectAllClicked()));
    connect(btnSelectNone, SIGNAL(clicked()), this,SLOT(onBtnSelectNoneClicked()));
    connect(btnDelete, SIGNAL(clicked()), this, SLOT(onBtnDeleteClicked()));
    connect(btnExport, SIGNAL(clicked()), this, SLOT(onBtnExportClicked()));
    hbox->addWidget(btnSelectAll);
    hbox->addWidget(btnSelectNone);
    hbox->addWidget(btnDelete);
    hbox->addWidget(btnExport);
    mFrameRootBox->addLayout(hbox);

    //列表
#ifndef QT_NO_DEBUG
    mCdrListModel = new QStandardItemModel(0, 6, this);
#else
    mCdrListModel = new QStandardItemModel(0, 5, this);
#endif
    mCdrListModel->setHeaderData(0, Qt::Horizontal, tr(""));
    mCdrListModel->setHeaderData(1, Qt::Horizontal, tr("通话时间"));
    mCdrListModel->setHeaderData(2, Qt::Horizontal, tr("主叫"));
    mCdrListModel->setHeaderData(3, Qt::Horizontal, tr("被叫"));
    mCdrListModel->setHeaderData(4, Qt::Horizontal, tr("时长(秒)"));
#ifndef QT_NO_DEBUG
    mCdrListModel->setHeaderData(5, Qt::Horizontal, tr("userfield"));
#endif
    QTreeView* cdrList = new QTreeView(this);
    cdrList->setRootIsDecorated(false);
    cdrList->setAlternatingRowColors(true);
    cdrList->setModel(mCdrListModel);
    cdrList->setColumnWidth(0, 30);
    cdrList->setColumnWidth(1, 150);
    cdrList->setColumnWidth(2, 150);
    cdrList->setColumnWidth(3, 150);
    cdrList->setColumnWidth(4, 150);
    connect(cdrList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onCdrViewClicked(const QModelIndex&)));
    connect(cdrList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onCdrViewDoubleClicked(QModelIndex)));
    cdrList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFrameRootBox->addWidget(cdrList);

    mSelectedStartTime.setDate(Config::Instance()->InvalidDate);
    mSelectedEndTime.setDate(Config::Instance()->InvalidDate);
}

void CallLogWidget::onResume(QMap<QString, QVariant>& param)
{
    mEndDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    BaseWidget::onResume(param);
}

void CallLogWidget::onPause(QMap<QString, QVariant> &param)
{
    foreach (QProcess* player, mPlayers)
        delete player;
    mPlayers.clear();
    BaseWidget::onPause(param);
}

void CallLogWidget::onBtnQueryClicked()
{
    mSelectedStartTime = mStartDateTimeEdit->dateTime();
    mSelectedEndTime = mEndDateTimeEdit->dateTime();
    reloadCdrList();
}

void CallLogWidget::onBtnEmergencyQueryClicked()
{
    mSelectedStartTime = QDateTime(Config::Instance()->InvalidDate);
    mSelectedEndTime = QDateTime(Config::Instance()->InvalidDate);
    reloadCdrList(true);
}

void CallLogWidget::onBtnShowAllClicked()
{
    mSelectedStartTime = QDateTime(Config::Instance()->InvalidDate);
    mSelectedEndTime = QDateTime(Config::Instance()->InvalidDate);
    reloadCdrList();
}

void CallLogWidget::onBtnSelectAllClicked()
{
    for (int i=0;i<mCdrListModel->rowCount();i++)
        mCdrListModel->setData(mCdrListModel->index(i, 0), true, Qt::CheckStateRole);
}

void CallLogWidget::onBtnSelectNoneClicked()
{
    for (int i=0;i<mCdrListModel->rowCount();i++)
        mCdrListModel->setData(mCdrListModel->index(i, 0), false, Qt::CheckStateRole);
}

void CallLogWidget::onBtnDeleteClicked()
{
    /*
    //删除选定的cdr记录
    QList<int> rowIds;
    for (int i=0;i<mCdrListModel->rowCount();i++) {
        bool isSelected = mCdrListModel->data(mCdrListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (isSelected)
            rowIds.append(i);
    }
    QStringList cdrIds;
    foreach (int rowId, rowIds) {
        QString id = mCdrListModel->data(mCdrListModel->index(rowId, 0), Qt::UserRole).toString();
        //TODO: 删除前，如果正在播放该cdr的录音，则先停止播放?
        cdrIds.append(id);
    }
    RPCCommand::delCDR(cdrIds);
    */
    QStringList cdrIds;
#ifdef EEPBX
    QStringList userFields;
#endif
    for (int i=0;i<mCdrListModel->rowCount();i++) {
        bool selected = mCdrListModel->data(mCdrListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (selected) {
            QString id = mCdrListModel->data(mCdrListModel->index(i, 0), Qt::UserRole).toString();
            cdrIds.append(id);
#ifdef EEPBX
            QString userField = mCdrListModel->data(mCdrListModel->index(i, 1), Qt::UserRole).toString();
            userFields.append(userField);
#endif
        }
    }
    RPCCommand::delCDR(cdrIds);
#ifdef EEPBX
    QString basePath = Config::Instance()->PbxMonitorFilePath;
    QString filesToBeRemoved = "";
    for (int i=0;i<userFields.length();i++) {
        if (i % 20 == 0) {
            //一次删除20个文件
            RPCCommand::removePath(filesToBeRemoved);
            filesToBeRemoved = "";
        }
        filesToBeRemoved = filesToBeRemoved + QString("%1%2.* ").arg(basePath).arg(userFields[i]);
    }
    if (filesToBeRemoved != "")
        RPCCommand::removePath(filesToBeRemoved);
#endif
    reloadCdrList();
}

void CallLogWidget::onCdrViewClicked(const QModelIndex &index)
{
    //选中、取消选中
    if (index.column() == 0) {
        bool currentValue = mCdrListModel->data(index, Qt::CheckStateRole).toBool();
        mCdrListModel->setData(index, !currentValue, Qt::CheckStateRole);
        return;
    }
}

void CallLogWidget::onCdrViewDoubleClicked(const QModelIndex index)
{
    QString file = mCdrListModel->data(mCdrListModel->index(index.row(), 1), Qt::UserRole).toString();
    if (file == "")
        return;
    static QString hostIp = "";
    if (hostIp == "")
        hostIp = RPCCommand::getIp();
#ifndef EEPBX
    Config* cfg = Config::Instance();
#endif
    //#ifndef QT_NO_DEBUG
    //    hostIp = cfg->mq_host;
    //#endif
#ifndef EEPBX
    //    QString arg = QString("ftp://%1:%2@%3:%4/var/spool/asterisk/monitor/%5.wav")
    QString arg = QString("ftp://%1:%2@%3:%4%5%6.wav")
            .arg(cfg->TelnetUsername).arg(cfg->TelnetPassword).arg(hostIp).arg(cfg->FtpPort).arg(cfg->PbxMonitorFilePath).arg(file);
#else
    QString suffix = "WAV";
    if (file.startsWith("M") || file.startsWith("B"))
        suffix = "wav";
    QString arg = QString("http://%1/monitor/%2.%3")
            .arg(hostIp).arg(file).arg(suffix);
#endif
    LOG(Logger::Debug, "playing back: %s\n", arg.toStdString().c_str());
    //QString cmd = "%ProgramFiles%\Windows Media Player\wmplayer.exe";
    QString cmd = "C:/Program Files/Windows Media Player/wmplayer.exe";
    QStringList args;
    args.append(arg);
    QProcess* player = new QProcess(this);
    player->start(cmd, args);
    mPlayers.append(player);
    //cfg->cmdProcessor()->addJob(cmd, args, true);
    /*
    //如果点击的是有录音文件行对应的caller或callee，则播放对应的声音文件
    {
        int col = index.column();
        if (col == 3 || col == 4) {
            QString sndFile = mCdrListModel->data(index, Qt::UserRole).toString();
            if (sndFile != "" && QSound::isAvailable()) {
                mPlayer = new QSound(sndFile, this);
                mPlayer->play();
            }
        }
    }
    */
}

void CallLogWidget::reloadCdrList(bool isEmergencyCall)
{
    QDateTime starttime = mSelectedStartTime;
    QDateTime endtime = mSelectedEndTime;
    mCdrListModel->removeRows(0, mCdrListModel->rowCount());
    if (mSelectedStartTime.toTime_t() == QDateTime(Config::Instance()->InvalidDate).toTime_t()
            || mSelectedEndTime.toTime_t() == QDateTime(Config::Instance()->InvalidDate).toTime_t())
        endtime = endtime.addYears(1000);
    QList<PBX::CDR> cdrs = RPCCommand::getCDR(starttime, endtime);
    QString emergencyNumber = Config::Instance()->EmergencyNumber;
    for (int i=0;i<cdrs.size();i++) {
        PBX::CDR cdr = cdrs.at(i);
        if(isEmergencyCall && !cdr.src.startsWith(emergencyNumber)){
            continue;
        }
        mCdrListModel->insertRow(0);
        mCdrListModel->setData(mCdrListModel->index(0, 0), cdr.id, Qt::UserRole);
        mCdrListModel->setData(mCdrListModel->index(0, 0), false, Qt::CheckStateRole);
        mCdrListModel->setData(mCdrListModel->index(0, 1), cdr.calldate);
        mCdrListModel->setData(mCdrListModel->index(0, 1), cdr.userField, Qt::UserRole);
        mCdrListModel->setData(mCdrListModel->index(0, 2), cdr.src);
        mCdrListModel->setData(mCdrListModel->index(0, 3), cdr.dst);
        mCdrListModel->setData(mCdrListModel->index(0, 4), cdr.duration);
#ifndef QT_NO_DEBUG
        mCdrListModel->setData(mCdrListModel->index(0, 5), cdr.userField);
#endif
    }
}


void CallLogWidget::onBtnExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel file"), qApp->applicationDirPath (),
                                                    tr("Files (*.csv)"));

    if (fileName.isEmpty())
        return;
    //打开.csv文件
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this, tr("文件打开失败"), tr("查看一下文件%1！").arg(fileName), tr("确定"));
        return;
    }

    QTextStream out(&file);
    //获取数据
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    out << "\n," << tr("导出时间：,") << current_date << ",\n" << ",\n";
    out << tr("id,") << tr("通话时间,")<< tr("主叫,") << tr("被叫,") << tr("时长,") <<",\n";//表头


    for (int i=0;i<mCdrListModel->rowCount();i++)
    {
        bool selected = mCdrListModel->data(mCdrListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (selected)
        {
            QString id = mCdrListModel->data(mCdrListModel->index(i, 0), Qt::UserRole).toString();
            QString date = mCdrListModel->data(mCdrListModel->index(i, 1)).toString();
            QString calling = mCdrListModel->data(mCdrListModel->index(i, 2)).toString();
            QString called = mCdrListModel->data(mCdrListModel->index(i, 3)).toString();
            QString time_length = mCdrListModel->data(mCdrListModel->index(i, 4)).toString();
            out << id << "," << date << "," << calling << "," << called << "," << time_length << ",\n";
        }
    }
    file.close();
    QMessageBox::information(this, tr("导出数据成功"), tr("信息已保存在%1！").arg(fileName), tr("确定"));

}

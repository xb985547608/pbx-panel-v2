#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "RecordingFileZone.h"
#include "ui/base/misccomponents.h"
#include "pbx/EventModel.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/config.h"

RecordingFileZone::RecordingFileZone(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    rootBox->addWidget(MiscComponents::getSectionLabel(tr("录音文件处理"), this));

    rootBox->addWidget(getFileListTree());

    QHBoxLayout* footerLayout = new QHBoxLayout;
    footerLayout->setMargin(0);
    footerLayout->setSpacing(10);
    footerLayout->addStretch();
    QPushButton* btnUpload = new QPushButton(tr("上传录音文件"), this);
    connect(btnUpload, SIGNAL(clicked()), this, SLOT(onBtnUploadClicked()));
    QPushButton* btnDeleteSelected = new QPushButton(tr("删除选中的录音文件"), this);
    connect(btnDeleteSelected, SIGNAL(clicked()), this, SLOT(onBtnDeleteClicked()));
    footerLayout->addWidget(btnUpload);
    footerLayout->addWidget(btnDeleteSelected);
    footerLayout->addStretch();
    rootBox->addLayout(footerLayout);
    mFtpClient = Config::Instance()->ftpClient()->getQFtp();
    connect(mFtpClient, SIGNAL(commandFinished(int,bool)), this, SLOT(onFtpCommand()));
    reloadList();
}

QTreeView* RecordingFileZone::getFileListTree()
{
    //列表
    mRecordingFileListModel = new QStandardItemModel(0, 2, this);
    QTreeView* fileList = new QTreeView(this);
    fileList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    fileList->setFocusPolicy(Qt::NoFocus);
    fileList->setRootIsDecorated(false);
    fileList->setAlternatingRowColors(true);
    fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileList->setHeaderHidden(true);
    fileList->setStyleSheet(
                "QTreeView::item {height:50px; show-decoration-selected:0; font: 75 12pt;}"
                "QTreeView::item::selected {color: green;}"
                );
    /*
    fileList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; background: #32CC99; width: 30px; }"
                );
                */
    fileList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; width: 30px; }"
                );
    fileList->setModel(mRecordingFileListModel);
    fileList->setColumnWidth(0, 30);
    fileList->setColumnWidth(1, 150);
    connect(fileList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onFileListViewClicked(const QModelIndex&)));
    fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return fileList;
}

void RecordingFileZone::reloadList()
{
    mRecordingFileListModel->removeRows(0, mRecordingFileListModel->rowCount());
    QStringList files = RPCCommand::getBroadcastFileList();

    foreach (QString file, files) {
        mRecordingFileListModel->insertRow(0);
        //mCdrListModel->setData(mCdrListModel->index(0, 0), cdr.uniqueId, Qt::UserRole);
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), false, Qt::CheckStateRole);
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 1), FTPClient::_fromSpecialEncoding(file));
        mRecordingFileListModel->setData(mRecordingFileListModel->index(0, 0), file, Qt::UserRole);
        /*
        mCdrListModel->setData(mCdrListModel->index(0, 3), sndFile, Qt::UserRole);
        mCdrListModel->setData(mCdrListModel->index(0, 3), QColor(Qt::yellow), Qt::BackgroundColorRole);
        */
    }
}

void RecordingFileZone::onBtnUploadClicked()
{
#ifdef PBX_NO_DEBUG
    QString selectedFile = QFileDialog::getOpenFileName(this, tr("请选择扩音文件"), "", "sound file(*.wav *.gsm)");
#else
    QString selectedFile = QFileDialog::getOpenFileName(this, tr("请选择扩音文件"), "", "");
#endif
    if (selectedFile == "")
        return;
    QString fileName = selectedFile.right(selectedFile.size() - (selectedFile.lastIndexOf("/") + 1));
    LOG(Logger::Debug, "selected file: %s\n", fileName.toStdString().c_str());
    Config* cfg = Config::Instance();
    QStringList cmdAndArgs;
    cfg->ftpClient()->addJob(FTPClient::Upload, selectedFile, cfg->PbxSoundFilePath, fileName, cmdAndArgs);
    //reloadList();
}

void RecordingFileZone::onBtnDeleteClicked()
{
    //删除选定的文件
    QStringList files;
    for (int i=0;i<mRecordingFileListModel->rowCount();i++) {
        bool checked = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::CheckStateRole).toBool();
        if (checked) {
            //QString file = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 1)).toString();
            QString file = mRecordingFileListModel->data(mRecordingFileListModel->index(i, 0), Qt::UserRole).toString();
            files.append(file);
            LOG(Logger::Debug, "%s is selected to delete\n", file.toStdString().c_str());
        }
    }
    RPCCommand::delBroadcastFile(files);
    reloadList();
}

void RecordingFileZone::onFileListViewClicked(const QModelIndex &index)
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

void RecordingFileZone::onFtpCommand()
{
    if (mFtpClient->currentCommand() == QFtp::Put)
        reloadList();
}

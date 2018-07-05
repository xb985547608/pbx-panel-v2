#include "PlaySoundWidget.h"
#include "misc/Config.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/ftpclient.h"
#include <QTreeView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QScrollBar>

PlaySoundWidget::PlaySoundWidget(QWidget *parent, bool showFileList) :
    QDialog(parent), mCycleCount(1), mShowFileList(showFileList)
{
    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    setMinimumSize(600, 300);
    setLayout(rootbox);

    rootbox->addWidget(MiscComponents::getSectionLabel(tr("录音播放文件选择"), this), 1); //顶

    QHBoxLayout* hbox = new QHBoxLayout; //身
    hbox->setMargin(0);
    rootbox->addLayout(hbox, 4);
    if (mShowFileList)
        hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 1);

    hbox = new QHBoxLayout; //底
    rootbox->addLayout(hbox, 1);
    hbox->setMargin(0);
    QPushButton* btnOk = new QPushButton(tr("确认"), this);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkClicked()));
    btnOk->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QPushButton* btnCancel = new QPushButton(tr("取消"), this);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(onBtnCancelClicked()));
    btnCancel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    hbox->addWidget(btnOk);
    hbox->addWidget(btnCancel);
}

QWidget* PlaySoundWidget::getLeftFrame()
{
    mFileListModel = new QStandardItemModel(0, 1, this);
    loadFiles();
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
    fileList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; width: 30px; }"
                );
    fileList->setModel(mFileListModel);
    connect(fileList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onFileListClicked(const QModelIndex&)));
    return fileList;
}

QWidget* PlaySoundWidget::getRightFrame()
{
    QWidget* rightFrame = new QWidget(this);
    rightFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLabel* title = new QLabel(tr("选择播放次数"), this);
    QPushButton* btnIncrease = new QPushButton(tr("+"), this);
    connect(btnIncrease, SIGNAL(clicked()), this, SLOT(onBtnIncreaseClicked()));
    mLbCycleCount = new QLabel(this);
    mLbCycleCount->setText(QString("%1").arg(mCycleCount));
    mLbCycleCount->setAlignment(Qt::AlignCenter);
    QPushButton* btnDecrease = new QPushButton(tr("-"), this);
    connect(btnDecrease, SIGNAL(clicked()), this, SLOT(onBtnDecreaseClicked()));
    mCkForever = new QCheckBox(tr("无限循环"), this);

    title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    btnIncrease->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mLbCycleCount->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    btnDecrease->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mCkForever->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->setMargin(0);
    rightFrame->setLayout(rootbox);
    rootbox->addWidget(title, 1);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    rootbox->addLayout(hbox, 3);
    hbox->addWidget(btnIncrease);
    hbox->addWidget(mLbCycleCount);
    hbox->addWidget(btnDecrease);

    rootbox->addWidget(mCkForever, 2);

    return rightFrame;
}

void PlaySoundWidget::loadFiles()
{
    QStringList files = RPCCommand::getBroadcastFileList();
    foreach (QString f, files) {
        mFileListModel->insertRow(0);
        mFileListModel->setData(mFileListModel->index(0, 0), FTPClient::_fromSpecialEncoding(f));
        mFileListModel->setData(mFileListModel->index(0, 0), false, Qt::CheckStateRole);
        mFileListModel->setData(mFileListModel->index(0, 0), f, Qt::UserRole);
    }
}

void PlaySoundWidget::onFileListClicked(const QModelIndex &index)
{
    mFileListModel->setData(index, true, Qt::CheckStateRole);
    int row = index.row();
    for (int i=0;i<mFileListModel->rowCount();i++) {
        if (row != i)
            mFileListModel->setData(mFileListModel->index(i, 0), false, Qt::CheckStateRole);
    }
}

void PlaySoundWidget::onBtnIncreaseClicked()
{
    if (!mCkForever->isChecked()) {
        mCycleCount++;
        setLabelCycleCount();
    }
}

void PlaySoundWidget::onBtnDecreaseClicked()
{
    if (!mCkForever->isChecked()) {
        if (mCycleCount == 1)
            return;
        mCycleCount--;
        setLabelCycleCount();
    }
}

void PlaySoundWidget::setLabelCycleCount()
{
    QString cnt = QString("%1").arg(mCycleCount);
    mLbCycleCount->setText(cnt);
}

void PlaySoundWidget::onBtnOkClicked()
{
    if (mShowFileList) {
        QString sound = getSoundFile();
        if (sound != "") {
            accept();
            return;
        }
        QMessageBox::about(this, tr("错误"), tr("请选择扩音文件！"));
    } else
        accept();
}

void PlaySoundWidget::onBtnCancelClicked()
{
    reject();
}

int PlaySoundWidget::getLoopCount()
{
    if (mCkForever->isChecked())
        return 999999;
    else
        return mCycleCount;
}

QString PlaySoundWidget::getSoundFile()
{
    QString sound;
    for (int i=0;i<mFileListModel->rowCount();i++) {
        if (mFileListModel->data(mFileListModel->index(i, 0), Qt::CheckStateRole).toBool()) {
            sound = mFileListModel->data(mFileListModel->index(i, 0)).toString();
            //sound = mFileListModel->data(mFileListModel->index(i, 0), Qt::UserRole).toString();
            break;
        }
    }
    return sound;
}

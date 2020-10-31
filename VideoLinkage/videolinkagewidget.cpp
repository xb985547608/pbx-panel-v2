#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "videolinkagewidget.h"
#include <QPushButton>
#include <QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>
#include <QStringList>
#include <QMessageBox>
#include <QGridLayout>
#include <QComboBox>
#include <QSizePolicy>

#include "../pbx/pbx.h"
#include "../misc/logger.h"

VideoLinkageWidget::VideoLinkageWidget(QWidget *parent) :
    QWidget(parent)
{
    /* 实例视频窗口 */
    for (int i=0; i<VIDEOWIDGET_MAX_LENGTH; i++) {
        VideoWidget *vw = new VideoWidget(this);
        vw->setVisible(false);
        vw->installEventFilter(this);
        connect(vw, &VideoWidget::doubleClicked, this, &VideoLinkageWidget::fullScreen);
        mVideoWidgets.append(vw);
    }
    /* top-right */
    mpVideoWidgetsZone = new QFrame(this);
    mpVideoWidgetsZone->setObjectName("zone");
    mpVideoWidgetsZone->setStyleSheet("QFrame#zone{background-color:black}");
    mpVideoWidgetsZone->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpGridLayout = new QGridLayout();
    mpGridLayout->setSpacing(0);
    mpGridLayout->setContentsMargins(0, 0, 0, 0);
    mpVideoWidgetsZone->setLayout(mpGridLayout);

    /* bottom-right */
    QFrame *operatorFrame = new QFrame(this);
    operatorFrame->setFixedHeight(50);
    operatorFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    operatorFrame->setLineWidth(2);
    {
        QHBoxLayout *row = new QHBoxLayout();
        operatorFrame->setLayout(row);
        mpLayoutSelectCombo = new QComboBox(operatorFrame);
        mpLayoutSelectCombo->addItems(QStringList()
                                      << "1X1"
                                      << "2X2"
                                      << "3X3"
                                      << "4x4");
        mpLayoutSelectCombo->setFixedWidth(80);
        QPushButton *settingsBtn = new QPushButton("设置", operatorFrame);
        settingsBtn->setFixedWidth(40);

        row->addStretch();
        row->addWidget(mpLayoutSelectCombo);
        row->addWidget(settingsBtn);

        mpSettingsDialog = new SettingsDialog(this);
        mOperatorGroupChannel = mpSettingsDialog->getOperatorGroupChannel();
        connect(settingsBtn, &QPushButton::clicked, mpSettingsDialog, &SettingsDialog::exec);
    }

    /* left */
    mpDeviceList = new DeviceList(this);
    mpDeviceList->setFixedWidth(150);

    /* merge */
    QVBoxLayout *col = new QVBoxLayout();
    col->addWidget(mpVideoWidgetsZone);
    col->addWidget(operatorFrame);
    QHBoxLayout *row = new QHBoxLayout(this);
    row->addWidget(mpDeviceList);
    row->addLayout(col);

    /* connect */
    connect(mpLayoutSelectCombo, &QComboBox::currentTextChanged, this, &VideoLinkageWidget::changeLayout);
    connect(mpDeviceList, &DeviceList::extenStateChanged, this,
            static_cast<void(VideoLinkageWidget::*)(const QString &, const QString &, bool)>(&VideoLinkageWidget::extenStateChanged));
    connect(mpDeviceList, &DeviceList::closeAllSignal, this, &VideoLinkageWidget::closeAll);
    connect(this, &VideoLinkageWidget::extenStateChangedSignal, mpDeviceList,
            static_cast<void(DeviceList::*)(const QString &, bool)>(&DeviceList::changeState));
    connect(mpSettingsDialog, &SettingsDialog::extenStreamChanged, this, &VideoLinkageWidget::extenStreamChanged);
    connect(mpSettingsDialog, &SettingsDialog::extenStreamRemoved, this, &VideoLinkageWidget::extenStreamRemoved);
    connect(mpSettingsDialog, &SettingsDialog::extenStreamChanged, mpDeviceList,
            static_cast<void(DeviceList::*)(const QString &, const QStringList &)>(&DeviceList::changeState));
    connect(mpSettingsDialog, &SettingsDialog::extenStreamRemoved, mpDeviceList, &DeviceList::removeExten);

    /* 刷新视频窗口的时间标签 */
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        foreach (VideoWidget *vw, mVideoWidgets) {
            vw->setTimeHint(QDateTime::currentDateTime());
        }
    });
    timer->start(1000);

    mpLayoutSelectCombo->setCurrentIndex(0);
    mpLayoutSelectCombo->setCurrentIndex(3);
    mpSettingsDialog->init();
    resize(800, 600);
}

bool VideoLinkageWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (mFullScreen && event->type() == QEvent::KeyPress) {

        VideoWidget *w = dynamic_cast<VideoWidget *>(watched);
        if (w == NULL || !w->isPlaying())
            return false;

        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);
        if (e->key() == Qt::Key_Escape) {
            emit w->doubleClicked();
        }
    }
    return true;
}

void VideoLinkageWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void VideoLinkageWidget::closeEvent(QCloseEvent *event)
{
    QMessageBox::about(this, "提示", "无法关闭视频联动窗口");
    event->ignore();
}

void VideoLinkageWidget::changeLayout(const QString &text)
{
    (void)text;
    int row = mpLayoutSelectCombo->currentIndex() + 1;
    int col = row;
    QList<VideoWidget *> sortVideoWidgets;

    /* 对窗口列表进行排序，被占用的窗口放在前面 */
    QList<VideoWidget *> temps = mVideoWidgets;
    for (int i=0; i<VIDEOWIDGET_MAX_LENGTH; i++) {
        VideoWidget *vw = temps.takeLast();
        if (vw->isPlaying())
            sortVideoWidgets.prepend(vw);
        else
            sortVideoWidgets.append(vw);
    }

    /* 清空布局中的窗口 */
    while (mpGridLayout->takeAt(0) != NULL);

    int index = 0;
    for (int i=0; i<row; i++) {
        for (int j=0; j<col; j++) {
            sortVideoWidgets.at(index)->show();
            mpGridLayout->addWidget(sortVideoWidgets.at(index), i, j);
            index++;
        }
    }

    for (; index<VIDEOWIDGET_MAX_LENGTH; index++)
        sortVideoWidgets.at(index)->hide();

    /* 保存从新排序后的视频窗口列表 */
    mVideoWidgets = sortVideoWidgets;
}

void VideoLinkageWidget::fullScreen()
{
    VideoWidget *w = dynamic_cast<VideoWidget *>(sender());
    if (w == NULL || !w->isPlaying())
        return;

    if (w->isFullScreen()) {
        w->setWindowFlags(Qt::SubWindow);
        w->showNormal();
        mFullScreen = false;
    } else {
        w->setWindowFlags(Qt::Window);
        w->showFullScreen();
        mFullScreen = true;
    }
}

void VideoLinkageWidget::open(const QMap<QString, QStringList> &extensions)
{
    if (extensions.isEmpty())
        return;

    int count = 0;
    foreach (QStringList urls, extensions.values())
        count += urls.count();

    int index = 0;
    /* 跳过已在播放的视频窗口 */
    while (mVideoWidgets.at(index)->isPlaying()) {
        index++;
        if (index >= VIDEOWIDGET_MAX_LENGTH)
            break;
    }

    /* 重新布局和排序 */
    int sum = count + index;
    if (sum == 0) {
        return;
    } else if (sum <= 1) {
        mpLayoutSelectCombo->setCurrentIndex(0);
    } else if (sum <= 4) {
        mpLayoutSelectCombo->setCurrentIndex(1);
    } else if (sum <= 9) {
        mpLayoutSelectCombo->setCurrentIndex(2);
    } else {
        mpLayoutSelectCombo->setCurrentIndex(3);
        index = qBound(0, index, VIDEOWIDGET_MAX_LENGTH - count);
    }

    foreach (QString e, extensions.keys()) {
        if (index >= VIDEOWIDGET_MAX_LENGTH)
            break;
        QStringList urls = extensions.value(e);
        foreach (QString url, urls) {
            if (mVideoWidgets.at(index)->isPlaying()) {
                QString e = mVideoWidgets.at(index)->ExtensionNum();
                e = e.left(e.indexOf('/'));
                mWaitPlayList.append(QPair<QString, QString>(e, mVideoWidgets.at(index)->url()));
            }
            mVideoWidgets.at(index)->setInfo(QString("%1/视频%2")
                                             .arg(e)
                                             .arg(mpSettingsDialog->value(e).indexOf(url) + 1), url);
            index++;
        }
    }

    activateWindow();
}

void VideoLinkageWidget::close(const QMap<QString, QStringList> &extensions)
{
    if (extensions.isEmpty())
        return;

    bool isNeedLayout = false;
    int idleCount = 0;

    foreach (QString e, extensions.keys()) {
        foreach (VideoWidget *vw, mVideoWidgets) {
            foreach (QString url, extensions.value(e)) {
                if (vw->ExtensionNum().indexOf(e) != -1 && url == vw->url()) {
                    vw->reset();
                    idleCount++;
                    isNeedLayout = true;

                    qDebug() << __FUNCTION__ << "- close url:" << url << e;
                }
            }
        }
    }

    qDebug() << extensions;

    if (isNeedLayout)
        changeLayout(mpLayoutSelectCombo->currentText());

    if (idleCount && mWaitPlayList.size()) {
        QMap<QString, QStringList> prepareOpen;
        while (idleCount) {
            QPair<QString, QString> item = mWaitPlayList.takeFirst();

            if (prepareOpen.keys().contains(item.first)) {
                prepareOpen[item.first] << item.second;
            } else {
                prepareOpen.insert(item.first, QStringList() << item.second);
            }

            idleCount--;
        }

        qDebug() << __FUNCTION__ << "- open:" << prepareOpen;
        open(prepareOpen);
    }
}

void VideoLinkageWidget::closeAll()
{
    foreach (VideoWidget *vw, mVideoWidgets) {
        vw->reset();
    }

    foreach (QString e, mExtenStreamStatus.keys()) {
        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
        foreach (QString url, streamStatus.keys()) {
            if (streamStatus.value(url)) {
                streamStatus[url] = false;
            }
        }
    }
}

void VideoLinkageWidget::extenStateChanged(const QString &numer, const QString &url, bool isPlay)
{
    QMap<QString, bool> &streamStatus = mExtenStreamStatus[numer];
    if (streamStatus.value(url) == isPlay)
        return;

    streamStatus[url] = isPlay;

    QMap<QString, QStringList> prepare;
    prepare.insert(numer, QStringList() << url);

    if (isPlay)
        open(prepare);
    else
        close(prepare);

    qDebug() << __FUNCTION__ << (isPlay ? "- open" : "- close") << prepare;
}

void VideoLinkageWidget::extenStreamChanged(QString e, QStringList urls)
{
    if (urls.isEmpty())
        return;

    if (mExtenStreamStatus.keys().contains(e)) {
        QMap<QString, QStringList> es;
        es.insert(e, mExtenStreamStatus.value(e).keys());
        close(es);
    }

    QMap<QString, bool> status;
    foreach (QString url, urls)
        status.insert(url, false);
    mExtenStreamStatus.insert(e, status);
}

void VideoLinkageWidget::extenStreamRemoved(QString e)
{
    if (!mExtenStreamStatus.keys().contains(e))
        return;

    QMap<QString, QStringList> es;
    es.insert(e, mExtenStreamStatus.value(e).keys());
    close(es);

    mExtenStreamStatus.remove(e);
}

void VideoLinkageWidget::operatorExtensChanged(QStringList extens)
{
    mOperatorExtens = extens;
    mOperatorCallExtens.clear();
}

/* 处理调度员呼叫分机的情况 */
void VideoLinkageWidget::operatorExtenStateChanged(QString number)
{
    if (!mOperatorExtens.contains(number))
        return;

    QStringList uncallExtens;
    QMap<QString, QStringList> prepareClose;

    QMap<QString, PBX::Extension> opexten = PBX::Instance()->getExtensionDetail(number);
    if (opexten[number].getState() > PBX::eIdle)
        return;

    foreach (QString e, mOperatorCallExtens) {
        QMap<QString, PBX::Extension> exten = PBX::Instance()->getExtensionDetail(e);

        if (exten[e].getState() <= PBX::eIdle)
            uncallExtens.append(e);
    }

    foreach (QString e, uncallExtens) {
        mOperatorCallExtens.removeAll(e);
        if (!mExtenStreamStatus.keys().contains(e))
            continue;

        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
        QStringList urls;
        foreach (QString url, streamStatus.keys()) {
            if (streamStatus.value(url)) {
                streamStatus[url] = false;
                urls.append(url);
                emit extenStateChangedSignal(e, false);
            }
        }
        prepareClose.insert(e, urls);
    }

//    close(prepareClose);

    qDebug() << __FUNCTION__ << "- close:" << prepareClose.keys() << "->" << number << opexten[number].getState();

    LOG(Logger::Debug, "close number: %s, extens: %s\n", prepareClose.keys().join(',').toLocal8Bit().data(),
        mOperatorCallExtens.join(',').toLocal8Bit().data());
}

/* 处理分机呼叫调度员振铃组的情况 */
void VideoLinkageWidget::extenStateChanged(QString e)
{
    if (!mExtenStreamStatus.keys().contains(e))
        return;

    QMap<QString, QStringList> prepareOpen;
    QMap<QString, QStringList> prepareClose;

    QMap<QString, PBX::Extension> exten = PBX::Instance()->getExtensionDetail(e);
    if (!exten.contains(e))
        return;

    LOG(Logger::Debug, "extenStateChanged %s %d\n", e.toLocal8Bit().data(), exten[e].getState());

    bool isPasv = false;  //调度员所在的振铃组是否被呼
    bool isBroadcast = false;

    if (exten[e].getState() > PBX::eIdle) {
        foreach (PBX::PeerInfo peer, exten.value(e).peers.values()) {
            if (peer.state == PBX::eIncall) {
                /* 如果分机呼叫的是调度员所在的振铃组号码 */
                if (peer.number == mOperatorGroupChannel) {
                    isPasv = true;
                    if (!mPasvExtenNumber.contains(e)) {
                        mPasvExtenNumber.append(e);

                        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
                        QStringList urls;
                        foreach (QString url, streamStatus.keys()) {
                            if (!streamStatus.value(url)) {
                                streamStatus[url] = true;
                                urls.append(url);
                            }
                        }
                        emit extenStateChangedSignal(e, true);
                        prepareOpen.insert(e, urls);
                    }
                } else if (peer.number.isEmpty() && peer.name.isEmpty()) {
                    isBroadcast = true;
                    if (!mBroadcastExtenNumber.contains(e)) {
                        mBroadcastExtenNumber.append(e);

                        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
                        QStringList urls;
                        foreach (QString url, streamStatus.keys()) {
                            if (!streamStatus.value(url)) {
                                streamStatus[url] = true;
                                urls.append(url);
                            }
                        }
                        emit extenStateChangedSignal(e, true);
                        prepareOpen.insert(e, urls);
                    }
                }
            }
        }
    } else {
        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
        QStringList urls;
        foreach (QString url, streamStatus.keys()) {
            streamStatus[url] = false;
            urls.append(url);
        }

        if (!isPasv && mPasvExtenNumber.contains(e))
            mPasvExtenNumber.removeAll(e);

        if (!isBroadcast && mBroadcastExtenNumber.contains(e))
            mBroadcastExtenNumber.removeAll(e);

        emit extenStateChangedSignal(e, false);
        prepareClose.insert(e, urls);
    }

    open(prepareOpen);
    close(prepareClose);

    qDebug() << __FUNCTION__ << "- open:" << prepareOpen << "->" << e << exten[e].getState();
    qDebug() << __FUNCTION__ << "- close:" << prepareClose << "->" << e << exten[e].getState();

    LOG(Logger::Debug, "open number: %s\n", prepareOpen.keys().join(',').toLocal8Bit().data());
    LOG(Logger::Debug, "close number: %s\n", prepareClose.keys().join(',').toLocal8Bit().data());
}

void VideoLinkageWidget::callExtens(QStringList extens)
{    
    foreach (QString oe, mOperatorExtens)
        extens.removeAll(oe);

    if (extens.isEmpty())
        return;

    QMap<QString, QStringList> prepareOpen;

    foreach (QString e, extens) {
        if (!mExtenStreamStatus.keys().contains(e))
            continue;


        if (!mOperatorCallExtens.contains(e))
            mOperatorCallExtens.append(e);
        QMap<QString, bool> &streamStatus = mExtenStreamStatus[e];
        QStringList urls;
        foreach (QString url, streamStatus.keys()) {
            if (!streamStatus.value(url)) {
                streamStatus[url] = true;
                urls.append(url);
            }
        }
        emit extenStateChangedSignal(e, true);
        prepareOpen.insert(e, urls);
    }

    open(prepareOpen);

    qDebug() << __FUNCTION__ << "- open:" << prepareOpen.keys() << "->" << extens;
    LOG(Logger::Debug, "open number: %s, extens: %s\n", prepareOpen.keys().join(',').toLocal8Bit().data(),
        mOperatorCallExtens.join(',').toLocal8Bit().data());
}


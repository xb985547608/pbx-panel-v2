#include "videolinkagewidget.h"
#include <QPushButton>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include <QMouseEvent>

#include "../pbx/pbx.h"
#include "../misc/logger.h"

VideoLinkageWidget::VideoLinkageWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                   Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    /* 实例9个视频窗口 */
    for (int i=0; i<9; i++) {
        VideoWidget *vw = new VideoWidget(this);
        vw->setVisible(false);
        vw->installEventFilter(this);
        connect(vw, &VideoWidget::doubleClicked, this, &VideoLinkageWidget::fullWidget);
        mVideoWidgets.append(vw);
    }
    /* top */
    mpVideoWidgetsZone = new QWidget(this);
    mpVideoWidgetsZone->setObjectName("zone");
    mpVideoWidgetsZone->setStyleSheet("QWidget#zone{background-color:green}");
    mpGridLayout = new QGridLayout();
    mpGridLayout->setSpacing(2);
    mpGridLayout->setContentsMargins(2, 2, 2, 2);
    mpVideoWidgetsZone->setLayout(mpGridLayout);

    /* bottom */
    QFrame *operatorFrame = new QFrame(this);
    operatorFrame->setFixedHeight(50);
    operatorFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    operatorFrame->setLineWidth(2);
    QHBoxLayout *row = new QHBoxLayout();
    operatorFrame->setLayout(row);
    mpLayoutSelectCombo = new QComboBox(operatorFrame);
    mpLayoutSelectCombo->addItems(QStringList()
                                  << "1X1"
                                  << "2X2"
                                  << "3X3");
    mpLayoutSelectCombo->setFixedWidth(80);
    QPushButton *settingsBtn = new QPushButton("设置", operatorFrame);
    settingsBtn->setFixedWidth(40);

    row->addStretch();
    row->addWidget(mpLayoutSelectCombo);
    row->addWidget(settingsBtn);

    /* merge */
    QVBoxLayout *col = new QVBoxLayout();
    setLayout(col);
    col->addWidget(mpVideoWidgetsZone);
    col->addWidget(operatorFrame);

    /* dialog */
    mpSettingsDialog = new SettingsDialog(this);

    /* connect */
    connect(mpLayoutSelectCombo, &QComboBox::currentTextChanged,
            this, &VideoLinkageWidget::changeLayout);
    connect(settingsBtn, &QPushButton::clicked, mpSettingsDialog, &SettingsDialog::exec);

    mpLayoutSelectCombo->setCurrentText("3X3");

    resize(800, 600);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        foreach (VideoWidget *vw, mVideoWidgets) {
            vw->setTimeHint(QDateTime::currentDateTime());
        }
    });
    timer->start(1000);
}

void VideoLinkageWidget::keyReleaseEvent(QKeyEvent *event)
{
    /* 退出全屏 */
    if (event->key() == Qt::Key_Escape &&
            !mpLayoutSelectCombo->isEnabled()) {
        mpLayoutSelectCombo->setEnabled(true);
        changeLayout(mpLayoutSelectCombo->currentText());
    }
}

void VideoLinkageWidget::changeLayout(const QString &text)
{
    int row = text.left(1).toInt();
    int col = text.right(1).toInt();
    QList<VideoWidget *> sortVideoWidgets;
    int length = mVideoWidgets.count();

    /* 对窗口列表进行排序，被占用的窗口放在前面 */
    QList<VideoWidget *> temps = mVideoWidgets;
    for (int i=0; i<length; i++) {
        VideoWidget *vw = temps.takeLast();
        if (vw->isBusy())
            sortVideoWidgets.prepend(vw);
        else
            sortVideoWidgets.append(vw);
    }

    /* 清空布局中的窗口 */
    while (mpGridLayout->takeAt(0) != NULL);

    int index = 0;
    for (int i=0; i<row; i++) {
        for (int j=0; j<col; j++) {
            sortVideoWidgets.at(index)->setVisible(true);
            mpGridLayout->addWidget(sortVideoWidgets.at(index), i, j);
            index++;
        }
    }

    for (; index<length; index++)
        sortVideoWidgets.at(index)->setVisible(false);

    /* 保存从新排序后的视频窗口列表 */
    mVideoWidgets = sortVideoWidgets;
}

void VideoLinkageWidget::fullWidget()
{
    VideoWidget *w = dynamic_cast<VideoWidget *>(sender());
    if (w == NULL || !w->isBusy())
        return;

    /* 隐藏双击窗口之外的其余窗口 */
    foreach (VideoWidget *vw, mVideoWidgets)
        vw->setVisible(vw == sender());

    if (w != NULL)
        mpLayoutSelectCombo->setEnabled(false);
}

void VideoLinkageWidget::open(QStringList extensions)
{
    if (extensions.isEmpty())
        return;

    QMap<QString, QStringList> items;
    int count = 0;

    foreach (QString key, extensions) {
        QStringList urls = mpSettingsDialog->value(key);
        if (mpSettingsDialog->contains(key) && !urls.isEmpty()) {
            items.insert(key, urls);
            count += urls.count();
        }
    }

    int index = 0;
    /* 排除被占用的视频窗口 */
    while (mVideoWidgets.at(index)->isBusy())
        index++;
    if (index >= 9)
        return;

    if (count != 0)
        mpLayoutSelectCombo->setEnabled(true);

    /* 对其从新布局和排序 */
    count += index;
    if (count == 0) {
        return;
    } else if (count <= 1) {
        mpLayoutSelectCombo->setCurrentIndex(0);
    } else if (count <= 4) {
        mpLayoutSelectCombo->setCurrentIndex(1);
    } else {
        mpLayoutSelectCombo->setCurrentIndex(2);
    }

    foreach (QString key, items.keys()) {
        if (mAlreadyUseExten.contains(key))
            continue;

        mAlreadyUseExten.append(key);
        QStringList urls = items.value(key);
        foreach (QString url, urls) {
            mVideoWidgets.at(index)->setInfo(key, url);
            index++;
            if (index >= 9)
                break;
        }
        if (index >= 9)
            break;
    }
}

void VideoLinkageWidget::close(QStringList extensions)
{
    if (extensions.isEmpty() || mAlreadyUseExten.isEmpty())
        return;

    bool isNeedLayout = false;

    foreach (QString key, extensions) {
        mAlreadyUseExten.removeAll(key);
        foreach (VideoWidget *vw, mVideoWidgets) {
            if (vw->ExtensionNum() == key) {
                vw->reset();
                isNeedLayout = true;
            }
        }
    }

    if (isNeedLayout)
        changeLayout(mpLayoutSelectCombo->currentText());
}

#if 1
/* 处理调度员呼叫分机的情况 */
void VideoLinkageWidget::operatorExtenStateChanged(QString number)
{
    if (!mActiveExtenNumber.keys().contains(number))
        mActiveExtenNumber.insert(number, QStringList());

    QStringList openNumbers;
    QStringList closeNumbers;
    QStringList activeCallExtens;

    QMap<QString, PBX::Extension> exten = PBX::Instance()->getExtensionDetail(number);
    if (!exten.contains(number))
        return;

    qDebug() << "operatorExtenStateChanged" << number;

    if (exten.value(number).peers.count() != 0) {
        foreach (PBX::PeerInfo peer, exten.value(number).peers.values()) {
            if (peer.state == PBX::eIncall) {
                /* 如果是本地发起的呼叫，且在配置中能找到分机的信息 */
                if (peer.peerChannel.left(5) == "Local" &&
                        mpSettingsDialog->contains(peer.number)) {
                    /* 新加入的主呼分机 */
                    if (!mActiveExtenNumber[number].contains(peer.number)) {
                        mActiveExtenNumber[number].append(peer.number);
                        openNumbers.append(peer.number);
                    }
                    activeCallExtens.append(peer.number);
                }
            }
        }

        foreach (QString extenNum, mActiveExtenNumber[number]) {
            /* 发现一个退出通话的分机，关闭该分机的视频 */
            if (!activeCallExtens.contains(extenNum)) {
                mActiveExtenNumber[number].removeAll(extenNum);
                closeNumbers.append(extenNum);
            }
        }
    } else {
        closeNumbers = mActiveExtenNumber[number];
        mActiveExtenNumber.insert(number, QStringList());
    }

    open(openNumbers);
    close(closeNumbers);
}
#endif


/* 处理分机呼叫调度员振铃组的情况 */
void VideoLinkageWidget::extenStateChanged(QString number)
{
    if (!mpSettingsDialog->contains(number))
        return;

    QStringList openNumbers;
    QStringList closeNumbers;

    QMap<QString, PBX::Extension> exten = PBX::Instance()->getExtensionDetail(number);
    if (!exten.contains(number))
        return;

    LOG(Logger::Debug, "extenStateChanged %s", number.toStdString().c_str());

    bool isPasv = false;  //调度员是否被呼
    foreach (PBX::PeerInfo peer, exten.value(number).peers.values()) {
        if (peer.state == PBX::eIncall) {
            /* 如果分机呼叫的是调度员所在的振铃组号码，且在配置中能找到分机的信息 */
            if (peer.number == OPERATOR_RING_GROUNP && mpSettingsDialog->contains(number)) {
                isPasv = true;
                if (!mPasvExtenNumber.contains(number)) {
                    mPasvExtenNumber.append(number);
                    openNumbers.append(number);
                }
            }
        }
    }

    /* 如果没有呼叫调度员所在的振铃组且在mPasvExtenNumber中包含改分机号，则清理 */
    if (!isPasv && mPasvExtenNumber.contains(number)) {
        mPasvExtenNumber.removeAll(number);
        closeNumbers.append(number);
    }

    open(openNumbers);
    close(closeNumbers);
}


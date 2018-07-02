#include "videowidget.h"
#include <QFont>
#include <QResizeEvent>
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QBoxLayout>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Audio.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>

VlcInstance *VideoWidget::mspInstance = new VlcInstance(VlcCommon::args());

VideoWidget::VideoWidget(QWidget *parent) :
    QFrame(parent),
    mpMedia(NULL)
{
    setStyleSheet("background-color: black");
    /************************ top ************************/
    mpTopWidget = new QFrame(this);
    QHBoxLayout *row1 = new QHBoxLayout(mpTopWidget);
    row1->setContentsMargins(4, 0, 4, 0);
    mpExtensionLbl = new QLabel(mpTopWidget);
    mpExtensionLbl->setStyleSheet("color:white;background-color: transparent");
    mpExtensionLbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    mpTimeLbl = new QLabel(mpTopWidget);
    mpTimeLbl->setStyleSheet("color:white;background-color: transparent");
    mpTimeLbl->setAlignment(Qt::AlignRight | Qt::AlignTop);
    row1->addWidget(mpExtensionLbl);
    row1->addWidget(mpTimeLbl);

    /************************ bottom ************************/
    mpBottomWidget = new QFrame(this);
    QHBoxLayout *row2 = new QHBoxLayout(mpBottomWidget);
    mpVideoWidget = new VlcWidgetVideo(mpBottomWidget);
    mpPlayer = new VlcMediaPlayer(mspInstance);
    mpPlayer->setVideoWidget(mpVideoWidget);
    mpPlayer->audio()->setVolume(0);
    mpVideoWidget->setMediaPlayer(mpPlayer);
    row2->addWidget(mpVideoWidget);
    mpVideoWidget->installEventFilter(this);
}

void VideoWidget::setInfo(QString extensionName, QString url)
{
    if (url.isEmpty())
        return;
    mpExtensionLbl->setText(extensionName);

    if (mpMedia != NULL) {
        delete mpMedia;
        mpMedia = NULL;
    }

    mpMedia = new VlcMedia("rtsp://" + url, mspInstance);
    mpPlayer->open(mpMedia);
}

void VideoWidget::setTimeHint(const QDateTime &time)
{
    if (isBusy()) {
        mpTimeLbl->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
    }
}

void VideoWidget::reset()
{
    mpExtensionLbl->setText("");
    mpTimeLbl->setText("");

    mpPlayer->stop();
    if (mpMedia != NULL) {
        delete mpMedia;
        mpMedia = NULL;
    }
}

bool VideoWidget::isPlaying()
{
    return mpPlayer->hasVout();
}

bool VideoWidget::isBusy()
{
    return mpMedia != NULL;
}

QString VideoWidget::ExtensionNum()
{
    return mpExtensionLbl->text();
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    mpTopWidget->setGeometry(0, 0, size().width(), 16);
    mpBottomWidget->setGeometry(QRect(QPoint(0, 0), size()).adjusted(0, 16, 0, 0));
}


bool VideoWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    if (event->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    }
    return QFrame::eventFilter(watched, event);
}





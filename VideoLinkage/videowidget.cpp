#include "videowidget.h"
#include <QFont>
#include <QResizeEvent>
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QBoxLayout>

VideoWidget::VideoWidget(QWidget *parent) :
    QFrame(parent)
{
    setStyleSheet("background-color: black; border: 2px solid green");
    /************************ top ************************/
    mpExtensionLbl = new QLabel(this);
    mpExtensionLbl->setStyleSheet("color:white;background-color: transparent; border: 0px");
    mpExtensionLbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    mpTimeLbl = new QLabel(this);
    mpTimeLbl->setStyleSheet("color:white;background-color: transparent; border: 0px");
    mpTimeLbl->setAlignment(Qt::AlignRight | Qt::AlignTop);

    /************************ bottom ************************/
    mpVideoWidget = new QWidget(this);
    mpVideoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpVideoWidget->setStyleSheet("border: 0px");
    mpVideoWidget->installEventFilter(this);
    mpPlayer = new PlayControl(mpVideoWidget->winId(), this);

    QGridLayout *grid = new QGridLayout(this);
    grid->setContentsMargins(2, 2, 2, 2);
    grid->setSpacing(0);
    grid->addWidget(mpExtensionLbl, 0, 0);
    grid->addWidget(mpTimeLbl, 0, 1);
    grid->addWidget(mpVideoWidget, 1, 0, 1, 2);
}

void VideoWidget::setInfo(QString extensionName, QString url)
{
    if (url.isEmpty())
        return;
    mpExtensionLbl->setText(extensionName);

    mpVideoWidget->raise();
    mpPlayer->play("rtsp://" + url);
}

void VideoWidget::setTimeHint(const QDateTime &time)
{
    if (isPlaying()) {
        mpTimeLbl->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
    }
}

void VideoWidget::reset()
{
    mpExtensionLbl->setText("");
    mpTimeLbl->setText("");

    mpPlayer->stop();
}

bool VideoWidget::isPlaying()
{
    return mpPlayer->isPlaying();
}

QString VideoWidget::ExtensionNum()
{
    return mpExtensionLbl->text();
}

bool VideoWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    if (event->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    }
    return QFrame::eventFilter(watched, event);
}





#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
//#include "playcontrol.h"

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;
class VlcWidgetVideo;

class VideoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

    void setInfo(QString extensionName, QString url);
    void setTimeHint(const QDateTime &time);
    void reset();
    bool isPlaying();
    bool isBusy();
    QString ExtensionNum();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void doubleClicked();
    void playError();

    void play();
    void stop();

    void pause();
    void clear();

public slots:

private:
//    QWidget *mpVideoWidget;
    QFrame *mpBottomWidget;
    VlcWidgetVideo *mpVideoWidget;
    QFrame *mpTopWidget;
    QLabel *mpExtensionLbl;
    QLabel *mpTimeLbl;

    static VlcInstance *mspInstance;
    VlcMedia *mpMedia;
    VlcMediaPlayer *mpPlayer;

//    PlayControl *mpPlayer;
};

#endif // VIDEOWIDGET_H

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include "playcontrol.h"

class VideoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

    void setInfo(QString extensionName, QString url);
    void setTimeHint(const QDateTime &time);
    void reset();
    bool isPlaying();
    QString ExtensionNum();
    const QString &url() const { return mURL; }

protected:
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
    QWidget *mpVideoWidget;
    QLabel *mpExtensionLbl;
    QLabel *mpTimeLbl;

    PlayControl *mpPlayer;
    QString mURL;
};

#endif // VIDEOWIDGET_H

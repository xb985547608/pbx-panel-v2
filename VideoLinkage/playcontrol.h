#ifndef PLAYCONTROL_H
#define PLAYCONTROL_H

/**
 * @brief       调用VLC接口播放url
 * @author      xiaobin <xb985547608@gmail.com>
 * @date        20180627
 */

#include <QObject>
#include <qwindowdefs.h>
#include <QMutex>

struct libvlc_media_player_t;
struct libvlc_media_t;
struct libvlc_instance_t;

class PlayControl : public QObject
{
    Q_OBJECT
public:
    explicit PlayControl(WId id, QObject *parent = nullptr);
    ~PlayControl();

    bool isPlaying();
public slots:
    bool play(QString url);
    void stop();
    void pause();

signals:


private:
    static libvlc_instance_t    *mspVlcInstance;

    libvlc_media_t              *mpVlcMedia;
    libvlc_media_player_t       *mpVlcMediaPlayer;
    WId                         mVlcWId;
};

#endif // PLAYCONTROL_H

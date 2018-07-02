#include "playcontrol.h"
#include <QDebug>

libvlc_instance_t *PlayControl::mspVlcInstance = libvlc_new(0, NULL);
QMutex *PlayControl::mspMutex = new QMutex();

PlayControl::PlayControl(WId id, QObject *parent) :
    QObject(parent),
    mpVlcMedia(NULL),
    mpVlcMediaPlayer(NULL),
    mpMutex(new QMutex),
    mVlcWId(id)
{
}

PlayControl::~PlayControl()
{
    reset();
}

bool PlayControl::play(QString url)
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    if (url.isNull())
        return false;

    if (mpVlcMedia != NULL && mpVlcMediaPlayer != NULL) {
        if (url.compare(libvlc_media_get_mrl(mpVlcMedia)) == 0) {
            libvlc_media_player_set_position(mpVlcMediaPlayer, 0.0);
            return libvlc_media_player_play(mpVlcMediaPlayer) == 0;
        } else {
            reset();
        }
    }
    {
        QMutexLocker locker1(mspMutex);
        Q_UNUSED(locker1)
        mpVlcMedia = libvlc_media_new_location(mspVlcInstance, url.toLocal8Bit().data());
    }
    if (mpVlcMedia == NULL) {
        qDebug() << "#PlayControl# play(),  call libvlc_media_new_location() error, url:" << url;
        return false;
    }

    mpVlcMediaPlayer = libvlc_media_player_new_from_media(mpVlcMedia);
    if (mpVlcMediaPlayer == NULL) {
        qDebug() << "#PlayControl# play(),  call libvlc_media_player_new_from_media() error, url:" << url;
        reset();
        return false;
    }

    libvlc_media_player_set_hwnd(mpVlcMediaPlayer, (void *)mVlcWId);

    return libvlc_media_player_play(mpVlcMediaPlayer) == 0;
}

void PlayControl::stop()
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    if (mpVlcMediaPlayer == NULL)
        return;
    libvlc_media_player_stop(mpVlcMediaPlayer);
}

void PlayControl::pause()
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    if (mpVlcMediaPlayer == NULL)
        return;
    libvlc_media_player_pause(mpVlcMediaPlayer);
}

void PlayControl::reset()
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    if(mpVlcMediaPlayer != NULL) {
        libvlc_media_player_release(mpVlcMediaPlayer);
    }
    if(mpVlcMedia != NULL) {
        libvlc_media_release(mpVlcMedia);
    }

    mpVlcMedia = NULL;
    mpVlcMediaPlayer = NULL;
}

bool PlayControl::isPlaying()
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    if (mpVlcMediaPlayer == NULL)
        return false;
    return libvlc_media_player_is_playing(mpVlcMediaPlayer) == 1;
}

bool PlayControl::isInit()
{
    QMutexLocker locker(mpMutex);
    Q_UNUSED(locker)

    return mpVlcMedia != NULL && mpVlcMediaPlayer != NULL;
}


#include "playcontrol.h"
#include <QDebug>

static const char *argv[7]= {"--intf=dummy",
                      "--no-media-library",
                      "--no-stats",
                      "--no-osd",
                      "--no-loop",
                      "--no-video-title-show",
                      "--drop-late-frames"};

libvlc_instance_t *PlayControl::mspVlcInstance = libvlc_new(7, argv);

PlayControl::PlayControl(WId id, QObject *parent) :
    QObject(parent),
    mpVlcMedia(NULL),
    mpVlcMediaPlayer(NULL),
    mVlcWId(id)
{
    mpVlcMediaPlayer = libvlc_media_player_new(mspVlcInstance);
    libvlc_video_set_key_input(mpVlcMediaPlayer, false);
    libvlc_video_set_mouse_input(mpVlcMediaPlayer, false);

    libvlc_media_player_set_hwnd(mpVlcMediaPlayer, (void *)mVlcWId);
}

PlayControl::~PlayControl()
{
    if(mpVlcMediaPlayer != NULL) {
        libvlc_media_player_release(mpVlcMediaPlayer);
        mpVlcMediaPlayer = NULL;
    }

    if(mpVlcMedia != NULL) {
        libvlc_media_release(mpVlcMedia);
        mpVlcMedia = NULL;
    }
}

bool PlayControl::play(QString url)
{
    if (mpVlcMediaPlayer == NULL)
        return false;

    if (url.isNull())
        return false;

    if(mpVlcMedia != NULL) {
        libvlc_media_release(mpVlcMedia);
        mpVlcMedia = NULL;
    }

    mpVlcMedia = libvlc_media_new_location(mspVlcInstance, url.toLocal8Bit().data());
    if (mpVlcMedia == NULL) {
        qDebug() << "#PlayControl# play(),  call libvlc_media_new_location() error, url:" << url;
        return false;
    }
    libvlc_media_player_set_media(mpVlcMediaPlayer, mpVlcMedia);

    return libvlc_media_player_play(mpVlcMediaPlayer) == 0;
}

void PlayControl::stop()
{
    if (mpVlcMediaPlayer == NULL)
        return;

    libvlc_media_player_stop(mpVlcMediaPlayer);
}

void PlayControl::pause()
{
    if (mpVlcMediaPlayer == NULL)
        return;

    libvlc_media_player_pause(mpVlcMediaPlayer);
}

bool PlayControl::isPlaying()
{
    if (mpVlcMediaPlayer == NULL)
        return false;

    return libvlc_media_player_is_playing(mpVlcMediaPlayer) == 1;
}

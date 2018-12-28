#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <vlc/vlc.h>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);

    static void amemPlay(void *opaque, const void *data,
                          unsigned count, int64_t /*pts*/);
    static void handleEvents(const libvlc_event_t* pEvt, void *opaque);

    /**
     * @brief VlcInstance
     * указатель на инстанцию
     */
    libvlc_instance_t *VlcInstance;
    /**
     * @brief Media
     * указатель на объект медиа дескриптора
     */
    libvlc_media_t *Media;
    /**
     * @brief MediaPlayer
     * указатель на обект плеера для прослушивания
     */
    libvlc_media_player_t *MediaPlayer;
    /**
     * @brief ArduinoMediaPlayer
     * указатель на обект плеерп для ардвино
     */
    libvlc_media_player_t *ArduinoMediaPlayer;

signals:

public slots:

private:
    long double TrackDuration;

};

#endif // PLAYER_H

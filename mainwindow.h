#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <vlc/vlc.h>
#include <QDebug>
#include "serialthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief amem_Play -коллбек для получения блоков семплов
     * @param opaque -указатель на данные указаные при регистрации
     * @param data -массив с семплами
     * @param count -колличество семплов
     */
    static void amem_Play(void *opaque, const void *data,
                          unsigned count, int64_t /*pts*/);
    /**
     * @brief handle_Events -коллбек для оработки событий
     * @param pEvt -указатель на тип произошедшего события
     * @param opaque -данные указанные при регистации
     */
    static void handle_Events(const libvlc_event_t* pEvt, void *opaque);

    void initVLCMedia(QString file);
    void releaseVLCMedia();

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

    SerialThread *ST;

    int channels;

signals:
    void updateProgress();
    void setFRQ(int, int);

private slots:
    void timeChanged();

    void on_OpenFile_clicked();

    void on_Pause_clicked();

    void on_Play_clicked();

    void on_Stop_clicked();

    void on_Volume_valueChanged(int value);

    void on_ArduinoEnabled_toggled(bool checked);

    void on_ArduinoVolue_valueChanged(int value);

    void on_frq_p9_valueChanged(int value);

    void on_frq_p10_valueChanged(int value);

    void on_frq_p11_valueChanged(int value);

    void on_frq_p6_valueChanged(int value);

    void on_frq_p5_valueChanged(int value);

    void on_frq_p3_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    long double TrackDuration;
};

#endif // MAINWINDOW_H

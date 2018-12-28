#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::handle_Events(const libvlc_event_t* pEvt, void *opaque)
{
    MainWindow *MW = static_cast<MainWindow *>(opaque);
    switch (pEvt->type) {
    case libvlc_MediaPlayerTimeChanged:
        emit MW->updateProgress();
        break;
    default:
        qDebug() << "unknown event";
        break;
    }
}
void MainWindow::amem_Play(void *opaque, const void *data,
                           unsigned count, int64_t /*pts*/)
{
    MainWindow *MW = static_cast<MainWindow *>(opaque);
    const char *fdata = static_cast<const char *>(data);

    QVector<short> PCMData;
    int byte_per_sample = 2;//16 bit
    int length = count * byte_per_sample * MW->channels;
    short semple;
    for(int i = 0; i < length; ++i)
    {
        semple = static_cast<short>(fdata[i]);// little byte
        semple += static_cast<short>(fdata[++i] << 8);// + big byte
        PCMData.push_back(semple);
    }
    MW->ST->setPCMData(PCMData);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Media = NULL;
    MediaPlayer = NULL;
    ArduinoMediaPlayer = NULL;

    ST = new SerialThread();

    ui->setupUi(this);

    VlcInstance = libvlc_new(0, NULL);

    QStringList PortList = ST->getPortList();
    if(PortList.isEmpty())
    {
       ui->ArduinoEnabled->setEnabled(false);
       ui->ArduinoPort->setEnabled(false);
    }
    else
        ui->ArduinoPort->addItems(PortList);

    qRegisterMetaType< QVector<short> >("QVector<short>");
    connect(this, SIGNAL(updateProgress()),
            this, SLOT(timeChanged()));
    connect(ui->MinLevel, SIGNAL(valueChanged(int)),
            ST, SLOT(setMinLevel(int)));
    connect(ui->SendInterval, SIGNAL(valueChanged(int)),
            ST, SLOT(setSendInterval(int)));
    connect(this, SIGNAL(setFRQ(int, int)),
            ST, SLOT(setSetFRQ(int, int)));
}

MainWindow::~MainWindow()
{
    releaseVLCMedia();
    libvlc_release(VlcInstance);
    delete ST;
    delete ui;
}
void MainWindow::initVLCMedia(QString file)
{
    releaseVLCMedia();

    ///Создаёт медиа объект с указаным файлом
    Media = libvlc_media_new_path(VlcInstance,
                                  file.toStdString().c_str());

    ///получает информацию о файле
    libvlc_media_parse(Media);
    libvlc_media_track_t **tracks;
    libvlc_media_tracks_get(Media, &tracks);

    /// передаёт частоту дискретизации
    ST->setSampleRate(tracks[0]->audio->i_rate);

    channels = tracks[0]->audio->i_channels;

    /// получает длительность трека
    TrackDuration = static_cast<long double>(
                    libvlc_media_get_duration(Media));

    /// создаёт обект плеера для прослушивания
    MediaPlayer = libvlc_media_player_new_from_media(Media);

    /// объект плеера для ардуино
    ArduinoMediaPlayer = libvlc_media_player_new_from_media(Media);

    /// создаёт менеджер событий
    libvlc_event_manager_t* eventManager =
                        libvlc_media_player_event_manager(MediaPlayer);

    /// регистрирует событие изменения времени 
    libvlc_event_attach(eventManager, libvlc_MediaPlayerTimeChanged,
                        MainWindow::handle_Events, this);

    /// регистрирует функцию для коллбека
    libvlc_audio_set_callbacks(ArduinoMediaPlayer,
                               MainWindow::amem_Play, NULL,
                               NULL, NULL, NULL, this);

    /// указывает формат декодирования
    libvlc_audio_set_format(ArduinoMediaPlayer,"s16l",
                            tracks[0]->audio->i_rate, channels);
}

void MainWindow::releaseVLCMedia()
{
    if(MediaPlayer)
    {
        libvlc_media_player_release(MediaPlayer);
        libvlc_media_player_release(ArduinoMediaPlayer);
    }
    if(Media)
    {
        libvlc_media_release(Media);
    }
}

void MainWindow::on_OpenFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), "",
                                                "*.mp3");

    if(!file.isEmpty())
    {
        file.replace("/", "\\");
        initVLCMedia(file);
        ui->label_File->setText(file);
    }
}

void MainWindow::on_Pause_clicked()
{
    if(MediaPlayer)
    {
        libvlc_media_player_pause(MediaPlayer);
        libvlc_media_player_pause(ArduinoMediaPlayer);
    }
}

void MainWindow::on_Play_clicked()
{
    if(MediaPlayer)
    {
        libvlc_media_player_play(MediaPlayer);
        libvlc_audio_set_volume(MediaPlayer,
                                ui->Volume->value());
        libvlc_media_player_play(ArduinoMediaPlayer);
        libvlc_audio_set_volume(ArduinoMediaPlayer,
                                ui->ArduinoVolue->value());
    }
}

void MainWindow::on_Stop_clicked()
{
    if(MediaPlayer)
    {
        libvlc_media_player_stop(MediaPlayer);
        libvlc_media_player_stop(ArduinoMediaPlayer);
    }
}

void MainWindow::on_Volume_valueChanged(int value)
{
    if(MediaPlayer)
    {
        libvlc_audio_set_volume(MediaPlayer, value);
    }
}

void MainWindow::on_ArduinoEnabled_toggled(bool checked)
{
    if(checked)
        ST->enableArduino(ui->ArduinoPort->itemText(
                              ui->ArduinoPort->currentIndex()));
    else
        ST->disableArduino();
}

void MainWindow::timeChanged()
{
    long double c = static_cast<long double>(
                libvlc_media_player_get_time(MediaPlayer));
    int current_pos = static_cast<int>((c / TrackDuration) * 1000);
    ui->progressBar->setValue(current_pos + 1);
}

void MainWindow::on_ArduinoVolue_valueChanged(int value)
{
    libvlc_audio_set_volume(ArduinoMediaPlayer, value);
}

void MainWindow::on_frq_p9_valueChanged(int value)
{
    emit setFRQ(2, value);
}

void MainWindow::on_frq_p10_valueChanged(int value)
{
    emit setFRQ(1, value);
}

void MainWindow::on_frq_p11_valueChanged(int value)
{
    emit setFRQ(0, value);
}

void MainWindow::on_frq_p6_valueChanged(int value)
{
    emit setFRQ(3, value);
}

void MainWindow::on_frq_p5_valueChanged(int value)
{
    emit setFRQ(4, value);
}

void MainWindow::on_frq_p3_valueChanged(int value)
{
    emit setFRQ(5, value);
}

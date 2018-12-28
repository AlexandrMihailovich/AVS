#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QMutex>
#include <QDebug>
#include <math.h>

class SerialThread : public QThread
{
    Q_OBJECT
public:
    SerialThread();
    ~SerialThread();

    void run() Q_DECL_OVERRIDE;

    QStringList getPortList();
    void enableArduino(QString port);
    void disableArduino();
    QVector<short> FFTAnalysis();
    char getAmplitudeByFrequency(unsigned frequency,
                                 QVector<short> FFTValue);
    void setSampleRate(int rate) {SampleRate = rate;}
    void setPCMData(QVector<short> data);

public slots:
    void setMinLevel(int level);
    void setSendInterval(int interval);
    void setSetFRQ(int pin, int val);

private:
    bool enable;
    QMutex mutex, rwmutex;
    QSerialPort serial;
    QVector<short> PCMData;
    int SampleRate;
    char MinLevel;
    int SendInterval;
    QList<int> PinFRQ;
};

#endif // SERIALTHREAD_H

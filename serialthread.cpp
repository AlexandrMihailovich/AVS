#include "serialthread.h"

SerialThread::SerialThread()
{
    MinLevel = 25;
    SendInterval = 25;

    PinFRQ << 50   //pin 11
           << 50  //pin 10
           << 50  //pin9
           << 50  //pin6
           << 50 //pin5
           << 50; //pin3
}

SerialThread::~SerialThread()
{
    disableArduino();
}

void SerialThread::run()
{
    while(enable) {
        mutex.lock();

        int count = PinFRQ.count();
        char c[count];
        QVector<short> FFTValue = FFTAnalysis();

        for(int i = 0; i < count; ++i)
        {
            c[i] = getAmplitudeByFrequency(PinFRQ[i], FFTValue);
            if( c[i] < MinLevel)
                c[i] = 0;
        }

        serial.write(c, count);
        serial.waitForBytesWritten(SendInterval);
        mutex.unlock();
    }
    serial.close();
}

QStringList SerialThread::getPortList()
{
    QStringList PortList;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        PortList << info.portName();
    }
    return PortList;
}

void SerialThread::enableArduino(QString port)
{
    serial.setPortName(port);
    serial.open(QIODevice::ReadWrite);
    enable = true;
    if (!isRunning())
        start();
}

void SerialThread::disableArduino()
{
    enable = false;
}

QVector<short> SerialThread::FFTAnalysis() {

    if(PCMData.isEmpty())
        return PCMData;
    rwmutex.lock();
    QVector<short> AVal = PCMData;
    rwmutex.unlock();
    int Nvl = AVal.count() / 1024;
    Nvl *= 1024;
    int Nft = Nvl;
    double TwoPi = 6.283185307179586;

    QVector<short> FTvl(Nft);
    int i, j, n, m, Mmax, Istp;
    double Tmpr, Tmpi, Wtmp, Theta;
    double Wpr, Wpi, Wr, Wi;
    double *Tmvl;

    n = Nvl * 2;

    Tmvl = new double[n];

    for (i = 0; i < n; i+=2) {
        Tmvl[i] = 0;
        Tmvl[i+1] = AVal[i/2];
    }

    i = 1;
    j = 1;

    while (i < n) {
        if (j > i) {
            Tmpr = Tmvl[i];
            Tmvl[i] = Tmvl[j];
            Tmvl[j] = Tmpr;
            Tmpr = Tmvl[i+1];
            Tmvl[i+1] = Tmvl[j+1];
            Tmvl[j+1] = Tmpr;
        }
        i = i + 2;
        m = Nvl;
        while ((m >= 2) && (j > m)) {
            j = j - m;
            m = m >> 1;
        }
        j = j + m;
    }
    Mmax = 2;
    while (n > Mmax) {
        Theta = -TwoPi / Mmax;
        Wpi = sin(Theta);
        Wtmp = sin(Theta / 2);
        Wpr = Wtmp * Wtmp * 2;
        Istp = Mmax * 2;
        Wr = 1;
        Wi = 0;
        m = 1;

        while (m < Mmax) {
            i = m;
            m = m + 2;
            Tmpr = Wr;
            Tmpi = Wi;

            Wr = Wr - Tmpr * Wpr - Tmpi * Wpi;
            Wi = Wi + Tmpr * Wpi - Tmpi * Wpr;

            while (i < n) {
                j = i + Mmax;
                Tmpr = Wr * Tmvl[j] - Wi * Tmvl[j-1];
                Tmpi = Wi * Tmvl[j] + Wr * Tmvl[j-1];
                Tmvl[j] = Tmvl[i] - Tmpr;
                Tmvl[j-1] = Tmvl[i-1] - Tmpi;
                Tmvl[i] = Tmvl[i] + Tmpr;
                Tmvl[i-1] = Tmvl[i-1] + Tmpi;
                i = i + Istp;
            }
        }
        Mmax = Istp;
    }
    for (i = 0; i < Nft; i++) {
        j = i * 2;
        FTvl[Nft - i - 1] = sqrt(pow(Tmvl[j],2) + pow(Tmvl[j+1],2));
    }
    delete []Tmvl;
    return FTvl;
}

char SerialThread::getAmplitudeByFrequency(unsigned frequency,
                                           QVector<short> FFTValue)
{
    int size = FFTValue.count();
    short a;
    //if(size != 0 && SampleRate != 0 && frequency)
    //    qDebug() <<  frequency / (SampleRate / size);
    if(size != 0 && SampleRate != 0 && frequency)
        a = FFTValue[(size_t) frequency / (SampleRate / size)];
    else
        a = 0;
    return static_cast<char>(a / 256);
}

void SerialThread::setPCMData(QVector<short> data)
{
    rwmutex.lock();
    PCMData = data;
    rwmutex.unlock();
}

void SerialThread::setMinLevel(int level)
{
    mutex.lock();
    MinLevel = level;
    mutex.unlock();
}

void SerialThread::setSendInterval(int interval)
{
    mutex.lock();
    SendInterval = interval;
    mutex.unlock();
}

void SerialThread::setSetFRQ(int pin, int val) {
    mutex.lock();
    PinFRQ[pin] = val;
    mutex.unlock();
}

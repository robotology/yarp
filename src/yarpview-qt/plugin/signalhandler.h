#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <QObject>
#include <QVideoFrame>
#include <QMutex>
#include <QTimer>
#include "FpsStats.h"

class SignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit SignalHandler(QObject *parent = 0);
    ~SignalHandler();

    void sendVideoFrame(QVideoFrame);
    void synchToDisplay(bool check);
    void changeRefreshInterval(int ineterval);
    void freeze(bool check);
    void saveCurrentFrame();
    void setFileName(QUrl url);
    void setFileNames(QUrl url);
    void startDumpFrames();
    void stopDumpFrames();

private:
    void saveFrame(QImage);
    void saveFrameSet(QImage);
    void checkDefaultNameCounterCount();
    void checkCustomNameCounterCount(QString file);

signals:
    void internalSendFrame(QVideoFrame);
    void sendFrame(QVideoFrame*);
    void sendFps(double portAvg, double portMin, double portMax,
                 double dispAvg, double dispMin, double dispMax);
    void selfStartTimer();
    void testSignal();

private slots:
    void internalReceiveFrame(QVideoFrame);
    void onTimerElapsed();
    void onSelfStartTimer();
    void onFpsTimer();

private:
    FpsStats portFps;
    FpsStats displayFps;
    QString fileName;
    QString fileNames;
    bool saveSetFrameMode;
    bool saveCurrentFrameMode;
    bool freezeMode;
    bool synchMode;
    QVideoFrame frame;
    QMutex mutex;
    QTimer timer;
    QTimer fpsTimer;
    int defaultNameCounter;
    int customNameCounter;
    int framesetCounter;


};

#endif // SIGNALHANDLER_H

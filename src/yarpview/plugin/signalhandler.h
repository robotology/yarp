/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <QObject>
#include <QVideoFrame>
#include <QMutex>
#include <QTimer>
#include "FpsStats.h"

/*! \class SignalHandler
    \brief The class that handles some signals

    This class is used to unlock the backend thread from the GUI thread. It is mainly used for
    receive the frame from the callback and then redirect the signal to its own thread
    using a connection in Qt::QueuedConnection modality
*/
class SignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit SignalHandler(QObject *parent = 0);
    ~SignalHandler();

    void sendVideoFrame(QVideoFrame);
    void synchDisplayPeriod(bool check);
    void synchDisplaySize(bool check);
    void changeRefreshInterval(int ineterval);
    void freeze(bool check);
    void saveCurrentFrame();
    void setFileName(QUrl url);
    void setFileNames(QUrl url);
    void startDumpFrames();
    void stopDumpFrames();
    bool getAutosizeMode();

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
    bool b_saveSetFrameMode;
    bool b_saveCurrentFrameMode;
    bool b_freezeMode;
    bool b_synchRateMode;
    bool b_autosizeMode;
    QVideoFrame frame;
    QMutex mutex;
    QTimer timer;
    QTimer fpsTimer;
    int defaultNameCounter;
    int customNameCounter;
    int framesetCounter;


};

#endif // SIGNALHANDLER_H

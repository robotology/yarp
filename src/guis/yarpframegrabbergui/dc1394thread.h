/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef DC1394THREAD_H
#define DC1394THREAD_H

#include <QThread>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/PolyDriver.h>
#include <QMutex>
#include <QWaitCondition>
#include <QSize>
#include <QQueue>
#include <QVariant>

typedef enum {
    _init,
    _initFormatTab,
    _reload,
    _reset,
    _loadDefault,
    _setTransmissionDC1394,
    _setPowerDC1394,
    _setFormat7WindowDC1394,
    _setVideoModeDC1394,
    _setColorCodingDC1394,
    _setFPSDC1394,
    _setISOSpeedDC1394,
    _setBytesPerPacketDC1394,
    _setOperationModeDC1394,
    _sliderRefresh,
    _sliderWBRefresh,
    _sliderPropagate,
    _sliderWBPropagate,
    _sliderSetFeature,
    _sliderWBSetFeature,
    _sliderOnePush,
    _sliderWBOnePush,
    _sliderRadioAuto,
    _sliderPower,
    _sliderHasFeature,
    _unknown
} threadFunction;

class DC1394Thread : public QThread
{
    Q_OBJECT



public:
    explicit DC1394Thread(std::string loc, std::string rem, QObject* parent = 0);
    void stop();
    void initFormatTab();
    void init();
    void reload();
    void reset();
    void loadDefualt();
    void setTransmissionDC1394(QVariantList arg);
    void setPowerDC1394(QVariantList arg);
    void setFormat7WindowDC1394(QVariantList arg);
    void setVideoModeDC1394(QVariantList arg);
    void setColorCodingDC1394(QVariantList arg);
    void setFPSDC1394(QVariantList arg);
    void setISOSpeedDC1394(QVariantList);
    void setBytesPerPacketDC1394(QVariantList arg);
    void setOperationModeDC1394(QVariantList arg);

    /*** Sliders ***/
    void sliderRefresh(QVariantList arg);
    void sliderWBRefresh(QVariantList arg);
    void sliderPropagate(QVariantList arg);
    void sliderWBPropagate(QVariantList arg);
    void sliderSetFeatureDC1394(QVariantList arg);
    void sliderWBSetFeatureDC1394(QVariantList arg);
    void sliderOnePush(QVariantList arg);
    void sliderWBOnePush(QVariantList arg);
    void sliderRadioAuto(QVariantList arg);
    void sliderPower(QVariantList arg);
    void sliderHasFeature(QVariantList arg);
    bool getCameraDescription(yarp::dev::CameraDescriptor& camera);

private:
    yarp::dev::PolyDriver *grabberControl =nullptr;
    yarp::dev::IFrameGrabberControls* fgControl = nullptr;
    yarp::dev::IFrameGrabberControlsDC1394* DC1394Control = nullptr;

    QMutex semaphore;

    QMutex mutex;
    QMutex mutex1;
    QWaitCondition waitCond;
    bool keepRunning;

    QString rem;
    QString loc;


    QQueue <QVariantList> taskList;

    int opCounter;

protected:
    void run();

signals:
    void done();
    void initFormatTabDone(uint,uint,uint);
    void initDone(uint,uint,uint,
                  bool, uint, QSize, QSize, QSize,
                  QSize,QSize,uint,bool);
    void reloadDone(uint,uint,QSize, QSize, QSize,
                    QSize,QSize,uint,uint,uint,uint,uint);

    void resetDone(uint,uint,bool);
    void loadDefaultDone(uint,uint,bool);
    void setTransmissionDC1394Done();
    void setPowerDC1394Done();
    void setFormat7WindowDC1394Done();
    void setVideoModeDC1394Done();
    void setColorCodingDC1394Done();
    void setFPSDC1394Done();
    void setISOSpeedDC1394Done();
    void setBytesPerPacketDC1394Done();
    void setOperationModeDC1394Done();
    void sliderRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double);
    void sliderWBRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double,double);
    void sliderPropagateDone();
    void sliderWBPropagateDone();
    void sliderSetFeatureDC1394Done(QObject*,double);
    void sliderWBSetFeatureDC1394Done(QObject*,double,double);
    void sliderOnePushDone(QObject*,double);
    void sliderWBOnePushDone(QObject*,double,double);
    void sliderRadioAutoDone(QObject*,bool,bool);
    void sliderPowerDone(QObject*,bool,bool,bool,bool);
    void sliderHasFeatureDone(QObject*,bool);

    void startLoading();
    void stopLoading();

public slots:
    void doTask(threadFunction);
    void doTask(threadFunction, QVariantList args);

};

#endif // DC1394THREAD_H

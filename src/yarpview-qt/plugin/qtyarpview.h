/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef QTYARPVIEW_H
#define QTYARPVIEW_H

#include <QQuickItem>
#include "videoproducer.h"

//#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include "ImagePort.h"
#include "signalhandler.h"
//using namespace yarp::os;

//-------------------------------------------------
// Program Options
//-------------------------------------------------
/*! \struct mOptions
    \brief The struct that stores the options
*/
struct mOptions
{
    unsigned int	refreshTime;
    char			portName[256];
    char			networkName[256];
    int				windWidth;
    int				windHeight;
    int				posX;
    int				posY;
    char			fileName[256];
    int				saveOnExit;
    char			outPortName[256];
    char			outNetworkName[256];
    int				outputEnabled;
    bool            synch;
};
typedef struct mOptions pgmOptions;

/*! \class QtYARPView
    \brief The plugin Core class

    this is the plugin core class wich acts as bridge between the QML and c++.
    in the C++ code is implemented the backend logic, instead in the QML is
    implemented the Visual part.
*/
class QtYARPView : public QQuickItem
{
    Q_OBJECT

    Q_DISABLE_COPY(QtYARPView)
    Q_PROPERTY(QObject *videoProducer READ getVideoProducer NOTIFY videoProducerChanged)
    Q_PROPERTY(int posX READ posX NOTIFY posXChanged)
    Q_PROPERTY(int posY READ posY NOTIFY posYChanged)
    Q_PROPERTY(int windowWidth READ windowWidth NOTIFY widthChanged)
    Q_PROPERTY(int windowHeight READ windowHeight NOTIFY heightChanged)
    Q_PROPERTY(int refreshInterval READ refreshInterval NOTIFY refreshIntervalChanged)

public:
    QtYARPView(QQuickItem *parent = 0);
    ~QtYARPView();

    Q_INVOKABLE void freeze(bool check);
    Q_INVOKABLE void synchToDisplay(bool check);
    Q_INVOKABLE void changeRefreshInterval(int);
    Q_INVOKABLE void saveFrame();
    Q_INVOKABLE void setFileName(QUrl url);
    Q_INVOKABLE void setFilesName(QUrl url);
    Q_INVOKABLE void startDumpFrames();
    Q_INVOKABLE void stopDumpFrames();
    Q_INVOKABLE bool parseParameters(QStringList);
    Q_INVOKABLE void clickCoords(int x, int y);

    QObject *getVideoProducer();
    int posX();
    int posY();
    int windowWidth();
    int windowHeight();
    int refreshInterval();

private:
    void createObjects();
    void deleteObjects();
    void printHelp();
    void setOptionsToDefault();
    void setOptions(yarp::os::Searchable& options);
    bool openPorts();
    void closePorts();
    void saveOptFile(char *fileName);
    void periodToFreq(double avT, double mT, double MT, double &avH, double &mH, double &MH);

private:
    SignalHandler sigHandler;
    VideoProducer videoProducer;

    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgra> > *ptr_inputPort;
    yarp::os::BufferedPort<yarp::os::Bottle> *_pOutPort;
    InputCallback *ptr_portCallback;
    pgmOptions _options;
    yarp::os::Network yarp;

signals:
    void refreshIntervalChanged();
    void videoProducerChanged();
    void posXChanged();
    void posYChanged();
    void widthChanged();
    void heightChanged();
    void sendPortFps(QString avg, QString min, QString max);
    void sendDisplayFps(QString avg, QString min, QString max);
    void synch(bool check);
    void setName(QString name);
private slots:
    void onSendFps(double portAvg, double portMin, double portMax, double dispAvg, double dispMin, double dispMax);
};

#endif // QTYARPVIEW_H


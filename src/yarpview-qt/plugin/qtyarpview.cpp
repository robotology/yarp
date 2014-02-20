/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "qtyarpview.h"
#include <QThread>
using namespace yarp::os;

QtYarpView::QtYarpView(QQuickItem *parent):
    QQuickItem(parent),sigHandler(this)
{
    ptr_portCallback = NULL;
    setOptionsToDefault();

    connect(&sigHandler,SIGNAL(sendFrame(QVideoFrame*)),&videoProducer,
            SLOT(onNewVideoContentReceived(QVideoFrame*)),Qt::DirectConnection);

    connect(&sigHandler,SIGNAL(sendFps(double,double,double,double,double,double)),
            this, SLOT(onSendFps(double,double,double,double,double,double)));
    createObjects();
    sprintf(_options.portName, "%s", "/view");


}

QtYarpView::~QtYarpView()
{
    closePorts();
    deleteObjects();

    if (_options.saveOnExit != 0){
        saveOptFile(_options.fileName);
    }
}

/*! \brief Freeze the video stream.
 *
 *  \param check a bool parameter that enbales or disables the freeze state
 */
void QtYarpView::freeze(bool check)
{
    sigHandler.freeze(check);
}

/*! \brief returns the videoproducer that acts as bridge between C++ code and QML VideoOutput.
 *
 *  \return A pointer to the videoproducer
 */
QObject *QtYarpView::getVideoProducer()
{
    return &videoProducer;
}

/*! \brief Returns the x position from the options.*/
int QtYarpView::posX()
{
    return _options.posX;
}

/*! \brief Returns the y position from the options.*/
int QtYarpView::posY()
{
    return _options.posY;
}

/*! \brief Returns the width from the options.*/
int QtYarpView::windowWidth()
{
    return _options.windWidth;
}

/*! \brief Returns the height from the options.*/
int QtYarpView::windowHeight()
{
    return _options.windHeight;
}


/*! \brief Synchs the video stream to the display.
 *
 *  \param check a bool parameter that enbales or disables the synch option
 */
void QtYarpView::synchToDisplay(bool check)
{
    sigHandler.synchToDisplay(check);
}

/*! \brief Changes the refresh interval.
 *
 *  \param interval an int value representing the interval
 */
void QtYarpView::changeRefreshInterval(int interval)
{
    sigHandler.changeRefreshInterval(interval);
}

/*! \brief Saves the current frame. */
void QtYarpView::saveFrame()
{
    sigHandler.saveCurrentFrame();
}

/*! \brief Sets the current filename of the frame is going to be saved.
 *
 *  \param url the url to the file
 */
void QtYarpView::setFileName(QUrl url)
{
    sigHandler.setFileName(url);
}

/*! \brief Sets the current filename of the frame set is going to be saved.
 *
 *  \param url the url to the file
 */
void QtYarpView::setFilesName(QUrl url)
{
    sigHandler.setFileNames(url);
}

/*! \brief Saves the current frame set. */
void QtYarpView::startDumpFrames()
{
    sigHandler.startDumpFrames();
}

/*! \brief Stops the current frame set save mode. */
void QtYarpView::stopDumpFrames()
{
    sigHandler.stopDumpFrames();
}

void QtYarpView::periodToFreq(double avT, double mT, double MT, double &avH, double &mH, double &MH)
{
    if (avT!=0)
        avH=1.0/avT;
    else
        avH=0;

    if (mT!=0)
        MH=1.0/mT;
    else
        MH=0;

    if (MT!=0)
        mH=1.0/MT;
    else
        mH=0;
}

void QtYarpView::onSendFps(double portAvg, double portMin, double portMax,
                           double dispAvg, double dispMin, double dispMax)
{
    double pAvg,pMin,pMax,dAvg,dMin,dMax;

    periodToFreq(portAvg,portMin,portMax,pAvg,pMin,pMax);
    periodToFreq(dispAvg,dispMin,dispMax,dAvg,dMin,dMax);


    sendPortFps(QString::number(pAvg,'f',1),
            QString::number(pMin,'f',1),
            QString::number(pMax,'f',1));

    sendDisplayFps(QString::number(dAvg,'f',1),
            QString::number(dMin,'f',1),
            QString::number(dMax,'f',1));

}

/*! \brief Gets the refresh interval from options.
    \return the refresh interval
*/
int QtYarpView::refreshInterval()
{
    return _options.refreshTime;
}


/*! \brief Creates the input port and the port callback.*/
void QtYarpView::createObjects() {
    ptr_inputPort = new BufferedPort<yarp::sig::FlexImage>;
    ptr_portCallback = new InputCallback;
    ptr_portCallback->setSignalHandler(&sigHandler);
}

/*! \brief Deletes the input port and the port callback.*/
void QtYarpView::deleteObjects() {
    if (ptr_inputPort!=0)
        delete ptr_inputPort;
    if (ptr_portCallback!=0)
        delete ptr_portCallback;
}

/*! \brief parse the parameters received from the main container in QstringList form
    \param params the parameter list
*/
bool QtYarpView::parseParameters(QStringList params)
{

    Property options;
    int c = params.count();
    char **v;


    v = (char**)malloc(sizeof(char*) * c);

    for(int i=0;i<params.count();i++){
        v[i] = (char*)malloc(sizeof(char) * params.at(i).length()+1);
        strcpy(v[i],params.at(i).toLatin1().data());
    }

    if (c==1){
        if (params.at(0).compare("--help") == 0){
            printHelp();
            free(v);
            return false;
        }
        // user did not use flags, just gave a port name
        // might as well allow this
        options.put("name",params.at(0).toLatin1().data());
    }else {
        options.fromCommand(c,v,false);
    }
    setOptions(options);
    free(v);

    if (!openPorts()){
        qDebug("Error open ports");
        return false;
    }
    setName(QString("%1").arg(_options.portName));
    ptr_inputPort->useCallback(*ptr_portCallback);
    return true;
}

/*! \brief Sets the options received from command line*/
void QtYarpView::setOptions(yarp::os::Searchable& options) {
    // switch to subsections if available
    yarp::os::Value *val;
    if (options.check("PortName",val)||options.check("name",val)) {
        sprintf(_options.portName, "%s", val->asString().c_str());
        qDebug("%s", val->asString().c_str());
    }
    if (options.check("NetName",val)||options.check("n",val)) {
        sprintf(_options.networkName, "%s", val->asString().c_str());
    }
    if (options.check("OutPortName",val)||options.check("out",val)) {
        sprintf(_options.outPortName, "%s", val->asString().c_str());
    }
    if (options.check("OutNetName",val)||options.check("neto",val)) {
        sprintf(_options.outNetworkName, "%s", val->asString().c_str());
    }
    if (options.check("RefreshTime",val)||options.check("p",val)) {
        _options.refreshTime = val->asInt();
        sigHandler.changeRefreshInterval(_options.refreshTime);
        refreshIntervalChanged();
    }
    if (options.check("PosX",val)||options.check("x",val)) {
        _options.posX = val->asInt();
        posXChanged();
    }
    if (options.check("PosY",val)||options.check("y",val)) {
        _options.posY = val->asInt();
        posYChanged();
    }
    if (options.check("Width",val)||options.check("w",val)) {
        _options.windWidth = val->asInt();
        widthChanged();
    }
    if (options.check("Height",val)||options.check("h",val)) {
        _options.windHeight = val->asInt();
        heightChanged();
    }
    if (options.check("OutputEnabled",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (options.check("out",val)) {
        _options.outputEnabled = true;
    }
    if (options.check("SaveOptions",val)||options.check("saveoptions",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (options.check("synch"))
    {
        _options.synch=true;
        synchToDisplay(true);
        synch(true);
    }
}

/*! \brief Prints the help menu.*/
void QtYarpView::printHelp()
{
    qDebug("yarpview usage:");
    qDebug("--name: input port name (default: /yarpview/img:i)");
    qDebug("--x: x position of the window in the screen");
    qDebug("--y: y position of the window in the screen");
    qDebug("--w, --h: size of the window, weight and height");
    qDebug("--p: refresh time [ms]");
    qDebug("--synch: synchronous display, every image received by the input port is displayed");
    qDebug("--out: output port name (no default is given, if this option is not specified the port is not created)");
    qDebug("--neto: output network");
    qDebug("--neti: input network");
}

/*! \brief Stes the options to their defaults.*/
void QtYarpView::setOptionsToDefault()
{
    // Options defaults
    _options.refreshTime = 100;
    sprintf(_options.portName, "%s","/yarpview/img:i");
    sprintf(_options.networkName, "%s", "default");
    sprintf(_options.outPortName, "%s","/yarpview/o:point");
    sprintf(_options.outNetworkName, "%s", "default");
    _options.outputEnabled = 0;
    _options.windWidth = 300;
    _options.windHeight = 300;
    _options.posX = 100;
    _options.posY = 100;
    sprintf(_options.fileName, "%s","yarpview.conf");
    _options.saveOnExit = 0;

    posXChanged();
    posYChanged();
    widthChanged();
    heightChanged();

    sigHandler.changeRefreshInterval(_options.refreshTime);
    refreshIntervalChanged();
}


/*! \brief Opens the ports.
    \return false in case of error, true otherwise
*/
bool QtYarpView::openPorts()
{
    bool ret = false;

    ptr_inputPort->setReadOnly();
    ret= ptr_inputPort->open(_options.portName);

    if (!ret){
        qDebug("Error: port failed to open, quitting.");
        return false;
    }

    if (_options.outputEnabled == 1){
        _pOutPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
        qDebug("Registering port %s on network %s...", _options.outPortName, _options.outNetworkName);
        bool ok = _pOutPort->open(_options.outPortName);
        if (ok) {
            qDebug("Port registration succeed!");
        }
        else{
            qDebug("ERROR: Port registration failed.\nQuitting, sorry.");
            return false;
        }
    }

    return true;
}

/*! \brief Closes the ports.*/
void QtYarpView::closePorts()
{

    ptr_inputPort->close();

    if (_options.outputEnabled == 1){
        _pOutPort->close();
        bool ok = true;
        if  (ok)
            qDebug("Port %s unregistration succeed!\n", _options.outPortName);
        else
            qDebug("ERROR: Port %s unregistration failed.\n", _options.outPortName);
        delete _pOutPort;
        _pOutPort = NULL;
    }
}

/*! \brief Save the options to a file.
    \param fileName the filename
*/
void QtYarpView::saveOptFile(char *fileName)
{
    //FILE *optFile = NULL;
    QFile optFile(QString("%1").arg(fileName));

    bool opened = optFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    if (!opened){
        qDebug("ERROR: Impossible to save to option file.");
        return;
    }
    optFile.write((QString("PortName %1\n").arg(_options.portName)).toLatin1().data());
    optFile.write((QString("NetName %1\n").arg(_options.networkName)).toLatin1().data());
    optFile.write((QString("OutPortName %1\n").arg(_options.outPortName)).toLatin1().data());
    optFile.write((QString("OutNetName %s\n").arg(_options.outNetworkName)).toLatin1().data());
    optFile.write((QString("RefreshTime %d\n").arg(_options.refreshTime)).toLatin1().data());
    optFile.write((QString("PosX %d\n").arg(_options.posX)).toLatin1().data());
    optFile.write((QString("PosY %d\n").arg(_options.posY)).toLatin1().data());
    optFile.write((QString("Width %d\n").arg(_options.windWidth)).toLatin1().data());
    optFile.write((QString("Height %d\n").arg(_options.windHeight)).toLatin1().data());
    optFile.write((QString("OutputEnables %d\n").arg(_options.outputEnabled)).toLatin1().data());
    optFile.write((QString("SaveOptions %d\n").arg(_options.saveOnExit)).toLatin1().data());
    optFile.write((QString("synch %d\n").arg(_options.synch)).toLatin1().data());
    optFile.close();
}


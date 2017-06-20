/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

QtYARPView::QtYARPView(QQuickItem *parent):
    QQuickItem(parent), yarp(yarp::os::YARP_CLOCK_SYSTEM), sigHandler(this)
{
    ptr_portCallback = NULL;
    setOptionsToDefault();
    _pOutPort = NULL;

    connect(&sigHandler,SIGNAL(sendFrame(QVideoFrame*)),&videoProducer,
            SLOT(onNewVideoContentReceived(QVideoFrame*)),Qt::DirectConnection);

    connect(&sigHandler,SIGNAL(sendFps(double,double,double,double,double,double)),
            this, SLOT(onSendFps(double,double,double,double,double,double)));

    connect(&videoProducer,SIGNAL(resizeWindowRequest()),
            this, SLOT(onWindowSizeChangeRequested()));

    createObjects();
}

QtYARPView::~QtYARPView()
{
    closePorts();
    deleteObjects();

    if (_options.m_saveOnExit != 0){
        saveOptFile(_options.m_fileName);
    }
}

/*! \brief Freeze the video stream.
 *
 *  \param check a bool parameter that enbales or disables the freeze state
 */
void QtYARPView::freeze(bool check)
{
    sigHandler.freeze(check);
}

/*! \brief returns the videoproducer that acts as bridge between C++ code and QML VideoOutput.
 *
 *  \return A pointer to the videoproducer
 */
QObject *QtYARPView::getVideoProducer()
{
    return &videoProducer;
}

/*! \brief Returns the x position from the options.*/
int QtYARPView::posX()
{
    return _options.m_posX;
}

/*! \brief Returns the y position from the options.*/
int QtYARPView::posY()
{
    return _options.m_posY;
}

/*! \brief Returns the width from the options.*/
int QtYARPView::windowWidth()
{
    return _options.m_windWidth;
}

/*! \brief Returns the height from the options.*/
int QtYARPView::windowHeight()
{
    return _options.m_windHeight;
}


/*! \brief Synchs the video stream to the display.
 *
 *  \param check a bool parameter that enables or disables the synch option
 */
void QtYARPView::synchDisplayPeriod(bool check)
{
    sigHandler.synchDisplayPeriod(check);
}

/*! \brief Synchs the size of the window with the size of the video stream.
*
*  \param check a bool parameter that enables or disables the synch option
*/
void QtYARPView::synchDisplaySize(bool check)
{
    sigHandler.synchDisplaySize(check);
}

/*! \brief Changes the refresh interval.
 *
 *  \param interval an int value representing the interval
 */
void QtYARPView::changeRefreshInterval(int interval)
{
    sigHandler.changeRefreshInterval(interval);
}

/*! \brief Saves the current frame. */
void QtYARPView::saveFrame()
{
    sigHandler.saveCurrentFrame();
}

/*! \brief Sets the current filename of the frame is going to be saved.
 *
 *  \param url the url to the file
 */
void QtYARPView::setFileName(QUrl url)
{
    sigHandler.setFileName(url);
}

/*! \brief Sets the current filename of the frame set is going to be saved.
 *
 *  \param url the url to the file
 */
void QtYARPView::setFilesName(QUrl url)
{
    sigHandler.setFileNames(url);
}

/*! \brief Saves the current frame set. */
void QtYARPView::startDumpFrames()
{
    sigHandler.startDumpFrames();
}

/*! \brief Stops the current frame set save mode. */
void QtYARPView::stopDumpFrames()
{
    sigHandler.stopDumpFrames();
}

void QtYARPView::periodToFreq(double avT, double mT, double MT, double &avH, double &mH, double &MH)
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

void QtYARPView::onSendFps(double portAvg, double portMin, double portMax,
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
int QtYARPView::refreshInterval()
{
    return _options.m_refreshTime;
}


/*! \brief Creates the input port and the port callback.*/
void QtYARPView::createObjects() {
#ifdef YARP_LITTLE_ENDIAN
    ptr_inputPort = new  yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgra> >;
#else
    ptr_inputPort = new  yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgba> >;
#endif
    ptr_portCallback = new InputCallback;
    ptr_portCallback->setSignalHandler(&sigHandler);
}

/*! \brief Deletes the input port and the port callback.*/
void QtYARPView::deleteObjects() {
    if (ptr_inputPort!=0)
        delete ptr_inputPort;
    if (ptr_portCallback!=0)
        delete ptr_portCallback;
}

/*! \brief parse the parameters received from the main container in QstringList form
    \param params the parameter list
*/
bool QtYARPView::parseParameters(QStringList params)
{
    Property options;
    int c = params.count();
    char **v;
    v = (char**)malloc(sizeof(char*) * c);

    for(int i=0;i<params.count();i++){
        v[i] = strdup(params.at(i).toLatin1().data());
    }

    options.fromCommand(c,v,false);

    for(int i=0;i<params.count();i++) {
        free(v[i]);
    }
    free(v);

    // If the user asks for help, let's print it and return.
    if (options.check("help"))
    {
        printHelp();
        return false;
    }

    if (!yarp::os::Network::checkNetwork()) {
        qCritical("Cannot connect to yarp server");
        return false;
    }

    // Otherwise, simply set the options asked
    setOptions(options);

    if (!openPorts()){
        qDebug("Error open ports");
        return false;
    }
    ptr_inputPort->useCallback(*ptr_portCallback);
    return true;
}

/*! \brief Sets the options received from command line*/
void QtYARPView::setOptions(yarp::os::Searchable& options) {
    // switch to subsections if available
    yarp::os::Value *val;
    if (options.check("PortName",val)||options.check("name",val)) {
        qsnprintf(_options.m_portName, 256, "%s", val->asString().c_str());
        qDebug("%s", val->asString().c_str());
    }
    if (options.check("NetName",val)||options.check("n",val)) {
        qsnprintf(_options.m_networkName, 256, "%s", val->asString().c_str());
    }
    if (options.check("OutPortName",val)||options.check("out",val)) {
        qsnprintf(_options.m_outPortName, 256, "%s", val->asString().c_str());
    }
    if (options.check("OutNetName",val)||options.check("neto",val)) {
        qsnprintf(_options.m_outNetworkName, 256, "%s", val->asString().c_str());
    }
    if (options.check("RefreshTime",val)||options.check("p",val)) {
        _options.m_refreshTime = val->asInt();
        sigHandler.changeRefreshInterval(_options.m_refreshTime);
        refreshIntervalChanged();
    }
    if (options.check("PosX",val)||options.check("x",val)) {
        _options.m_posX = val->asInt();
        posXChanged();
    }
    if (options.check("PosY",val)||options.check("y",val)) {
        _options.m_posY = val->asInt();
        posYChanged();
    }
    if (options.check("Width",val)||options.check("w",val)) {
        _options.m_windWidth = val->asInt();
        widthChanged();
    }
    if (options.check("Height",val)||options.check("h",val)) {
        _options.m_windHeight = val->asInt();
        heightChanged();
    }
    if (options.check("OutputEnabled",val)) {
        _options.m_outputEnabled = val->asInt();
    }
    if (options.check("out",val)) {
        _options.m_outputEnabled = true;
    }
    if (options.check("SaveOptions",val)||options.check("saveoptions",val)) {
        _options.m_outputEnabled = val->asInt();
    }
    if (options.check("synch"))
    {
        _options.m_synchRate=true;
        synchDisplayPeriod(true);
        synchRate(true);
    }
    if (options.check("autosize"))
    {
        _options.m_autosize = true;
        synchDisplaySize(true);
        autosize(true);
    }
}

/*! \brief Prints the help menu.*/
void QtYARPView::printHelp()
{
    qDebug("yarpview usage:");
    qDebug("  --name: input port name (default: /yarpview/img:i)");
    qDebug("  --x: x position of the window in the screen");
    qDebug("  --y: y position of the window in the screen");
    qDebug("  --w: width of the window");
    qDebug("  --h: height of the window");
    qDebug("  --p: refresh time [ms]");
    qDebug("  --synch: synchronous display, every image received by the input port is displayed");
    qDebug("  --autosize: the display automatically resizes on every new frame");
    qDebug("  --out: output port name (no default is given, if this option is not specified the port is not created)");
    qDebug("  --neto: output network");
    qDebug("  --neti: input network");
    qDebug("  --compact: if this flag is enabled, no status bar will be showed");
    qDebug("  --minimal: if this flag is enabled, neither the status bars nor the title bar/menu bar will be showed. It overwrites --compact.");
    qDebug("  --keep-above: keep windows above others");
}

/*! \brief Stes the options to their defaults.*/
void QtYARPView::setOptionsToDefault()
{
    // Options defaults
    _options.m_refreshTime = 100;
    qsnprintf(_options.m_portName, 256, "%s","/yarpview/img:i");
    qsnprintf(_options.m_networkName, 256, "%s", "default");
    qsnprintf(_options.m_outPortName, 256, "%s","/yarpview/o:point");
    qsnprintf(_options.m_outNetworkName, 256, "%s", "default");
    _options.m_outputEnabled = 0;
    _options.m_windWidth = 300;
    _options.m_windHeight = 300;
    _options.m_posX = 100;
    _options.m_posY = 100;
    _options.m_autosize = false;
    qsnprintf(_options.m_fileName, 256, "%s","yarpview.conf");
    _options.m_saveOnExit = 0;

    posXChanged();
    posYChanged();
    widthChanged();
    heightChanged();

    sigHandler.changeRefreshInterval(_options.m_refreshTime);
    refreshIntervalChanged();
}


/*! \brief Opens the ports.
    \return false in case of error, true otherwise
*/
bool QtYARPView::openPorts()
{
    bool ret = false;

    ptr_inputPort->setReadOnly();
    ret= ptr_inputPort->open(_options.m_portName);
    setName(ptr_inputPort->getName().c_str());

    if (!ret){
        qDebug("Error: port failed to open, quitting.");
        return false;
    }

    if (_options.m_outputEnabled == 1){
        _pOutPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
        qDebug("Registering port %s on network %s...", _options.m_outPortName, _options.m_outNetworkName);
        bool ok = _pOutPort->open(_options.m_outPortName);
        if (ok) {
            qDebug("Port registration succeed!");
        }
        else{
            _pOutPort = NULL;
            qDebug("ERROR: Port registration failed.\nQuitting, sorry.");
            return false;
        }
    }

    return true;
}

/*! \brief Closes the ports.*/
void QtYARPView::closePorts()
{

    ptr_inputPort->close();

    if (_options.m_outputEnabled == 1 && _pOutPort){
        _pOutPort->close();
        bool ok = true;
        if  (ok)
            qDebug("Port %s unregistration succeed!\n", _options.m_outPortName);
        else
            qDebug("ERROR: Port %s unregistration failed.\n", _options.m_outPortName);
        delete _pOutPort;
        _pOutPort = NULL;
    }
}

/*! \brief Save the options to a file.
    \param fileName the filename
*/
void QtYARPView::saveOptFile(char *fileName)
{
    //FILE *optFile = NULL;
    QFile optFile(QString("%1").arg(fileName));

    bool opened = optFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    if (!opened){
        qDebug("ERROR: Impossible to save to option file.");
        return;
    }
    optFile.write((QString("PortName %1\n").arg(_options.m_portName)).toLatin1().data());
    optFile.write((QString("NetName %1\n").arg(_options.m_networkName)).toLatin1().data());
    optFile.write((QString("OutPortName %1\n").arg(_options.m_outPortName)).toLatin1().data());
    optFile.write((QString("OutNetName %s\n").arg(_options.m_outNetworkName)).toLatin1().data());
    optFile.write((QString("RefreshTime %d\n").arg(_options.m_refreshTime)).toLatin1().data());
    optFile.write((QString("PosX %d\n").arg(_options.m_posX)).toLatin1().data());
    optFile.write((QString("PosY %d\n").arg(_options.m_posY)).toLatin1().data());
    optFile.write((QString("Width %d\n").arg(_options.m_windWidth)).toLatin1().data());
    optFile.write((QString("Height %d\n").arg(_options.m_windHeight)).toLatin1().data());
    optFile.write((QString("OutputEnables %d\n").arg(_options.m_outputEnabled)).toLatin1().data());
    optFile.write((QString("SaveOptions %d\n").arg(_options.m_saveOnExit)).toLatin1().data());
    optFile.write((QString("synchRate %d\n").arg(_options.m_synchRate)).toLatin1().data());
    optFile.write((QString("autosize %d\n").arg(_options.m_autosize)).toLatin1().data());
    optFile.close();
}

void QtYARPView::clickCoords_4(int start_x, int start_y, int end_x, int end_y)
{
    int imageWidth, imageHeight;

    imageWidth = videoProducer.getWidth();
    imageHeight = videoProducer.getHeight();

    if ((imageWidth != 0) && (imageHeight != 0)) {
        qDebug("Transmitting click information...");
        if (_pOutPort != NULL) {
            yarp::os::Bottle& bot = _pOutPort->prepare();
            bot.clear();
            bot.addInt(start_x);
            bot.addInt(start_y);
            bot.addInt(end_x);
            bot.addInt(end_y);
            //_pOutPort->Content() = _outBottle;
            _pOutPort->write();
        }

    }
    else {
        qDebug("I would send a position, but there's no image for scaling");
    }
}

void QtYARPView::clickCoords_2(int x,int y)
{
    int imageWidth, imageHeight;

    imageWidth = videoProducer.getWidth();
    imageHeight = videoProducer.getHeight();

    if ( (imageWidth != 0) && (imageHeight != 0) ) {
        qDebug("Transmitting click information...");
        if (_pOutPort!=NULL) {
            yarp::os::Bottle& bot = _pOutPort->prepare();
            bot.clear();
            bot.addInt(x);
            bot.addInt(y);
            //_pOutPort->Content() = _outBottle;
            _pOutPort->write();
        }

    } else {
        qDebug("I would send a position, but there's no image for scaling");
    }
}

void QtYARPView::onWindowSizeChangeRequested()
{
    if (sigHandler.getAutosizeMode())
    {
        emit sizeChanged();
    }
}

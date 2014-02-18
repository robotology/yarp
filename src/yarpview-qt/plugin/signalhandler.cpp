#include "signalhandler.h"
#include <QUrl>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
SignalHandler::SignalHandler(QObject *parent) :
    QObject(parent),timer(this)
{

    saveSetFrameMode = false;
    saveCurrentFrameMode = false;
    freezeMode = false;
    synchMode = false;
    defaultNameCounter = 0;
    customNameCounter = 0;
    framesetCounter = 0;

    checkDefaultNameCounterCount();

    fpsTimer.setSingleShot(false);
    fpsTimer.setInterval(3000);

    timer.setTimerType(Qt::PreciseTimer);

    connect(this,SIGNAL(internalSendFrame(QVideoFrame)),
            this,SLOT(internalReceiveFrame(QVideoFrame)),Qt::QueuedConnection);

    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimerElapsed()),Qt::QueuedConnection);

    connect(this,SIGNAL(selfStartTimer()),
            &timer,SLOT(start()),Qt::QueuedConnection);

    connect(&fpsTimer,SIGNAL(timeout()),
            this,SLOT(onFpsTimer()),Qt::DirectConnection);
    fpsTimer.start();


}

SignalHandler::~SignalHandler()
{
    disconnect(this,SIGNAL(internalSendFrame(QVideoFrame)),
            this,SLOT(internalReceiveFrame(QVideoFrame)));

    disconnect(&timer,SIGNAL(timeout()),this,SLOT(onTimerElapsed()));

    disconnect(this,SIGNAL(selfStartTimer()),&timer,SLOT(start()));
    if(timer.isActive()){
        timer.stop();
    }
}

/*! \brief Self start the timer of the display.
 *
 *  This method is used to start a timer because of the fact that a timer cannot be started
 *  from a thread different from the on it was created in
 */
void SignalHandler::onSelfStartTimer()
{
    timer.start();
}

/*! \brief Gets a videoframe.
 *
 *   this function gets a videframe and redirect it to itself
 *
 *  \param f The FrameVideo
 */
void SignalHandler::sendVideoFrame(QVideoFrame f)
{
    portFps.update();

    if(synchMode){
        displayFps.update();
        internalSendFrame(f);
    }else{
        mutex.lock();
        frame = f;
        if(!timer.isActive()){
            selfStartTimer();
        }
        mutex.unlock();

    }

    if(saveCurrentFrameMode){
        saveCurrentFrameMode = false;
        f.map(QAbstractVideoBuffer::ReadOnly);
        QImage img = QImage(f.bits(),f.width(),f.height(),QImage::Format_RGB32);
        f.unmap();
        saveFrame(img);
    }

    if(saveSetFrameMode){
        f.map(QAbstractVideoBuffer::ReadOnly);
        QImage img = QImage(f.bits(),f.width(),f.height(),QImage::Format_RGB32);
        f.unmap();
        saveFrameSet(img);
    }

}

/*! \brief Receive a videoframe.
 *
 *  Receive a videoframe from the internal and sends it with a signal to the
 *  videoproducer
 *
 *  \param f The FrameVideo
 */
void SignalHandler::internalReceiveFrame(QVideoFrame f)
{
    if(!freezeMode){
        sendFrame(&f);
    }
}

/*! \brief Enable/Disable the synch mode.
 *
 *  \param check
 */
void SignalHandler::synchToDisplay(bool check)
{
    synchMode = check;
    if(synchMode){
        if(timer.isActive()){
            timer.stop();
        }
    }else{
        if(frame.isValid() && !timer.isActive()){
            timer.start();
        }
    }
}

/*! \brief Enable/Disable the freeze mode.
 *
 *  \param check
 */
void SignalHandler::freeze(bool check)
{
    freezeMode = check;
}

/*! \brief Sets the refresh interval.
 *
 *  \param interval
 */
void SignalHandler::changeRefreshInterval(int interval)
{
    timer.setInterval(interval);
}



void SignalHandler::onTimerElapsed()
{
    displayFps.update();
    mutex.lock();
    internalReceiveFrame(frame);
    mutex.unlock();
}

/*! \brief Enable the save curretn frame  mode.
 */
void SignalHandler::saveCurrentFrame()
{
    saveCurrentFrameMode = true;
}

/*! \brief Sets the filename used for saving a video frame.
 *
 *  \param url url to the file
 */
void SignalHandler::setFileName(QUrl url)
{

    this->fileName = url.toLocalFile();
    customNameCounter = 0;
    checkCustomNameCounterCount(this->fileName);
}

/*! \brief saves the image
 *
 *  \param img the image
 */
void SignalHandler::saveFrame(QImage img)
{
    if(fileName.isEmpty()){
        QString dir = QCoreApplication::applicationDirPath();
        QString name = QString("%1/frame%2.ppm").arg(dir).arg(defaultNameCounter,3,10,QChar('0'));
        img.save(name,"PPM");
        defaultNameCounter++;
    }else{
        QString name = QString("%1%2.ppm").arg(fileName).arg(customNameCounter,3,10,QChar('0'));
        img.save(name,"PPM");
        customNameCounter++;
    }
}


/*! \brief saves the image set
 *
 *  \param img the image
 */
void SignalHandler::saveFrameSet(QImage img)
{
    if(fileNames.isEmpty()){
        QString dir = QCoreApplication::applicationDirPath();
        QString name = QString("%1/frameset%2.ppm").arg(dir).arg(framesetCounter);
        img.save(name,"PPM");
        framesetCounter++;
    }else{
        QString name = QString("%1%2.ppm").arg(fileNames).arg(framesetCounter);
        img.save(name,"PPM");
        framesetCounter++;
    }
}


/*! \brief Checks the default Save Image name Count
    it is used to continue the enumeration of the default image name
    in case the user does not specify a custom name.
*/
void SignalHandler::checkDefaultNameCounterCount()
{
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    QStringList filter;
    filter.append("*.ppm");
    QFileInfoList fil = dir.entryInfoList(filter,QDir::Files|QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> li(fil);

    while(li.hasNext()){
        QFileInfo fi = li.next();
        if(fi.fileName().contains("frame") )
            defaultNameCounter++;
    }

}

/*! \brief Checks the Custom Save Image name Count
    it is used to continue the enumeration of the custom image name
    in case the user specify an existing file name
*/
void SignalHandler::checkCustomNameCounterCount(QString file)
{
    int index = file.lastIndexOf("/");
    QString sdir = file.left(index);
    QString sfile = file.right(file.length() - index -1);

    QDir dir = QDir(sdir);
    QStringList filter;
    filter.append("*.ppm");
    QFileInfoList fil = dir.entryInfoList(filter,QDir::Files|QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> li(fil);

    while(li.hasNext()){
        QFileInfo fi = li.next();
        if(fi.fileName().contains(sfile) )
            customNameCounter++;
    }

}

/*! \brief Sets the filename used for saving a video frame set.
 *
 *  \param url url to the file
 */
void SignalHandler::setFileNames(QUrl url)
{
    if(saveSetFrameMode == false)
        this->fileNames = url.toLocalFile();
}

/*! \brief Enables the Dump frame modality (Save frame set).*/
void SignalHandler::startDumpFrames()
{
    framesetCounter = 0;
    saveSetFrameMode = true;
}

/*! \brief Stops the Dump frame modality (Save frame set).*/
void SignalHandler::stopDumpFrames()
{
    saveSetFrameMode = false;
}


void SignalHandler::onFpsTimer()
{
    double pAvg,pMin,pMax,dAvg,dMin,dMax;
    portFps.getStats(pAvg,pMin,pMax);
    portFps.reset();
    displayFps.getStats(dAvg,dMin,dMax);
    displayFps.reset();

    sendFps(pAvg,pMin,pMax,dAvg,dMin,dMax);
}

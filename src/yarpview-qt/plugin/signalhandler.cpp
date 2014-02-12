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

void SignalHandler::onSelfStartTimer()
{
    timer.start();
}

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

void SignalHandler::internalReceiveFrame(QVideoFrame f)
{
    if(!freezeMode){
        sendFrame(&f);
    }
}

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

void SignalHandler::freeze(bool check)
{
    freezeMode = check;
}

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

void SignalHandler::saveCurrentFrame()
{
    saveCurrentFrameMode = true;
}

void SignalHandler::setFileName(QUrl url)
{

    this->fileName = url.toLocalFile();
    customNameCounter = 0;
    checkCustomNameCounterCount(this->fileName);
}

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


void SignalHandler::setFileNames(QUrl url)
{
    if(saveSetFrameMode == false)
        this->fileNames = url.toLocalFile();
}

void SignalHandler::startDumpFrames()
{
    framesetCounter = 0;
    saveSetFrameMode = true;
}

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

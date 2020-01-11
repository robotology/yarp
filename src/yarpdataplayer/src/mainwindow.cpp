/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QCheckBox>
#include <QShortcut>
#include <QProgressBar>
#include <include/aboutdlg.h>
#include <QMessageBox>
#include "include/log.h"
#include <csignal>
#include <yarp/conf/version.h>

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#define WND_DEF_HEIGHT          400
#define WND_DEF_WIDTH           800

#define ACTIVE      0
#define PART        1
#define TYPE        2
#define FRAMES      3
#define SAMPLERATE  4
#define PORT        5
#define TIMETAKEN   6
#define PERCENT     7


#ifndef APP_NAME
 #define APP_NAME "yarpdataplayer"
#endif

using namespace std;
using namespace yarp::os;

void sighandler(int sig)
{
    LOG("\n\nCaught ctrl-c, please quit within gui for clean exit\n\n");
}

/**********************************************************/
MainWindow::MainWindow(yarp::os::ResourceFinder &rf, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loadingWidget(this)
{
    ui->setupUi(this);
    setWindowTitle(APP_NAME);
    utilities = nullptr;
    pressed = false;
    initThread = nullptr;

    moduleName =  QString("%1").arg(rf.check("name", Value("yarpdataplayer"), "module name (string)").asString().c_str());

    if (rf.check("withExtraTimeCol")){
        withExtraTimeCol = true;
        column = rf.check("withExtraTimeCol",Value(1)).asInt32();

        if (column < 1 || column > 2 ){
            column = 1;
        }

        LOG( "Selected timestamp column to check is %d \n", column);

    } else {
        withExtraTimeCol = false;
        column = 0;
    }

    add_prefix = rf.check("add_prefix");
    createUtilities();

    subDirCnt = 0;
    utilities = nullptr;
    setWindowTitle(moduleName);
    setupActions();
    setupSignals();

    QString port = QString("/%1/rpc:i").arg(moduleName);
    rpcPort.open( port.toLatin1().data() );

    ::signal(SIGINT, sighandler);
    ::signal(SIGTERM, sighandler);

    attach(rpcPort);

    quitFromCmd = false;

    connect(ui->mainWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(internalLoad(QString)),this,SLOT(onInternalLoad(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(internalPlay()),this,SLOT(onInternalPlay()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalPause()),this,SLOT(onInternalPause()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalStop()),this,SLOT(onInternalStop()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalStep(Bottle*)),this,SLOT(onInternalStep(Bottle*)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalSetFrame(std::string,int)),this,SLOT(onInternalSetFrame(std::string,int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalGetFrame(std::string, int*)),this,SLOT(onInternalGetFrame(std::string,int*)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalQuit()),this,SLOT(onInternalQuit()),Qt::QueuedConnection);
    connect(this,SIGNAL(internalGetSliderPercentage(int*)),this,SLOT(onInternalGetSliderPercentage(int*)),Qt::BlockingQueuedConnection);

    QShortcut *openShortcut = new QShortcut(QKeySequence("Ctrl+O"), parent);
    QObject::connect(openShortcut, SIGNAL(activated()), this, SLOT(onInternalLoad(QString)));

    QShortcut *closeShortcut = new QShortcut(QKeySequence("Ctrl+Q"), parent);
    QObject::connect(closeShortcut, SIGNAL(activated()), this, SLOT(onInternalQuit()));

}

/**********************************************************/
MainWindow::~MainWindow()
{
    delete ui;
    LOG("cleaning up rpc port...\n");
    rpcPort.close();
    LOG("done cleaning rpc port...\n");
    clearUtilities();
}

/**********************************************************/
void MainWindow::onItemDoubleClicked(QTreeWidgetItem *item,int column)
{
    if(column == 5){
        //ui->treeWidget->openPersistentEditor(item,column);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        return;
    }else{
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
}

/**********************************************************/
bool MainWindow::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}

/**********************************************************/
bool MainWindow::step()
{
    Bottle reply;
    emit internalStep(&reply);
    if (reply.toString() == "error"){
        return false;
    }
    if (reply.toString() == "ok"){
        return true;
    }
    return false;
}

/**********************************************************/
void MainWindow::onInternalStep(Bottle *reply)
{
    stepFromCommand(*reply);
}

/**********************************************************/
bool MainWindow::setFrame(const string &name, const int frameNum)
{
    emit internalSetFrame(name,frameNum);
    return true;
}

/**********************************************************/
void MainWindow::onInternalSetFrame(const string &name, const int frameNum)
{
    updateFrameNumber(name.c_str(), frameNum);
}

/**********************************************************/
int MainWindow::getFrame(const string &name)
{
    int frame = 0;
    emit internalGetFrame(name,&frame);
    if (frame < 1){
        return -1;
    } else {
        return frame;
    }
}

/**********************************************************/
void MainWindow::onInternalGetFrame(const string &name, int *frame)
{
    getFrameCmd(name.c_str(),frame);
}

/**********************************************************/
int MainWindow::getSliderPercentage()
{
    int percentage = 0;
    emit internalGetSliderPercentage(&percentage);
    if (percentage < 1){
        return -1;
    } else {
        return percentage;
    }
}

/**********************************************************/
void MainWindow::onInternalGetSliderPercentage(int *percentage)
{
    *percentage = ui->playSlider->value();
}

/**********************************************************/
bool MainWindow::load(const string &path)
{
    string cmdPath = path;
    QString sPath = QString("%1").arg(path.c_str());

    size_t slashErr = cmdPath.find('/');

    if (slashErr == string::npos){
        LOG_ERROR("Error, please make sure you are using forward slashes '/' in path.\n");
        return false;
    } else {
        emit internalLoad(sPath);
    }

    waitMutex.lock();
    waitCond.wait(&waitMutex);
    waitMutex.unlock();

    if (subDirCnt <= 0 ){
        return false;
    }

    return true;
}

/**********************************************************/
void  MainWindow::onInternalLoad(QString sPath)
{
    ui->mainWidget->clear();
    for (int x=0; x < subDirCnt; x++){
        utilities->closePorts(utilities->partDetails[x]);
    }

    doGuiSetup(sPath);
}

/**********************************************************/
bool MainWindow::play()
{
    emit internalPlay();
    return true;
}

/**********************************************************/
void MainWindow::onInternalPlay()
{
    onMenuPlayBackPlay();
}

/**********************************************************/
bool MainWindow::pause()
{
    emit internalPause();
    return true;
}

/**********************************************************/
void MainWindow::onInternalPause()
{
    onMenuPlayBackPause();
}

/**********************************************************/
bool MainWindow::stop()
{
    emit internalStop();
    return true;
}

/**********************************************************/
void MainWindow::onInternalStop()
{
    onMenuPlayBackStop();
}

/**********************************************************/
bool MainWindow::quit()
{
    quitFromCmd = true;
    emit internalQuit();
    return true;
}

/**********************************************************/
void MainWindow::onInternalQuit()
{
    if(cmdSafeExit()){
        QMainWindow::close();
    }
}

/**********************************************************/
bool MainWindow::updateFrameNumber(const char* part, int frameNum)
{
    if (subDirCnt > 0){
        LOG("setting initial frame to %d\n",frameNum);
        //if (frameNum == 0)
            //frameNum = 1;

        for (auto& itr : partMap){
            utilities->masterThread->virtualTime = utilities->partDetails[itr.second].timestamp[utilities->partDetails[itr.second].currFrame];
            utilities->partDetails[itr.second].currFrame = frameNum;
        }
        utilities->masterThread->virtualTime = utilities->partDetails[0].timestamp[utilities->partDetails[0].currFrame];
        return true;
    } else {
        return false;
    }
}

/**********************************************************/
void MainWindow::getFrameCmd( const char* part , int *frame)
{
    if (subDirCnt > 0){
        for (auto& itr : partMap) {
            if (strcmp (part, itr.first) == 0) {
                *frame = utilities->partDetails[itr.second].currFrame;
            }
        }
    }
}

/**********************************************************/
void MainWindow::stepFromCommand(Bottle &reply)
{
    if (subDirCnt > 0){
        utilities->stepThread();
        reply.addString("ok");
    } else {
        reply.addString("error");
    }
}

/**********************************************************/
bool MainWindow::cmdSafeExit()
{
    quitFromCmd = true;
    if(utilities){
        LOG( "asking the threads to stop...\n");
        if (utilities->masterThread->isSuspended()){
            utilities->masterThread->resume();
        }

        utilities->masterThread->stop();
        LOG( "done stopping!\n");
        for (int i=0; i < subDirCnt; i++)
            utilities->partDetails[i].currFrame = 1;

        LOG( "Module closing...\nCleaning up...\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->partDetails[x].worker->release();
        }
        LOG( "Attempt to interrupt ports\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->interruptPorts(utilities->partDetails[x]);
        }
        LOG( "Attempt to close ports\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->closePorts(utilities->partDetails[x]);
        }
        clearUtilities();
        LOG( "Done!...\n");
    }
    return true;
}

/**********************************************************/
bool MainWindow::safeExit()
{
    if(utilities){
        LOG( "asking the threads to stop...\n");
        if (utilities->masterThread->isSuspended()){
            utilities->masterThread->resume();
        }

        utilities->masterThread->stop();
        LOG( "done stopping!\n");
        for (int i=0; i < subDirCnt; i++)
            utilities->partDetails[i].currFrame = 1;

        LOG( "Module closing...\nCleaning up...\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->partDetails[x].worker->release();
        }
        LOG( "Attempt to interrupt ports\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->interruptPorts(utilities->partDetails[x]);
        }
        LOG( "Attempt to close ports\n");
        for (int x=0; x < subDirCnt; x++){
            utilities->closePorts(utilities->partDetails[x]);
        }
        clearUtilities();
        LOG( "Done!...\n");
    }
    return true;
}

/**********************************************************/
void MainWindow::createUtilities()
{
    if(!utilities){
        utilities = new Utilities(moduleName.toLatin1().data(),add_prefix,this);
        utilities->withExtraColumn = withExtraTimeCol;
        utilities->column = column;
    }
}

/**********************************************************/
void MainWindow::clearUtilities()
{
    if(utilities){
        delete utilities;
        utilities = nullptr;
    }
}

/**********************************************************/
bool MainWindow::getPartActivation(const char* szName)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));


    if(row){
        QCheckBox *check = ((QCheckBox*)ui->mainWidget->itemWidget(row,ACTIVE));
        if(check){
            return check->isChecked();
        }
    }
    return false;
}

/**********************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!quitFromCmd){
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, APP_NAME,
                                                               "Quitting, Are you sure?\n",
                                                               QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

        if (resBtn != QMessageBox::Yes) {
            event->ignore();

        } else {
            safeExit();
            event->accept();
        }
    }
}

/**********************************************************/
void MainWindow::setupSignals()
{
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(onSpeedValueChanged(int)));
    connect(ui->playSlider,SIGNAL(sliderPressed()),this,SLOT(onSliderPressed()));
    connect(ui->playSlider,SIGNAL(sliderReleased()),this,SLOT(onSliderReleased()));

    connect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackStop()));
    connect(ui->ffwButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackForward()));
    connect(ui->rewButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackBackward()));
}

/**********************************************************/
void MainWindow::setupActions()
{
    connect(ui->actionOpen_Directory,SIGNAL(triggered()),this,SLOT(onMenuFileOpen()));
    connect(ui->actionPlay,SIGNAL(triggered()),this,SLOT(onMenuPlayBackPlay()));
    connect(ui->actionPause,SIGNAL(triggered()),this,SLOT(onMenuPlayBackPause()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(onMenuPlayBackStop()));
    connect(ui->actionForward,SIGNAL(triggered()),this,SLOT(onMenuPlayBackForward()));
    connect(ui->actionRewind,SIGNAL(triggered()),this,SLOT(onMenuPlayBackBackward()));
    connect(ui->actionStrict,SIGNAL(triggered()),this,SLOT(onMenuPlayBackStrict()));
    connect(ui->actionRepeat,SIGNAL(triggered()),this,SLOT(onMenuPlayBackRepeat()));
    connect(ui->actionIncrease,SIGNAL(triggered()),this,SLOT(onMenuSpeedUp()));
    connect(ui->actionDecrease,SIGNAL(triggered()),this,SLOT(onMenuSpeedDown()));
    connect(ui->actionNormal_1x,SIGNAL(triggered()),this,SLOT(onMenuSpeedNormal()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(onMenuHelpAbout()));
    connect(ui->actionQuit,SIGNAL(triggered()),this,SLOT(onClose()));

}

/**********************************************************/
bool  MainWindow::doGuiSetup(QString newPath)
{
    if(initThread && initThread->isRunning()){
        return false;
    }

    clearUtilities();
    createUtilities();

    ui->statusBar->showMessage(newPath);//Write path to the gui
    //look for folders and log files associated with them
    LOG("the full path is %s \n", newPath.toLatin1().data());
    subDirCnt = 0;
    rowInfoVec.clear();


    itr = 0;
    partMap.clear();

    if(!initThread){
        initThread = new InitThread(utilities,newPath,rowInfoVec,this);
        connect(initThread,SIGNAL(initDone(int)),this,SLOT(onInitDone(int)),Qt::QueuedConnection);
        initThread->start();
    }else{
        if(!initThread->isRunning()){
            delete initThread;
            initThread = new InitThread(utilities,newPath,rowInfoVec,this);
            connect(initThread,SIGNAL(initDone(int)),this,SLOT(onInitDone(int)),Qt::QueuedConnection);
            initThread->start();
        }
    }

    loadingWidget.start();

    return true;
}

/**********************************************************/
void MainWindow::onInitDone(int subDirCount)
{
    subDirCnt = subDirCount;
    //add the parts to the gui
    for (int x=0; x < subDirCnt; x++){
        addPart(utilities->partDetails[x].name.c_str(), utilities->partDetails[x].type.c_str(), utilities->partDetails[x].maxFrame, utilities->partDetails[x].portName.c_str() );
        setInitialPartProgress(utilities->partDetails[x].name.c_str(), 0);
        utilities->configurePorts(utilities->partDetails[x]);
    }

    ui->playButton->setEnabled(true);
    ui->ffwButton->setEnabled(true);
    ui->rewButton->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->playSlider->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);

    ui->actionPlay->setEnabled(true);
    ui->actionForward->setEnabled(true);
    ui->actionRewind->setEnabled(true);
    ui->actionStop->setEnabled(true);

    ui->actionIncrease->setEnabled(true);
    ui->actionDecrease->setEnabled(true);
    ui->actionNormal_1x->setEnabled(true);
    ui->actionRepeat->setEnabled(true);
    ui->actionStrict->setEnabled(true);

    loadingWidget.accept();
    loadingWidget.stop();

    if(!errorMessage.isEmpty()){
        switch(QMessageBox::critical(this,"Setup Error",errorMessage,QMessageBox::Ok)){
            case(QMessageBox::Ok):{
                yInfo("OK clicked.");
                break;
            }
            default:{
                yError("Unexpected button clicked.");
                break;
            }
        }
    }

    errorMessage = "";
    waitCond.wakeAll();

}

/**********************************************************/
void MainWindow::addPart(const char* szName, const char* type, int frames, const char* portName, const char* szFileName )
{
    partMap[szName] = itr;
    auto* item = new QTreeWidgetItem();
    ui->mainWidget->addTopLevelItem(item);
    auto* checkBox = new QCheckBox();
    checkBox->setChecked(true);
    ui->mainWidget->setItemWidget(item,ACTIVE,checkBox);
    if(szName){
        item->setText(PART,QString("%1").arg(szName));
        ui->mainWidget->resizeColumnToContents(PART);
    }
    if(type){
        item->setText(TYPE,QString("%1").arg(type));
        ui->mainWidget->resizeColumnToContents(TYPE);
    }
    item->setText(FRAMES,QString("%1").arg(frames));
    ui->mainWidget->resizeColumnToContents(FRAMES);

    if(portName){
        item->setText(PORT,QString("%1").arg(portName));
        ui->mainWidget->resizeColumnToContents(PORT);
    }

    auto* progress = new QProgressBar();
    progress->setMaximum(100);
    progress->setValue(0);
    progress->setAlignment(Qt::AlignCenter);
    ui->mainWidget->setItemWidget(item,PERCENT,progress);

    itr++;
}

/**********************************************************/
bool MainWindow::setInitialPartProgress(const char* szName, int percentage)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));


    if(row){
        QProgressBar *progress = ((QProgressBar*)ui->mainWidget->itemWidget(row,PERCENT));
        if(progress){
            progress->setValue(percentage);
            return true;
        }
    }

    return false;
}

/**********************************************************/
bool MainWindow::setPartProgress(const char* szName, int percentage)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));
    if(row){
        QProgressBar *progress = ((QProgressBar*)ui->mainWidget->itemWidget(row,PERCENT));
        if(progress){
            progress->setValue(percentage);
            return true;
        }
    }
    return false;
}

/**********************************************************/
QTreeWidgetItem * MainWindow::getRowByPart(QString szName )
{

    for(int i=0;i<ui->mainWidget->topLevelItemCount();i++){
        if(ui->mainWidget->topLevelItem(i)->text(PART) == szName){
            return ui->mainWidget->topLevelItem(i);
        }
    }
    return nullptr;
}

/**********************************************************/
void MainWindow::onMenuFileOpen()
{
    if(ui->actionRepeat->isChecked())
    {
        LOG("repeat mode is activated, setting it to false\n");
        utilities->repeat = false;
        ui->actionRepeat->setChecked(false);
    }

    if (ui->actionStrict->isChecked())
    {
        LOG("send strict mode is activated, setting it to false\n");
        utilities->sendStrict = false;
        ui->actionStrict->setChecked(false);
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Please choose a folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty())
    {
        ui->mainWidget->clear();
        for (int x=0; x < subDirCnt; x++)
            utilities->closePorts(utilities->partDetails[x]);

        doGuiSetup(dir);
    }
}

/**********************************************************/
void MainWindow::onErrorMessage(QString msg)
{
    QString file = msg;

    if(loadingWidget.isVisible()){
        QString fullMessage;
        if(errorMessage.isEmpty()){
            fullMessage = QString( "There was a problem opening \n\n%1\n\nplease check its validity").arg(file);
            errorMessage = errorMessage + "\n" + fullMessage;
        }else{
            int index = errorMessage.indexOf("please check");
            QString aux = errorMessage.left(errorMessage.length() -(errorMessage.length() - index));
            errorMessage = aux + msg + "\n\nplease check their validity";
        }


        return;
    }else{
        QString fullMessage = QString( "There was a problem opening \n\n%1\n\nplease check its validity").arg(file);
        switch(QMessageBox::critical(this,"Setup Error",fullMessage,QMessageBox::Ok)){
            case(QMessageBox::Ok):{
                yInfo("OK clicked.");
                break;
            }
            default:{
                yError("Unexpected button clicked.");
                break;
            }
        }
    }
}

/**********************************************************/
void MainWindow::onMenuHelpAbout()
{
    QString copyright = "Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)";
    QString name = APP_NAME;
    QString version = YARP_VERSION;
    AboutDlg dlg(name,version,copyright,"https://www.iit.it/");
    dlg.exec();
}

/**********************************************************/
void MainWindow::onMenuPlayBackPlay()
{

    if (subDirCnt > 0){
        ui->playButton->setIcon(QIcon(":/pause.svg"));
        disconnect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));
        connect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPause()));

        ui->actionPause->setEnabled(true);
        ui->actionPlay->setEnabled(false);

        LOG("checking if port was changed by the user...\n");

        for (int i=0; i < subDirCnt; i++){
            QString test;
            getPartPort(utilities->partDetails[i].name.c_str(), &test);
            if (strcmp( test.toLatin1().data() , utilities->partDetails[i].portName.c_str()) == 0 ){
                LOG( "Port is the same continue\n");

            }else{
                LOG( "Modifying ports\n");
                utilities->partDetails[i].portName = test.toLatin1().data();//getPartPort( utilities->partDetails[i].name.c_str(), test);
                utilities->configurePorts(utilities->partDetails[i]);
            }
        }

        if ( utilities->masterThread->isSuspended() ) {
            LOG("asking the thread to resume\n");

            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].worker->resetTime();

            utilities->masterThread->resume();
        } else if (!utilities->masterThread->isRunning()) {
            LOG("asking the thread to start\n");
            LOG("initializing the workers...\n");

            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].worker->init();

            LOG("starting the master thread...\n");

            utilities->masterThread->start();
        }
        ui->playSlider->setEnabled(true);
    }
}

/**********************************************************/
void MainWindow::onMenuPlayBackPause()
{
    if (subDirCnt > 0){
        ui->playButton->setIcon(QIcon(":/play.svg"));
        disconnect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPause()));
        connect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));

        ui->actionPause->setEnabled(false);
        ui->actionPlay->setEnabled(true);

        LOG( "asking the threads to pause...\n");
        utilities->masterThread->pause();
    }
}

/**********************************************************/
void MainWindow::resetButtonOnStop()
{
    ui->playButton->setIcon(QIcon(":/play.svg"));
    disconnect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPause()));
    connect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));

    ui->actionPause->setEnabled(false);
    ui->actionPlay->setEnabled(true);
    ui->playSlider->setEnabled(false);
}

/**********************************************************/
void MainWindow::onMenuPlayBackStop()
{
    if (subDirCnt > 0){
        ui->playButton->setIcon(QIcon(":/play.svg"));
        disconnect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPause()));
        disconnect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));
        connect(ui->playButton,SIGNAL(clicked()),this,SLOT(onMenuPlayBackPlay()));

        LOG( "asking the threads to stop...\n");
        if (utilities->masterThread->isSuspended()){
            utilities->masterThread->resume();
        }

        utilities->masterThread->stop();
        LOG( "done stopping!\n");
        for (int i=0; i < subDirCnt; i++)
            utilities->partDetails[i].currFrame = 1;

        LOG( "done stopping the thread...\n");
        ui->playSlider->setEnabled(false);


        for (int i=0; i < subDirCnt; i++){
            setFrameRate(utilities->partDetails[i].name.c_str(), 0);
            setPartProgress( utilities->partDetails[i].name.c_str(), 0 );
            setFrameRate(utilities->partDetails[i].name.c_str(), 0);
        }
        setPlayProgress(0);
    }
}

/**********************************************************/
void MainWindow::onMenuPlayBackForward()
{
    if (subDirCnt > 0)
        utilities->masterThread->forward(5);
}

/**********************************************************/
void MainWindow::onMenuPlayBackBackward()
{
    if (subDirCnt > 0)
        utilities->masterThread->backward(5);
}

/**********************************************************/
void MainWindow::onMenuPlayBackStrict()
{
    if(ui->actionStrict->isChecked()){
        LOG("strict mode is activated\n");
        utilities->sendStrict = true;
    } else {
        LOG("strict mode is deactivated\n");
        utilities->sendStrict = false;
    }
}

/**********************************************************/
void MainWindow::onMenuPlayBackRepeat()
{
    if(ui->actionRepeat->isChecked()){
        LOG("repeat mode is activated\n");
        utilities->repeat = true;
    } else {
        LOG("repeat mode is deactivated\n");
        utilities->repeat = false;
    }
}

/**********************************************************/
void MainWindow::onMenuSpeedUp()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value() + 1);
    //m_hScale.set_value(m_hScale.get_value() + 1.0);
}

/**********************************************************/
void MainWindow::onMenuSpeedDown()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value() - 1);
}

/**********************************************************/
void MainWindow::onMenuSpeedNormal()
{
    ui->horizontalSlider->setValue(10);
}

/**********************************************************/
void MainWindow::onSpeedValueChanged(int val)
{
    double value = (double)val/10;
    value = (value>=1.0) ? value : (value+1.0)/2.0;
    char szValue[16];
    sprintf(szValue, "%.1fx", value);
    QString speed = QString("%1").arg(szValue);
    ui->speedValueLbl->setText(speed);
    // the range is [0.25 ... 4.0]
    if(utilities){
        utilities->speed = value;
    }
}

/**********************************************************/
void MainWindow::onSliderPressed()
{
    pressed = true;
}

/**********************************************************/
void MainWindow::onSliderReleased()
{
    int currValue = ui->playSlider->value();
    goToPercentage(currValue);
    pressed = false;
}

/**********************************************************/
bool MainWindow::getPartPort(const char* szName, QString *dest)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));
    if(row){
        *dest = row->text(PORT);
        return true;
    }
    LOG("returning null\n");

    return false;
}

/**********************************************************/
bool MainWindow::setFrameRate(const char* szName, int frameRate)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));
    if(row){
        row->setText(SAMPLERATE,QString("%1 ms").arg(frameRate));
        return true;
    }

    return false;
}

/**********************************************************/
bool MainWindow::setTimeTaken(const char* szName, double time)
{
    QTreeWidgetItem *row = nullptr;
    row = getRowByPart(QString("%1").arg(szName));
    if(row){
        row->setText(TIMETAKEN,QString("%1 s").arg(time, 0, 'f', 3));
        return true;
    }

    return false;
}

/**********************************************************/
void MainWindow::setPlayProgress(int percentage)
{
    if(pressed){
        return;
    }
    ui->playSlider->setValue(percentage);
}

/**********************************************************/
void MainWindow::onUpdateGuiRateThread()
{
    for (int i=0; i < subDirCnt; i++){
        //TODO SIGNAL

        if (getPartActivation(utilities->partDetails[i].name.c_str()) ){
            if ( utilities->partDetails[i].bot.get(1).asList()->get(2).isString() && utilities->partDetails[i].type == "Bottle"){
                //avoid checking frame rate for string data
                setFrameRate(utilities->partDetails[i].name.c_str(), 0);
            } else {
                if (utilities->partDetails[i].currFrame <= utilities->partDetails[i].maxFrame){
                    int rate = (int)utilities->partDetails[i].worker->getFrameRate();
                    setFrameRate(utilities->partDetails[i].name.c_str(),rate);

                    double time = utilities->partDetails[i].worker->getTimeTaken();

                    if (time > 700000){ //value of a time stamp...
                        setTimeTaken(utilities->partDetails[i].name.c_str(),0.0);
                    }else
                        setTimeTaken(utilities->partDetails[i].name.c_str(),time);
                }
            }
            //percentage = 0;
            percentage = ( utilities->partDetails[i].currFrame *100 ) / utilities->partDetails[i].maxFrame;
            setPartProgress( utilities->partDetails[i].name.c_str(), percentage );

            //LOG( "part %d is at frame %d of %d therefore %d\n",i, utilities->partDetails[i].currFrame, utilities->partDetails[i].maxFrame, percentage);
        }
        if(i == 0){
            setPlayProgress(percentage);
        }
    }
}

/**********************************************************/
void MainWindow::goToPercentage(int value)
{
    utilities->masterThread->goToPercentage(value);
}

/**********************************************************/
void MainWindow::onClose()
{
    //just send the closing event
    QMainWindow::close();
}

/**********************************************************/
InitThread::InitThread(Utilities *utilities,
                       QString  newPath,
                       std::vector<RowInfo>& rowInfoVec,
                       QObject *parent) : QThread(parent),
                                          utilities(utilities),
                                          newPath(std::move(newPath)),
                                          mainWindow(dynamic_cast<QMainWindow*> (parent)),
                                          rowInfoVec(rowInfoVec)
{
}

/**********************************************************/
void InitThread::run()
{
    utilities->resetMaxTimeStamp();
    int subDirCnt = utilities->getRecSubDirList(newPath.toLatin1().data(), rowInfoVec, 1);
    LOG("the size of subDirs is: %d\n", subDirCnt);
    //reset totalSent to 0
    utilities->totalSent = 0;
    utilities->totalThreads = subDirCnt;

    if (subDirCnt > 0){
        utilities->partDetails = new partsData [subDirCnt];//resize(subDirCnt);
    }

    //fill in parts with all data
    for (int x=0; x < subDirCnt; x++){
        utilities->partDetails[x].name = rowInfoVec[x].name;
        utilities->partDetails[x].infoFile = rowInfoVec[x].info;
        utilities->partDetails[x].logFile = rowInfoVec[x].log;
        utilities->partDetails[x].path = rowInfoVec[x].path;

        utilities->setupDataFromParts(utilities->partDetails[x]);

        utilities->partDetails[x].worker = new WorkerClass(x, subDirCnt);
        utilities->partDetails[x].worker->setManager(utilities);
    }

    //get the max timestamp of all the parts for synchronization
    if (subDirCnt > 0){
        utilities->getMaxTimeStamp();
    }

    if (subDirCnt > 0){
        utilities->getMinTimeStamp();
    }

    //set initial frames for all parts depending on first timestamps
    for (int x=0; x < subDirCnt; x++){
        utilities->initialFrame.push_back( utilities->partDetails[x].currFrame);

        double totalTime = 0.0;
        double final = utilities->partDetails[x].timestamp[utilities->partDetails[x].timestamp.length()-1];
        double initial = utilities->partDetails[x].timestamp[utilities->partDetails[x].currFrame];

        //LOG("initial timestamp is = %lf\n", initial);
        //LOG("final timestamp is  = %lf\n", final);

        totalTime = final - initial;

        LOG("The part %s, should last for: %lf with %d frames\n", utilities->partDetails[x].name.c_str(), totalTime, utilities->partDetails[x].maxFrame);

    }

    utilities->masterThread = new MasterThread(utilities, subDirCnt, mainWindow);
    //connect(utilities->masterThread,SIGNAL(updateGuiRateThread()),this,SLOT(onUpdateGuiRateThread()),Qt::QueuedConnection);
    utilities->masterThread->stepfromCmd = false;

    emit initDone(subDirCnt);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QCheckBox>
#include <QProgressBar>
#include <aboutdlg.h>
#include <QMessageBox>
#include "log.h"

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif


#define WND_DEF_HEIGHT          400
#define WND_DEF_WIDTH           800
#define CMD_HELP                VOCAB4('h','e','l','p')
#define CMD_STEP                VOCAB4('s','t','e','p')
#define CMD_QUIT                VOCAB4('q','u','i','t')
#define CMD_SET                 VOCAB3('s','e','t')
#define CMD_GET                 VOCAB3('g','e','t')
#define CMD_LOAD                VOCAB4('l','o','a','d')
#define CMD_PLAY                VOCAB4('p','l','a','y')
#define CMD_STOP                VOCAB4('s','t','o','p')
#define CMD_PAUSE               VOCAB4('p','a','u','s')



#define ACTIVE      0
#define PART        1
#define TYPE        2
#define FRAMES      3
#define SAMPLERATE  4
#define PORT        5
#define PERCENT     6



MainWindow::MainWindow(yarp::os::ResourceFinder &rf, QWidget *parent) :
    QMainWindow(parent),loadingWidget(this),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(APP_NAME);
    utilities = NULL;
    pressed = false;
    initThread = NULL;

    moduleName =  QString("%1").arg(rf.check("name", Value("dataSetPlayer"), "module name (string)").asString().c_str());

    if (rf.check("withExtraTimeCol")){
        withExtraTimeCol = true;
        column = rf.check("withExtraTimeCol",Value(1)).asInt();

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
    utilities = NULL;
    setWindowTitle(moduleName);
    setupActions();
    setupSignals();

    QString port = QString("/%1/rpc:i").arg(moduleName);
    rpcPort.open( port.toLatin1().data() );
    attach(rpcPort);

    connect(this,SIGNAL(internalLoad(QString)),this,SLOT(onInternalLoad(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(internalPlay()),this,SLOT(onInternalPlay()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalPause()),this,SLOT(onInternalPause()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalStop()),this,SLOT(onInternalStop()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalStep(Bottle*)),this,SLOT(onInternalStep(Bottle*)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalSetFrame(std::string,int)),this,SLOT(onInternalSetFrame(std::string,int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(internalGetFrame(std::string, int*)),this,SLOT(onInternalGetFrame(std::string,int*)),Qt::BlockingQueuedConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
    LOG("cleaning up rpc port...\n");
    rpcPort.close();
    LOG("done cleaning rpc port...\n");
    clearUtilities();
}

/************************************************************************/
bool MainWindow::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}

/************************************************************************/
bool MainWindow::step()
{
    Bottle reply;
    internalStep(&reply);
    if (reply.toString() == "error"){
        return false;
    }
    if (reply.toString() == "ok"){
        return true;
    }
    return false;
}

void MainWindow::onInternalStep(Bottle *reply)
{
    stepFromCommand(*reply);
}

/************************************************************************/
bool MainWindow::setFrame(const string &name, const int frameNum)
{
    internalSetFrame(name,frameNum);
    return true;
}

void MainWindow::onInternalSetFrame(const string &name, const int frameNum)
{
    updateFrameNumber(name.c_str(), frameNum);
}

/************************************************************************/
int MainWindow::getFrame(const string &name)
{
    int frame = 0;
    internalGetFrame(name,&frame);
    if (frame < 1){
        return -1;
    } else {
        return frame;
    }
}

void MainWindow::onInternalGetFrame(const string &name, int *frame)
{
    getFrameCmd(name.c_str(),frame);
}

/************************************************************************/
bool MainWindow::load(const string &path)
{
    string cmdPath = path.c_str();
    QString sPath = QString("%1").arg(path.c_str());

    size_t slashErr = cmdPath.find('/');

    if (slashErr == string::npos){
        LOG_ERROR("Error, please make sure you are using forward slashes '/' in path.\n");
        return false;
    }else{
        internalLoad(sPath);
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
    for (int x=0; x < subDirCnt; x++){
        utilities->closePorts(utilities->partDetails[x]);
    }

    doGuiSetup(sPath);


}

/**********************************************************/
bool MainWindow::play()
{
    internalPlay();
    return true;
}

void MainWindow::onInternalPlay()
{
    onMenuPlayBackPlay();
}

/**********************************************************/
bool MainWindow::pause()
{
    internalPause();
    return true;
}

void MainWindow::onInternalPause()
{
    onMenuPlayBackPause();
}

/**********************************************************/
bool MainWindow::stop()
{
    internalStop();
    return true;
}

void MainWindow::onInternalStop()
{
    onMenuPlayBackStop();
}


/**********************************************************/
bool MainWindow::quit()
{
    if(cmdSafeExit()){
        QMainWindow::close();
    }
    return true;
}

/**********************************************************/
bool MainWindow::updateFrameNumber(const char* part, int frameNum)
{
    if (subDirCnt > 0){
        LOG("setting initial frame to %d\n",frameNum);
        //if (frameNum == 0)
            //frameNum = 1;

        for (std::map<const char*,int>::iterator itr=partMap.begin(); itr != partMap.end(); itr++){
            utilities->masterThread->virtualTime = utilities->partDetails[(*itr).second].timestamp[utilities->partDetails[(*itr).second].currFrame];
            utilities->partDetails[(*itr).second].currFrame = frameNum;
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
        for (std::map<const char*,int>::iterator itr=partMap.begin(); itr != partMap.end(); itr++){
            if (strcmp (part,(*itr).first) == 0){
                *frame = utilities->partDetails[(*itr).second].currFrame;
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
bool MainWindow::cmdSafeExit(void)
{
    onMenuPlayBackStop();
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
    return true;
}
/**********************************************************/
bool MainWindow::safeExit(void)
{
    if(QMessageBox::question(this,"Quit","Do you want to quit?") == QMessageBox::Yes){
        onMenuPlayBackStop();
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
        return true;
    }
    return false;
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
        utilities = NULL;
    }

}
/**********************************************************/

bool MainWindow::getPartActivation(const char* szName)
{
    QTreeWidgetItem *row = NULL;
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
    cmdSafeExit();
    event->accept();
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
    partsName.clear();
    partsFullPath.clear();
    partsInfoPath.clear();
    partsLogPath.clear();


    itr = 0;
    partMap.clear();

    if(!initThread){
        initThread = new InitThread(utilities,newPath,&partsName,&partsFullPath,&partsInfoPath,&partsLogPath,this);
        connect(initThread,SIGNAL(initDone(int)),this,SLOT(onInitDone(int)),Qt::QueuedConnection);
        initThread->start();
    }else{
        if(!initThread->isRunning()){
            delete initThread;
            initThread = new InitThread(utilities,newPath,&partsName,&partsFullPath,&partsInfoPath,&partsLogPath,this);
            connect(initThread,SIGNAL(initDone(int)),this,SLOT(onInitDone(int)),Qt::QueuedConnection);
            initThread->start();
        }
    }

    loadingWidget.start();

    return true;
}

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
                std::cout << "OK clicked." << std::endl;
                break;
            }
            default:{
                std::cout << "Unexpected button clicked." << std::endl;
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
    QTreeWidgetItem *item = new QTreeWidgetItem();
    ui->mainWidget->addTopLevelItem(item);
    QCheckBox *checkBox = new QCheckBox();
    checkBox->setChecked(true);
    ui->mainWidget->setItemWidget(item,ACTIVE,checkBox);
    if(szName){
        item->setText(PART,QString("%1").arg(szName));
    }
    if(type){
        item->setText(TYPE,QString("%1").arg(type));
    }
    item->setText(FRAMES,QString("%1").arg(frames));

//    if(szFileName){
//        item->setText(6,QString("%1").arg(szFileName));
//    }
    if(portName){
        item->setText(PORT,QString("%1").arg(portName));
    }
    QProgressBar *progress = new QProgressBar();
    progress->setMaximum(100);
    progress->setValue(0);
    progress->setAlignment(Qt::AlignCenter);
    ui->mainWidget->setItemWidget(item,PERCENT,progress);

    itr++;
}
/**********************************************************/
bool MainWindow::setInitialPartProgress(const char* szName, int percentage)
{
    QTreeWidgetItem *row = NULL;
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
    QTreeWidgetItem *row = NULL;
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
    return NULL;

}















/**********************************************************/
void MainWindow::onMenuFileOpen()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Please choose a folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()){
        ui->mainWidget->clear();
        for (int x=0; x < subDirCnt; x++){
            utilities->closePorts(utilities->partDetails[x]);
        }


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
                std::cout << "OK clicked." << std::endl;
                break;
            }
            default:{
                std::cout << "Unexpected button clicked." << std::endl;
                break;
            }
        }
    }
}


/**********************************************************/
void MainWindow::onMenuHelpAbout()
{
    QString copyright = "2014 (C) Robotics, Brain and Cognitive Sciences\nIstituto Italiano di Tecnologia";
    QString name = APP_NAME;
    QString version = APP_VERSION;
    AboutDlg dlg(name,version,copyright,"http://www.icub.org/");
    dlg.exec();
}


/**********************************************************/
void MainWindow::onMenuPlayBackPlay()
{

    if (subDirCnt > 0){
        ui->playButton->setIcon(QIcon(":/images/pause.png"));
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

        if ( utilities->masterThread->isRunning() ) {
            LOG("asking the thread to resume\n");

            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].worker->resetTime();

            utilities->masterThread->resume();
        } else {
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
        ui->playButton->setIcon(QIcon(":/images/play.png"));
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
    ui->playButton->setIcon(QIcon(":/images/play.png"));
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
        ui->playButton->setIcon(QIcon(":/images/play.png"));
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
        }
        setPlayProgress(0);
    }
}
/**********************************************************/
void MainWindow::onMenuPlayBackForward()
{
    if (subDirCnt > 0){
        utilities->masterThread->forward(5);
    }
}
/**********************************************************/
void MainWindow::onMenuPlayBackBackward()
{
    if (subDirCnt > 0){
        utilities->masterThread->backward(5);
    }
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
//void MainWindow::onPlayButtonTogle()
//{
//    if(m_playButton.get_stock_id() == Glib::ustring(Gtk::Stock::MEDIA_PLAY.id))
//        onMenuPlayBackPlay();
//    else
//        onMenuPlayBackPause();
//}
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
//void MainWindow::onPlayPositionSlided(Gtk::ScrollType scroll)
//{
//    LOG( "slided ...\n");
//}
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
    QTreeWidgetItem *row = NULL;
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
    QTreeWidgetItem *row = NULL;
    row = getRowByPart(QString("%1").arg(szName));
    if(row){
        row->setText(SAMPLERATE,QString("%1 ms").arg(frameRate));
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

void MainWindow::goToPercentage(int value)
{
    utilities->masterThread->goToPercentage(value);


}


void MainWindow::onClose()
{
    if(safeExit()){
        QMainWindow::close();
    }

}




/*************************************************/

InitThread::InitThread(Utilities *utilities,
                       QString newPath,
                       std::vector<std::string>    *partsName,
                       std::vector<std::string>    *partsFullPath,
                       std::vector<std::string>    *partsInfoPath,
                       std::vector<std::string>    *partsLogPath,
                       QObject *parent) : QThread(parent)
{
    this->utilities = utilities;
    this->newPath = newPath;
    this->partsName = partsName;
    this->partsFullPath = partsFullPath;
    this->partsInfoPath = partsInfoPath;
    this->partsLogPath = partsLogPath;
    this->mainWindow = (QMainWindow*)parent;
}

void InitThread::run()
{
    utilities->resetMaxTimeStamp();
    int subDirCnt = utilities->getRecSubDirList(newPath.toLatin1().data(), *partsName, *partsInfoPath, *partsLogPath, *partsFullPath, 1);
    LOG("the size of subDirs is: %d\n", subDirCnt);
    //reset totalSent to 0
    utilities->totalSent = 0;
    utilities->totalThreads = subDirCnt;

    if (subDirCnt > 0){
        utilities->partDetails = new partsData [subDirCnt];//resize(subDirCnt);
    }

    //fill in parts with all data
    for (int x=0; x < subDirCnt; x++){
        utilities->partDetails[x].name = partsName->at(x);
        utilities->partDetails[x].infoFile = partsInfoPath->at(x);
        utilities->partDetails[x].logFile = partsLogPath->at(x);
        utilities->partDetails[x].path = partsFullPath->at(x);

        utilities->setupDataFromParts(utilities->partDetails[x]);

        utilities->partDetails[x].worker = new WorkerClass(x, subDirCnt);
        utilities->partDetails[x].worker->setManager(utilities);
    }

    //get the max timestamp of all the parts for synchronization
    if (subDirCnt > 0){
        utilities->getMaxTimeStamp();
    }

    //set initial frames for all parts depending on first timestamps
    for (int x=0; x < subDirCnt; x++){
        utilities->initialFrame.push_back( utilities->partDetails[x].currFrame) ;
    }

    utilities->masterThread = new MasterThread(utilities, subDirCnt, mainWindow);
    //connect(utilities->masterThread,SIGNAL(updateGuiRateThread()),this,SLOT(onUpdateGuiRateThread()),Qt::QueuedConnection);
    utilities->masterThread->stepfromCmd = false;

    initDone(subDirCnt);
}

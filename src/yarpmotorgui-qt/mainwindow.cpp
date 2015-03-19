/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "flowlayout.h"
#include "jointitem.h"
#include "partitem.h"

#include <QToolBar>
#include <QDebug>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFont>
#include <QFontMetrics>
#include <QMessageBox>
#include <QSettings>

#define TREEMODE_OK     1
#define TREEMODE_WARN   2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLocale::setDefault(QLocale::C);
    tabPanel = NULL;
    sequenceActiveCount = 0;

    setWindowTitle("Qt Robot Motor GUI V1.0");
    setMinimumWidth(MAX_WIDTH_JOINT + 60);



    QString globalLabel("Global Joints Commands ");
    globalToolBar = new QToolBar("Global Joints Commands",this);
    QLabel *label1 = new QLabel(globalLabel);

    QFont f = label1->font();
    f.setBold(true);
    label1->setFont(f);
    globalToolBar->addWidget(label1)->setCheckable(false);



    globalToolBar->addSeparator();
    goAll = globalToolBar->addAction(QIcon(":/play-all.svg"),"Go All");
    globalToolBar->addSeparator();
    runAllSeq      = globalToolBar->addAction(QIcon(":/images/runSequence.png"),"Run All Sequences (use joint speeds from Speed tab)");
    runAllSeqTime  = globalToolBar->addAction(QIcon(":/images/runSequenceTime.png"),"Run All Sequences (ignore Speed tab, produce coordinated movement using Timing)");
    saveAllSeq     = globalToolBar->addAction(QIcon(":/file-save.svg"),"Save All Sequences");
    loadAllSeq     = globalToolBar->addAction(QIcon(":/file-open.svg"),"Load All Sequences");
    cycleAllSeq    = globalToolBar->addAction(QIcon(":/images/cycleAllSequence.png"),"Cycle All Sequences (use joint speeds from Speed tab)");
    cycleAllSeqTime= globalToolBar->addAction(QIcon(":/images/cycleAllSequenceTime.png"),"Cycle All Sequences (ignore Speed tab, produce coordinated movement using Timing)");
    stopAllSeq     = globalToolBar->addAction(QIcon(":/stop.svg"),"Stop All Sequences");
    globalToolBar->addSeparator();
    runAllParts    = globalToolBar->addAction(QIcon(":/play.svg"),"Run All Parts");
    homeAllParts   = globalToolBar->addAction(QIcon(":/home.svg"),"Home All Parts");
    addToolBar(globalToolBar);

    QMenu *globalMenuCommands = ui->menuBar->addMenu("Global Joints Commands ");
    globalMenuCommands->addAction(goAll);
    globalMenuCommands->addSeparator();
    globalMenuCommands->addAction(runAllSeq);
    globalMenuCommands->addAction(runAllSeqTime);
    globalMenuCommands->addAction(saveAllSeq);
    globalMenuCommands->addAction(loadAllSeq);
    globalMenuCommands->addAction(cycleAllSeq);
    globalMenuCommands->addAction(cycleAllSeqTime);
    globalMenuCommands->addAction(stopAllSeq);
    globalMenuCommands->addSeparator();
    globalMenuCommands->addAction(runAllParts);
    globalMenuCommands->addAction(homeAllParts);

    connect(goAll,SIGNAL(triggered()),this,SLOT(onGoAll()));
    connect(runAllParts,SIGNAL(triggered()),this,SLOT(onRunAllParts()));
    connect(homeAllParts,SIGNAL(triggered()),this,SLOT(onHomeAllParts()));

    connect(runAllSeq,SIGNAL(triggered()),this,SLOT(onRunAllSeq()));
    connect(runAllSeqTime,SIGNAL(triggered()),this,SLOT(onRunTimeAllSeq()));
    connect(cycleAllSeq,SIGNAL(triggered()),this,SLOT(onCycleAllSeq()));
    connect(cycleAllSeqTime,SIGNAL(triggered()),this,SLOT(onCycleTimeAllSeq()));

    connect(stopAllSeq,SIGNAL(triggered()),this,SLOT(onStopAllSeq()));
    connect(loadAllSeq,SIGNAL(triggered()),this,SLOT(onLoadAllSeq()));
    connect(saveAllSeq,SIGNAL(triggered()),this,SLOT(onSaveAllSeq()));


    //addToolBarBreak();

    partToolBar = new QToolBar("Current Part",this);
    partToolBar->setMovable(true);
    partToolBar->setFloatable(true);
    partToolBar->setAllowedAreas(Qt::AllToolBarAreas);

    QFontMetrics metrics(f);
    // int w = metrics.width(globalLabel);
    metrics.width(globalLabel);

    partName = new QLabel("NONE");
    f = partName->font();
    f.setBold(true);
    partName->setFont(f);
    //partName->setMinimumWidth(w);
    partToolBar->addWidget(partName)->setCheckable(false);

    partToolBar->addSeparator();
    QAction *openSequenceAction     = partToolBar->addAction(QIcon(":/file-new.svg"),"Open Sequence Tab");
    partToolBar->addSeparator();
    QAction *runAll                 = partToolBar->addAction(QIcon(":/play.svg"),"Run All");
    QAction *calibAll               = partToolBar->addAction(QIcon(":/images/calibrate.png"),"Calibrate All");
    QAction *homeAll                = partToolBar->addAction(QIcon(":/home.svg"),"Home All");
    partToolBar->addSeparator();
    QAction *idleAll                = partToolBar->addAction(QIcon(":/idle.svg"),"Idle All");
    addToolBar(partToolBar);

    currentPartMenu = ui->menuBar->addMenu("Current Part: ");
    currentPartMenu->addAction(openSequenceAction);
    currentPartMenu->addSeparator();
    currentPartMenu->addAction(runAll);
    currentPartMenu->addAction(calibAll);
    currentPartMenu->addAction(homeAll);
    currentPartMenu->addSeparator();
    currentPartMenu->addAction(idleAll);

    connect(openSequenceAction,SIGNAL(triggered()),this,SLOT(onOpenSequenceTab()));
    connect(runAll,SIGNAL(triggered()),this,SLOT(onRunAll()));
    connect(idleAll,SIGNAL(triggered()),this,SLOT(onIdleAll()));
    connect(homeAll,SIGNAL(triggered()),this,SLOT(onHomeAll()));
    connect(calibAll,SIGNAL(triggered()),this,SLOT(onCalibAll()));




    QMenu *windows = ui->menuBar->addMenu("View");
    QAction *viewGlobalToolbar = windows->addAction("Global Commands Toolbar");
    QAction *viewPartToolbar = windows->addAction("Part Commands Toolbar");
    QAction *viewSpeedValues = windows->addAction("View Speed Values");

    viewGlobalToolbar->setCheckable(true);
    viewPartToolbar->setCheckable(true);
    viewSpeedValues->setCheckable(true);


    QSettings settings("YARP","yarpmotorgui");
    bool bViewGlobalToolbar = settings.value("GlobalToolVisible",true).toBool();
    bool bViewPartToolbar = settings.value("PartToolVisible",true).toBool();
    bool bSpeedValues = settings.value("SpeedValuesVisible",false).toBool();

    viewGlobalToolbar->setChecked(bViewGlobalToolbar);
    viewPartToolbar->setChecked(bViewPartToolbar);
    viewSpeedValues->setChecked(bSpeedValues);

    globalToolBar->setVisible(bViewGlobalToolbar);
    partToolBar->setVisible(bViewPartToolbar);

    connect(viewGlobalToolbar,SIGNAL(triggered(bool)),this,SLOT(onViewGlobalToolbar(bool)));
    connect(viewPartToolbar,SIGNAL(triggered(bool)),this,SLOT(onViewPartToolbar(bool)));
    connect(viewSpeedValues,SIGNAL(triggered(bool)),this,SLOT(onViewSpeeds(bool)));



    timer.setInterval(200);
    timer.setSingleShot(false);
    connect(&timer,SIGNAL(timeout()),this,SLOT(onUpdate()),Qt::QueuedConnection);
    timer.start();
}

MainWindow::~MainWindow()
{
    mutex.lock();

    disconnect(&timer,SIGNAL(timeout()),this,SLOT(onUpdate()));
    timer.stop();

    onStopAllSeq();

    if(tabPanel){
        disconnect(tabPanel,SIGNAL(currentChanged(int)),this,SLOT(onCurrentPartChanged(int)));
        for(int i=0;i<tabPanel->count();i++){
            delete tabPanel->widget(i);
        }
        delete tabPanel;
        tabPanel = NULL;
    }

    delete ui;

    mutex.unlock();
}

void MainWindow::onSequenceActivated()
{
    sequenceActiveCount++;
    goAll->setEnabled(false);
    runAllSeq->setEnabled(false);
    runAllSeqTime->setEnabled(false);
    saveAllSeq->setEnabled(false);
    loadAllSeq->setEnabled(false);
    cycleAllSeq->setEnabled(false);
    cycleAllSeqTime->setEnabled(false);
    runAllParts->setEnabled(false);
    homeAllParts->setEnabled(false);

}

void MainWindow::onSequenceStopped()
{
    sequenceActiveCount--;
    if(sequenceActiveCount <= 0){
        sequenceActiveCount = 0;

        goAll->setEnabled(true);
        runAllSeq->setEnabled(true);
        runAllSeqTime->setEnabled(true);
        saveAllSeq->setEnabled(true);
        loadAllSeq->setEnabled(true);
        cycleAllSeq->setEnabled(true);
        cycleAllSeqTime->setEnabled(true);
        runAllParts->setEnabled(true);
        homeAllParts->setEnabled(true);

    }
}


void MainWindow::onViewGlobalToolbar(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("GlobalToolVisible",val);
    if(!val){
        globalToolBar->hide();
    }else{
        globalToolBar->show();
    }

}

void MainWindow::onViewPartToolbar(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("PartToolVisible",val);
    if(!val){
        partToolBar->hide();
    }else{
        partToolBar->show();
    }
}

void MainWindow::onViewSpeeds(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("SpeedValuesVisible",val);

    viewSpeedValues(val);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    mutex.lock();

    this->setVisible(false);

    disconnect(&timer,SIGNAL(timeout()),this,SLOT(onUpdate()));
    timer.stop();

    onStopAllSeq();

    if(tabPanel){
        disconnect(tabPanel,SIGNAL(currentChanged(int)),this,SLOT(onCurrentPartChanged(int)));
        for(int i=0;i<tabPanel->count();i++){
            delete tabPanel->widget(i);
        }
        delete tabPanel;
        tabPanel = NULL;
    }

    mutex.unlock();

    QMainWindow::closeEvent(event);

}




bool MainWindow::init(QString robotName, QStringList enabledParts,
                      ResourceFinder *finder,
                      bool debug_param_enabled,
                      bool speedview_param_enabled,
                      bool enable_calib_all,
                      bool position_direct_enabled,
                      bool openloop_enabled)
{

    int count = enabledParts.count();

    tabPanel = new QTabWidget(ui->mainContainer);



    int errorCount = 0;
    QScrollArea *scroll = NULL;
    PartItem *part = NULL;
    for(int i=0;i<count;i++){
        //JointItem *item = new JointItem();
        //layout->addWidget(item);
        scroll = new QScrollArea(tabPanel);
        scroll->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scroll->setWidgetResizable(true);
        part = new PartItem(robotName,
                                      enabledParts.at(i),
                                      finder,
                                      debug_param_enabled,
                                      speedview_param_enabled,
                                      enable_calib_all,
                                      position_direct_enabled,
                                      openloop_enabled,
                                      scroll);

        if(!part->getInterfaceError()){
            connect(part,SIGNAL(sequenceActivated()),this,SLOT(onSequenceActivated()));
            connect(part,SIGNAL(sequenceStopped()),this,SLOT(onSequenceStopped()));
            connect(this,SIGNAL(viewSpeedValues(bool)),part,SLOT(onViewSpeedValues(bool)));

            scroll->setWidget(part);
            tabPanel->addTab(scroll,enabledParts.at(i));
            if(i==0){

                QString auxName = enabledParts.at(i);
                auxName.replace(0,1,enabledParts.at(i).at(0).toUpper());
                currentPartMenu->setTitle(QString("%1 Commands ").arg(auxName));

                this->partName->setText(QString("%1 Commands ").arg(auxName));
            }

            QTreeWidgetItem *mode = new QTreeWidgetItem();
            mode->setText(0,enabledParts.at(i));
            ui->treeWidgetMode->addTopLevelItem(mode);
            mode->setExpanded(false);
            part->setTreeWidgetModeNode(mode);

        }else{
            if(part){
                delete part;
            }
            if(scroll){
                delete scroll;
            }
            errorCount++;
        }
    }

    if(errorCount == count){
        return false;
    }
    QHBoxLayout *lay = new QHBoxLayout();
    lay->setMargin(0);
    lay->setSpacing(0);
    ui->mainContainer->setLayout(lay);
    ui->mainContainer->layout()->addWidget(tabPanel);
    connect(tabPanel,SIGNAL(currentChanged(int)),this,SLOT(onCurrentPartChanged(int)));

    QSettings settings("YARP","yarpmotorgui");
    bool speedVisible = settings.value("SpeedValuesVisible",false).toBool();

    onViewSpeeds(speedVisible);
    return true;
}


void MainWindow::onCurrentPartChanged(int index)
{
    if(index < 0){
        return;
    }
    QString partName = tabPanel->tabText(index);

    QString auxName = partName;
    auxName.replace(0,1,partName.at(0).toUpper());
    currentPartMenu->setTitle(QString("%1 Commands").arg(auxName));
    this->partName->setText(QString("%1 Commands").arg(auxName));

    QScrollArea *scroll = (QScrollArea *)tabPanel->widget(index);
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->resizeWidget(part->width());

}

void MainWindow::onCalibAll()
{
    if(!tabPanel){
        return;
    }

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->calibrateAll();
}

void MainWindow::onHomeAllParts()
{
    if(!tabPanel){
        return;
    }

    QString parts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndHomeAll();
        if(!done){
            parts.append(QString("- %1_zero\n").arg(part->getPartName()));
        }
    }

    if(!parts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("No zero group found in the supplied files. Define suitable\n%1").arg(parts));
    }
}

void MainWindow::onHomeAll()
{
    if(!tabPanel){
        return;
    }

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->homeAll();
}

void MainWindow::onIdleAll()
{
    if(!tabPanel){
        return;
    }

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->idleAll();
}

void MainWindow::onCycleTimeAllSeq()
{
    if(!tabPanel){
        return;
    }

    QString notSelectedParts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndCycleTimeAllSeq();
        if(!done){
            notSelectedParts.append(QString("- %1\n").arg(part->getPartName()));
        }
    }
    if(!notSelectedParts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("Load a valid sequence before performing this action for these parts:\n%1").arg(notSelectedParts));
    }
}

void MainWindow::onCycleAllSeq()
{
    if(!tabPanel){
        return;
    }

    QString notSelectedParts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndCycleAllSeq();
        if(!done){
            notSelectedParts.append(QString("- %1\n").arg(part->getPartName()));
        }
    }
    if(!notSelectedParts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("Load a valid sequence before performing this action for these parts:\n%1").arg(notSelectedParts));
    }
}

void MainWindow::onRunAllSeq()
{
    if(!tabPanel){
        return;
    }

    QString notSelectedParts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndRunAllSeq();
        if(!done){
            notSelectedParts.append(QString("- %1\n").arg(part->getPartName()));
        }
    }
    if(!notSelectedParts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("Load a valid sequence before performing this action for these parts:\n%1").arg(notSelectedParts));
    }
}


void MainWindow::onRunTimeAllSeq()
{
    if(!tabPanel){
        return;
    }

    QString notSelectedParts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndRunTimeAllSeq();
        if(!done){
            notSelectedParts.append(QString("- %1\n").arg(part->getPartName()));
        }
    }
    if(!notSelectedParts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("Load a valid sequence before performing this action for these parts:\n%1").arg(notSelectedParts));
    }
}

void MainWindow::onLoadAllSeq()
{
    if(!tabPanel){
        return;
    }


    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->loadSequence();

    }
}

void MainWindow::onSaveAllSeq()
{
    if(!tabPanel){
        return;
    }


    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->saveSequence();

    }
}

void MainWindow::onStopAllSeq()
{
    if(!tabPanel){
        return;
    }


    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->stopSequence();

    }

 }

void MainWindow::onGoAll()
{
    if(!tabPanel){
        return;
    }

    QString notSelectedParts;

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        bool done = part->checkAndGo();
        if(!done){
            notSelectedParts.append(QString("- %1\n").arg(part->getPartName()));
        }
    }

    if(!notSelectedParts.isEmpty()){
        QMessageBox::critical(this,"Error", QString("Select a valid entry in the table before performing a movement for these parts:\n%1").arg(notSelectedParts));
    }

}




void MainWindow::onRunAllParts()
{
    if(!tabPanel){
        return;
    }

    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *scroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->runAll();
    }
}

void MainWindow::onRunAll()
{
    if(!tabPanel){
        return;
    }

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->runAll();
}

void MainWindow::onOpenSequenceTab()
{
    if(!tabPanel){
        return;
    }

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    PartItem *part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }
    part->openSequenceWindow();
}



void MainWindow::onUpdate()
{
    if(!tabPanel){
        return;
    }
    mutex.lock();

    QScrollArea *scroll = (QScrollArea *)tabPanel->currentWidget();
    if(!scroll){
        mutex.unlock();
        return;
    }
    PartItem *currentPart = (PartItem*)scroll->widget();
    if(!currentPart){
        mutex.unlock();
        return;
    }
    for(int i=0; i<tabPanel->count();i++){
        QScrollArea *tabScroll = (QScrollArea *)tabPanel->widget(i);
        PartItem *item = (PartItem*)tabScroll->widget();
        item->updateControlMode();
        updateModesTree(item);
        if(item == currentPart){
            item->updatePart();
        }
    }
    mutex.unlock();
}

QColor MainWindow::getColorMode(int m)
{
    QColor mode;
    switch (m) {
    case JointItem::Idle:{
        mode = idleColor;
        break;
    }
    case JointItem::Position:{
        mode = positionColor;
        break;
    }
    case JointItem::PositionDirect:{
        mode = positionDirectColor;
        break;
    }
    case JointItem::Mixed:{
        mode = mixedColor;
        break;
    }
    case JointItem::Velocity:{
        mode = velocityColor;
        break;
    }
    case JointItem::Torque:{
        mode = torqueColor;
        break;
    }
    case JointItem::OpenLoop:{
        mode = openLoopColor;
        break;
    }

    case JointItem::Disconnected:{
        mode = disconnectColor;
        break;
    }
    case JointItem::HwFault:{
        mode = hwFaultColor;
        break;
    }
    case JointItem::Calibrating:{
        mode = calibratingColor;
        break;
    }
    case JointItem::NotConfigured:{
        mode = calibratingColor;
        break;
    }
    case JointItem::Configured:{
        mode = calibratingColor;
        break;
    }

    default:
        mode = calibratingColor;
        break;
    }

    return mode;
}

QString MainWindow::getStringMode(int m)
{
    QString mode;
    switch (m) {
    case JointItem::Idle:{
        mode = "Idle";
        break;
    }
    case JointItem::Position:{
        mode = "Position";
        break;
    }
    case JointItem::PositionDirect:{
        mode = "Position Direct";
        break;
    }
    case JointItem::Mixed:{
        mode = "Mixed";
        break;
    }
    case JointItem::Velocity:{
        mode = "Velocity";
        break;
    }
    case JointItem::Torque:{
        mode = "Torque";
        break;
    }
    case JointItem::OpenLoop:{
        mode = "Open Loop";
        break;
    }

    case JointItem::Disconnected:{
        mode = "Disconnected";
        break;
    }
    case JointItem::HwFault:{
        mode = "Hardware Fault";
        break;
    }
    case JointItem::Calibrating:{
        mode = "Calibrating";
        break;
    }
    case JointItem::NotConfigured:{
        mode = "Not Configured";
        break;
    }
    case JointItem::Configured:{
        mode = "Configured";
        break;
    }

    default:
        mode = "Unknown";
        break;
    }

    return mode;

}

void MainWindow::updateModesTree(PartItem *part)
{

    QTreeWidgetItem *parentNode = part->getTreeWidgetModeNode();

    QList <int> modes = part->getPartMode();

    if(modes.count() > 0 && parentNode->childCount() <= 0){
        for(int i=0; i<modes.count(); i++){
            QString mode;
            mode = getStringMode(modes.at(i));
            QTreeWidgetItem *jointNode = new QTreeWidgetItem(parentNode);
            jointNode->setText(0,QString("Joint %1").arg(i));
            jointNode->setText(1,mode);
            QColor c = getColorMode(modes.at(i));
            jointNode->setBackgroundColor(0,c);
            jointNode->setBackgroundColor(1,c);


            if(c == hwFaultColor){
                parentNode->setData(0,Qt::UserRole,TREEMODE_WARN);
                jointNode->setData(0,Qt::UserRole,TREEMODE_WARN);
                parentNode->setIcon(0,QIcon(":/warning.svg"));
                jointNode->setIcon(0,QIcon(":/warning.svg"));
            }else{
                parentNode->setData(0,Qt::UserRole,TREEMODE_OK);
                jointNode->setData(0,Qt::UserRole,TREEMODE_OK);
                parentNode->setIcon(0,QIcon(":/apply.svg"));
            }
        }
    }else{
        for(int i=0;i<parentNode->childCount();i++){
            QTreeWidgetItem *item = parentNode->child(i);
            QString mode;
            QColor c = getColorMode(modes.at(i));
            mode = getStringMode(modes.at(i));

            if(c == hwFaultColor){
                item->setData(0,Qt::UserRole,TREEMODE_WARN);
            }else{
                item->setData(0,Qt::UserRole,TREEMODE_OK);
            }

            if(!parentNode->isExpanded()){
                if(item->data(0,Qt::UserRole).toInt() == TREEMODE_WARN){
                    if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                        parentNode->setBackgroundColor(0,hwFaultColor);
                        parentNode->setIcon(0,QIcon(":/warning.svg"));
                        parentNode->setData(0,Qt::UserRole,TREEMODE_WARN);
                    }
                }else{
                    if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_OK){
                        parentNode->setBackgroundColor(0,QColor("white"));
                        parentNode->setIcon(0,QIcon(":/apply.svg"));
                        parentNode->setData(0,Qt::UserRole,TREEMODE_OK);
                    }
                }
            }else{
                if(item->text(1) != mode){
                    item->setText(1,mode);
                }
                if(item->backgroundColor(0) != c){
                    item->setBackgroundColor(0,c);
                    item->setBackgroundColor(1,c);
                }

                if(parentNode->backgroundColor(0) != QColor("white")){
                    parentNode->setBackgroundColor(0,QColor("white"));
                }


                if(c == hwFaultColor){
                    if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                        parentNode->setIcon(0,QIcon(":/warning.svg"));
                        parentNode->setData(0,Qt::UserRole,TREEMODE_WARN);
                    }
                    if(item->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                        item->setIcon(0,QIcon(":/warning.svg"));
                        item->setData(0,Qt::UserRole,TREEMODE_WARN);
                    }
                }else{
                    if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_OK){
                        parentNode->setIcon(0,QIcon(":/apply.svg"));
                        parentNode->setData(0,Qt::UserRole,TREEMODE_OK);
                    }
                }


            }

        }
    }

}



ModesTreeWidget::ModesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
#include <QFileDialog>
#include <QShortcut>
#include <map>
#include <cstdlib>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#define TREEMODE_OK     1
#define TREEMODE_WARN   2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    QLocale::setDefault(QLocale::C);
    m_tabPanel = nullptr;
    m_sequenceActiveCount = 0;

    setWindowTitle("Qt Robot Motor GUI V2.0");
    setMinimumWidth(MAX_WIDTH_JOINT + 60);

    m_sliderOpt = nullptr;

    QString globalLabel("Global Joints Commands ");
    m_globalToolBar = new QToolBar("Global Joints Commands", this);
    QLabel *label1 = new QLabel(globalLabel);

    QFont f = label1->font();
    f.setBold(true);
    label1->setFont(f);
    m_globalToolBar->addWidget(label1)->setCheckable(false);


    m_globalToolBar->addSeparator();
    m_goAll = m_globalToolBar->addAction(QIcon(":/play-all.svg"), "Go All");
    m_globalToolBar->addSeparator();
    m_runAllSeq = m_globalToolBar->addAction(QIcon(":/images/runSequence.png"), "Run All Sequences (use joint speeds from Speed tab)");
    m_runAllSeqTime = m_globalToolBar->addAction(QIcon(":/images/runSequenceTime.png"), "Run All Sequences (ignore Speed tab, produce coordinated movement using Timing)");
    m_saveAllSeq = m_globalToolBar->addAction(QIcon(":/file-save.svg"), "Save All Sequences");
    m_loadAllSeq = m_globalToolBar->addAction(QIcon(":/file-open.svg"), "Load All Sequences");
    m_cycleAllSeq = m_globalToolBar->addAction(QIcon(":/images/cycleAllSequence.png"), "Cycle All Sequences (use joint speeds from Speed tab)");
    m_cycleAllSeqTime = m_globalToolBar->addAction(QIcon(":/images/cycleAllSequenceTime.png"), "Cycle All Sequences (ignore Speed tab, produce coordinated movement using Timing)");
    m_stopAllSeq = m_globalToolBar->addAction(QIcon(":/stop.svg"), "Stop All Sequences");
    m_globalToolBar->addSeparator();
    m_idleAllParts = m_globalToolBar->addAction(QIcon(":/idle.svg"), "Idle All Parts");
    m_runAllParts = m_globalToolBar->addAction(QIcon(":/play.svg"), "Run All Parts");
    m_homeAllParts = m_globalToolBar->addAction(QIcon(":/home.svg"), "Home All Parts");
    m_globalToolBar->addSeparator();
    m_script1 = m_globalToolBar->addAction(QIcon(":/action1.svg"), "Execute User Script1");
    m_script2 = m_globalToolBar->addAction(QIcon(":/action2.svg"), "Execute User Script2");
    addToolBar(m_globalToolBar);

    QMenu *globalMenuCommands = m_ui->menuBar->addMenu("Global Joints Commands ");
    globalMenuCommands->addAction(m_goAll);
    globalMenuCommands->addSeparator();
    globalMenuCommands->addAction(m_runAllSeq);
    globalMenuCommands->addAction(m_runAllSeqTime);
    globalMenuCommands->addAction(m_saveAllSeq);
    globalMenuCommands->addAction(m_loadAllSeq);
    globalMenuCommands->addAction(m_cycleAllSeq);
    globalMenuCommands->addAction(m_cycleAllSeqTime);
    globalMenuCommands->addAction(m_stopAllSeq);
    globalMenuCommands->addSeparator();
    globalMenuCommands->addAction(m_idleAllParts);
    globalMenuCommands->addAction(m_runAllParts);
    globalMenuCommands->addAction(m_homeAllParts);
    //Looking for custom positions in the config file
    yarp::os::ResourceFinder &finder = yarp::os::ResourceFinder::getResourceFinderSingleton();
    //Positions have the following form: "customPosition_{NAME_OF_CUSTOM_POSITION}"
    //To iterate on all groups, transform the finder into Bottle
    yarp::os::Bottle ini(finder.toString());

    std::map<std::string, yarp::os::Bottle> customPositions;

    for (size_t index = 0; index < ini.size(); ++index) {
        //Look for groups starting with "customPosition_"
        yarp::os::Value item = ini.get(index);
        if (!item.isList()) {
            continue;
        }
        yarp::os::Bottle *subElement = item.asList();
        //At least two elements and first should be string
        if (!subElement
            || subElement->size() < 2
            || !subElement->get(0).isString()) {
            continue;
        }
        //get first element
        std::string key = subElement->get(0).asString();
        std::string pattern = "customPosition_";
        size_t subStringPosition = key.find(pattern);
        if (subStringPosition != 0) {
            continue; //not starting or not found
        }

        std::string customPositionName = key.substr(pattern.size());
        customPositions.insert(std::map<std::string, yarp::os::Bottle>::value_type(customPositionName, subElement->tail()));
    }

    m_customPositionsAllParts.reserve(customPositions.size());
    if (customPositions.size() > 0) {
        //If there are customPositions create a submenu item
        QMenu *customPositionsMenu = globalMenuCommands->addMenu(QIcon(":/home.svg"), "Custom positions");

        unsigned keyIndex = 0;
        for (std::map<std::string, yarp::os::Bottle>::const_iterator it(customPositions.begin()); it != customPositions.end(); ++it) {

            QAction *newAction = customPositionsMenu->addAction(("Move all parts to " + it->first).c_str());
            m_customPositionsAllParts.push_back(newAction);
            //
            const yarp::os::Bottle &position = it->second;

            // Adding shortcut (to only the first 9 sequences)
            if (keyIndex < 9) {
                QKeySequence shortcut(Qt::CTRL + Qt::META + (Qt::Key_1 + keyIndex++));
                newAction->setShortcut(shortcut);
                newAction->setShortcutContext(Qt::ApplicationShortcut);
            }

            //copy position in the lambda
            connect(newAction, &QAction::triggered, this, [this, position]{onHomeAllPartsToCustomPosition(position); });
        }
    }

    globalMenuCommands->addSeparator();
    globalMenuCommands->addAction(m_script1);
    globalMenuCommands->addAction(m_script2);

    connect(m_goAll, SIGNAL(triggered()), this, SLOT(onGoAll()));
    connect(m_idleAllParts, SIGNAL(triggered()), this, SLOT(onIdleAllParts()));
    connect(m_runAllParts, SIGNAL(triggered()), this, SLOT(onRunAllParts()));
    connect(m_homeAllParts, SIGNAL(triggered()), this, SLOT(onHomeAllParts()));

    connect(m_runAllSeq, SIGNAL(triggered()), this, SLOT(onRunAllSeq()));
    connect(m_runAllSeqTime, SIGNAL(triggered()), this, SLOT(onRunTimeAllSeq()));
    connect(m_cycleAllSeq, SIGNAL(triggered()), this, SLOT(onCycleAllSeq()));
    connect(m_cycleAllSeqTime, SIGNAL(triggered()), this, SLOT(onCycleTimeAllSeq()));

    connect(m_stopAllSeq, SIGNAL(triggered()), this, SLOT(onStopAllSeq()));
    connect(m_loadAllSeq, SIGNAL(triggered()), this, SLOT(onLoadAllSeq()));
    connect(m_saveAllSeq, SIGNAL(triggered()), this, SLOT(onSaveAllSeq()));

    connect(m_script1, SIGNAL(triggered()), this, SLOT(onExecuteScript1()));
    connect(m_script2, SIGNAL(triggered()), this, SLOT(onExecuteScript2()));

    //addToolBarBreak();

    m_partToolBar = new QToolBar("Current Part", this);
    m_partToolBar->setMovable(true);
    m_partToolBar->setFloatable(true);
    m_partToolBar->setAllowedAreas(Qt::AllToolBarAreas);

    m_partName = new QLabel("NONE");
    f = m_partName->font();
    f.setBold(true);
    m_partName->setFont(f);

    m_partToolBar->addWidget(m_partName)->setCheckable(false);

    m_partToolBar->addSeparator();
    openSequenceAction = m_partToolBar->addAction(QIcon(":/file-new.svg"), "Open Sequence Tab");
    m_partToolBar->addSeparator();
    m_runSinglePart = m_partToolBar->addAction(QIcon(":/play.svg"), "Run all joints of this part");
    m_idleSinglePart = m_partToolBar->addAction(QIcon(":/idle.svg"), "Idle all joints of this part");
    m_calibSinglePart = m_partToolBar->addAction(QIcon(":/images/calibrate.png"), "Calibrate all joints of this part");
    m_homeSinglePart = m_partToolBar->addAction(QIcon(":/home.svg"), "Home all joints of this part");

    m_customPositionsSinglePartToolbar.reserve(customPositions.size());
    if (customPositions.size() > 0) {
        for (std::map<std::string, yarp::os::Bottle>::const_iterator it(customPositions.begin()); it != customPositions.end(); ++it) {
            QAction *newAction = m_partToolBar->addAction(QIcon(":/home.svg"), ("Move all joints of this part to "  + it->first).c_str());
            m_customPositionsSinglePartToolbar.push_back(newAction);
            //
            const yarp::os::Bottle &position = it->second;

            //copy position in the lambda
            connect(newAction, &QAction::triggered, this, [this, position]{onHomeSinglePartToCustomPosition(position); });
        }
    }

    addToolBar(m_partToolBar);

    m_currentPartMenu = m_ui->menuBar->addMenu("Current Part: ");
    m_currentPartMenu->addAction(openSequenceAction);
    m_currentPartMenu->addSeparator();
    m_currentPartMenu->addAction(m_runSinglePart);
    m_currentPartMenu->addAction(m_calibSinglePart);
    m_currentPartMenu->addAction(m_homeSinglePart);
    m_currentPartMenu->addAction(m_idleSinglePart);

    m_customPositionsSinglePart.reserve(customPositions.size());
    if (customPositions.size() > 0) {

        //If there are customPositions create a submenu item
        QMenu *customPositionsMenu = m_currentPartMenu->addMenu(QIcon(":/home.svg"), "Custom positions");

        unsigned keyIndex = 0;
        for (std::map<std::string, yarp::os::Bottle>::const_iterator it(customPositions.begin()); it != customPositions.end(); ++it) {
            QAction *newAction = customPositionsMenu->addAction(("Move all joints of this part to " + it->first).c_str());
            m_customPositionsSinglePartToolbar.push_back(newAction);
            //
            const yarp::os::Bottle &position = it->second;

            // Adding shortcut (to only the first 9 sequences)
            if (keyIndex < 9) {
                QKeySequence shortcut(Qt::CTRL + Qt::ALT + Qt::META + (Qt::Key_1 + keyIndex++));
                newAction->setShortcut(shortcut);
                newAction->setShortcutContext(Qt::ApplicationShortcut);
            }
            //copy position in the lambda
            connect(newAction, &QAction::triggered, this, [this, position]{onHomeSinglePartToCustomPosition(position); });

        }
    }


    connect(openSequenceAction,SIGNAL(triggered()),this,SLOT(onOpenSequenceTab()));
    connect(m_runSinglePart, SIGNAL(triggered()), this, SLOT(onRunSinglePart()));
    connect(m_idleSinglePart, SIGNAL(triggered()), this, SLOT(onIdleSinglePart()));
    connect(m_homeSinglePart, SIGNAL(triggered()), this, SLOT(onHomeSinglePart()));
    connect(m_calibSinglePart, SIGNAL(triggered()), this, SLOT(onCalibSinglePart()));

    QMenu *windows = m_ui->menuBar->addMenu("View");
    QAction *viewGlobalToolbar = windows->addAction("Global Commands Toolbar");
    QAction *viewPartToolbar = windows->addAction("Part Commands Toolbar");
    QAction *viewSpeedValues = windows->addAction("View Speed Values");
    QAction *viewCurrentValues = windows->addAction("View Current Values");
    QAction *viewMotorPosition = windows->addAction("View Motor Position");
    QAction *viewDutyCycles = windows->addAction("View Duty Cycles");
    QAction *viewPositionTarget = windows->addAction("View Position Target");
    QAction *enableControlVelocity = windows->addAction("Enable Velocity Control");
    QAction *enableControlMixed = windows->addAction("Enable Mixed Control");
    QAction *enableControlPositionDirect = windows->addAction("Enable Position Direct Control");
    QAction *enableControlPWM = windows->addAction("Enable PWM Control");
    QAction *enableControlCurrent = windows->addAction("Enable Current Control");
    QAction *sliderOptions = windows->addAction("Slider Options...");

    viewGlobalToolbar->setCheckable(true);
    viewPartToolbar->setCheckable(true);
    viewSpeedValues->setCheckable(true);
    viewCurrentValues->setCheckable(true);
    viewMotorPosition->setCheckable(true);
    viewDutyCycles->setCheckable(true);
    enableControlVelocity->setCheckable(true);
    enableControlMixed->setCheckable(true);
    enableControlPositionDirect->setCheckable(true);
    enableControlPWM->setCheckable(true);
    enableControlCurrent->setCheckable(true);
    viewPositionTarget->setCheckable(true);

    QSettings settings("YARP","yarpmotorgui");
    bool bViewGlobalToolbar = settings.value("GlobalToolVisible",true).toBool();
    bool bViewPartToolbar = settings.value("PartToolVisible",true).toBool();
    bool bSpeedValues = settings.value("SpeedValuesVisible",false).toBool();
    bool bViewPositionTarget = settings.value("ViewPositionTarget", true).toBool();
    bool bviewMotorPosition = settings.value("MotorPositionVisible", false).toBool();
    bool bviewDutyCycles = settings.value("DutyCycleVisible", false).toBool();
    bool bCurrentValues = settings.value("CurrentsVisible", false).toBool();

    viewGlobalToolbar->setChecked(bViewGlobalToolbar);
    viewPartToolbar->setChecked(bViewPartToolbar);
    viewSpeedValues->setChecked(bSpeedValues);
    viewCurrentValues->setChecked(bCurrentValues);
    viewMotorPosition->setChecked(bviewMotorPosition);
    viewDutyCycles->setChecked(bviewDutyCycles);
    viewPositionTarget->setChecked(bViewPositionTarget);
    enableControlVelocity->setChecked(false);
    enableControlMixed->setChecked(false);
    enableControlPositionDirect->setChecked(false);
    enableControlPWM->setChecked(false);
    enableControlCurrent->setChecked(false);

    m_globalToolBar->setVisible(bViewGlobalToolbar);
    m_partToolBar->setVisible(bViewPartToolbar);

    connect(viewGlobalToolbar,SIGNAL(triggered(bool)),this,SLOT(onViewGlobalToolbar(bool)));
    connect(viewPartToolbar,SIGNAL(triggered(bool)),this,SLOT(onViewPartToolbar(bool)));
    connect(viewSpeedValues,SIGNAL(triggered(bool)),this,SLOT(onViewSpeeds(bool)));
    connect(viewCurrentValues, SIGNAL(triggered(bool)), this, SLOT(onViewCurrents(bool)));
    connect(viewMotorPosition, SIGNAL(triggered(bool)), this, SLOT(onViewMotorPositions(bool)));
    connect(viewDutyCycles, SIGNAL(triggered(bool)), this, SLOT(onViewDutyCycles(bool)));
    connect(viewPositionTarget, SIGNAL(triggered(bool)), this, SLOT(onViewPositionTarget(bool)));
    connect(enableControlVelocity, SIGNAL(triggered(bool)), this, SLOT(onEnableControlVelocity(bool)));
    connect(enableControlMixed, SIGNAL(triggered(bool)), this, SLOT(onEnableControlMixed(bool)));
    connect(enableControlPositionDirect, SIGNAL(triggered(bool)), this, SLOT(onEnableControlPositionDirect(bool)));
    connect(enableControlPWM, SIGNAL(triggered(bool)), this, SLOT(onEnableControlPWM(bool)));
    connect(enableControlCurrent, SIGNAL(triggered(bool)), this, SLOT(onEnableControlCurrent(bool)));
    connect(sliderOptions, SIGNAL(triggered()), this, SLOT(onSliderOptionsClicked()));

    connect(this,SIGNAL(internalClose()),this,SLOT(close()),Qt::QueuedConnection);


    m_timer.setInterval(200);
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onUpdate()), Qt::QueuedConnection);
    m_timer.start();
}

MainWindow::~MainWindow()
{
    m_mutex.lock();

    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_timer.stop();

    onStopAllSeq();

    if (m_tabPanel){
        disconnect(m_tabPanel, SIGNAL(currentChanged(int)), this, SLOT(onCurrentPartChanged(int)));
        for (int i = 0; i<m_tabPanel->count(); i++)
        {
            if (m_tabPanel->widget(i))
            {
                delete m_tabPanel->widget(i);
            }
        }
        delete m_tabPanel;
        m_tabPanel = nullptr;
    }

    delete m_ui;

    m_mutex.unlock();
}

void MainWindow::term()
{
    emit sig_internalClose();
    this->close();
}

void MainWindow::onSequenceActivated()
{
    m_sequenceActiveCount++;
    m_goAll->setEnabled(false);
    m_runAllSeq->setEnabled(false);
    m_runAllSeqTime->setEnabled(false);
    m_saveAllSeq->setEnabled(false);
    m_loadAllSeq->setEnabled(false);
    m_cycleAllSeq->setEnabled(false);
    m_cycleAllSeqTime->setEnabled(false);
    m_runAllParts->setEnabled(false);
    m_idleAllParts->setEnabled(false);
    m_homeAllParts->setEnabled(false);
}

void MainWindow::onSequenceStopped()
{
    m_sequenceActiveCount--;
    if (m_sequenceActiveCount <= 0)
    {
        m_sequenceActiveCount = 0;
        m_goAll->setEnabled(true);
        m_runAllSeq->setEnabled(true);
        m_runAllSeqTime->setEnabled(true);
        m_saveAllSeq->setEnabled(true);
        m_loadAllSeq->setEnabled(true);
        m_cycleAllSeq->setEnabled(true);
        m_cycleAllSeqTime->setEnabled(true);
        m_runAllParts->setEnabled(true);
        m_idleAllParts->setEnabled(true);
        m_homeAllParts->setEnabled(true);
    }
}


void MainWindow::onViewGlobalToolbar(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("GlobalToolVisible",val);
    if(!val){
        m_globalToolBar->hide();
    }else{
        m_globalToolBar->show();
    }

}

void MainWindow::onViewPartToolbar(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("PartToolVisible",val);
    if(!val){
        m_partToolBar->hide();
    }else{
        m_partToolBar->show();
    }
}

void MainWindow::onEnableControlVelocity(bool val)
{
    emit sig_enableControlVelocity(val);
}

void MainWindow::onEnableControlMixed(bool val)
{
    emit sig_enableControlMixed(val);
}

void MainWindow::onEnableControlPositionDirect(bool val)
{
    emit sig_enableControlPositionDirect(val);
}

void MainWindow::onEnableControlPWM(bool val)
{
    emit sig_enableControlPWM(val);
}

void MainWindow::onEnableControlCurrent(bool val)
{
    emit sig_enableControlCurrent(val);
}

void MainWindow::onSliderOptionsClicked()
{
    m_sliderOpt = new sliderOptions(this);

    m_sliderOpt->exec();

    delete m_sliderOpt;
    m_sliderOpt = nullptr;
}

void MainWindow::onViewSpeeds(bool val)
{
    QSettings settings("YARP","yarpmotorgui");
    settings.setValue("SpeedValuesVisible",val);

    emit sig_viewSpeedValues(val);
}

void MainWindow::onViewCurrents(bool val)
{
    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("CurrentValuesVisible", val);

    emit sig_viewCurrentValues(val);
}

void MainWindow::onViewMotorPositions(bool val)
{
    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("MotorPositionVisible", val);

    emit sig_viewMotorPositions(val);
}

void MainWindow::onViewDutyCycles(bool val)
{
    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("DutyCyclesVisible", val);

    emit sig_viewDutyCycles(val);
}

void MainWindow::onViewPositionTarget(bool val)
{
    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("ViewPositionTarget", val);

    emit sig_viewPositionTarget(val);
}

void MainWindow::onSetPosSliderOptionMW(int choice, double val)
{
    emit sig_setPosSliderOptionMW(choice, val);
}
void MainWindow::onSetVelSliderOptionMW(int choice, double val)
{
    emit sig_setVelSliderOptionMW(choice, val);
}
void MainWindow::onSetTrqSliderOptionMW(int choice, double val)
{
    emit sig_setTrqSliderOptionMW(choice, val);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    m_mutex.lock();

    this->setVisible(false);

    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_timer.stop();

    onStopAllSeq();

    if (m_tabPanel){
        disconnect(m_tabPanel, SIGNAL(currentChanged(int)), this, SLOT(onCurrentPartChanged(int)));
        for (int i = 0; i<m_tabPanel->count(); i++){
            if (m_tabPanel->widget(i)){
                delete m_tabPanel->widget(i);
            }
        }
        delete m_tabPanel;
        m_tabPanel = nullptr;
    }

    m_mutex.unlock();

    QMainWindow::closeEvent(event);

}




bool MainWindow::init(QStringList enabledParts,
                      ResourceFinder& finder,
                      bool debug_param_enabled,
                      bool speedview_param_enabled,
                      bool enable_calib_all)
{
    m_tabPanel = new QTabWidget(m_ui->mainContainer);

    if(!enable_calib_all)
    {
        m_calibSinglePart->setEnabled(false);
    }

    int errorCount = 0;
    QScrollArea *scroll = nullptr;
    PartItem *part = nullptr;
    m_finder = finder;
    m_user_script1 = m_finder.find("script1").asString();
    m_user_script2 = m_finder.find("script2").asString();

    struct robot_type
    {
        QTreeWidgetItem* tree_pointer;
        std::string      robot_name_without_slash;
    };

    struct part_type
    {
        std::string      robot_name_without_slash;
        std::string      robot_name;
        std::string      complete_name;
        std::string      part_name_without_slash;
        int              partindex;
    };

    std::map<std::string, robot_type> robots;
    std::map<std::string, part_type> parts;

    for (int i = 0; i < enabledParts.size(); i++)
    {
        std::string ss = enabledParts.at(i).toStdString();
        size_t b1 = ss.find('/');
        size_t b2 = ss.find('/', b1 + 1);
        std::string cur_robot_name = ss.substr(b1, b2 - b1);
        auto it = robots.find(cur_robot_name);
        if (it == robots.end())
        {
            robot_type r;
            r.robot_name_without_slash = cur_robot_name;
            if (r.robot_name_without_slash[0] == '/') {
                r.robot_name_without_slash.erase(0, 1);
            }
            r.tree_pointer = nullptr;
            robots[cur_robot_name]=r;
        }
        part_type p;
        p.partindex = i;
        p.complete_name = enabledParts.at(i).toStdString();
        p.part_name_without_slash = ss.substr(b2);
        if (p.part_name_without_slash[0] == '/') {
            p.part_name_without_slash.erase(0, 1);
        }
        p.robot_name = cur_robot_name;
        p.robot_name_without_slash = robots[cur_robot_name].robot_name_without_slash;
        parts[ss.substr(b2)] = p;
    }

    for (auto& robot : robots)
    {
        auto* robot_top = new QTreeWidgetItem();
        robot_top->setText(0, robot.first.c_str());
        m_ui->treeWidgetMode->addTopLevelItem(robot_top);
        robot_top->setExpanded(true);
        robot.second.tree_pointer = robot_top;
    }

    for (auto& i_parts : parts)
    {
        //JointItem *item = new JointItem();
        //layout->addWidget(item);
        scroll = new QScrollArea(m_tabPanel);
        scroll->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scroll->setWidgetResizable(true);
        std::string part_name = i_parts.first;
        std::string robot_name = i_parts.second.robot_name;
        std::string robot_name_without_slash = i_parts.second.robot_name_without_slash;
        std::string part_name_without_slash = i_parts.second.part_name_without_slash;
        int         part_id = i_parts.second.partindex;
        part = new PartItem(robot_name_without_slash.c_str(), part_id, part_name_without_slash.c_str(), finder, debug_param_enabled, speedview_param_enabled, enable_calib_all, scroll);

        if(part && !part->getInterfaceError())
        {
            connect(part,SIGNAL(sequenceActivated()),this,SLOT(onSequenceActivated()));
            connect(part,SIGNAL(sequenceStopped()),this,SLOT(onSequenceStopped()));
            connect(this,SIGNAL(sig_viewSpeedValues(bool)),part,SLOT(onViewSpeedValues(bool)));
            connect(this, SIGNAL(sig_viewCurrentValues(bool)), part, SLOT(onViewCurrentValues(bool)));
            connect(this, SIGNAL(sig_viewMotorPositions(bool)), part, SLOT(onViewMotorPositions(bool)));
            connect(this, SIGNAL(sig_viewDutyCycles(bool)), part, SLOT(onViewDutyCycles(bool)));
            connect(this, SIGNAL(sig_setPosSliderOptionMW(int, double)), part, SLOT(onSetPosSliderOptionPI(int, double)));
            connect(this, SIGNAL(sig_setVelSliderOptionMW(int, double)), part, SLOT(onSetVelSliderOptionPI(int, double)));
            connect(this, SIGNAL(sig_setTrqSliderOptionMW(int, double)), part, SLOT(onSetTrqSliderOptionPI(int, double)));
            connect(this,SIGNAL(sig_viewPositionTarget(bool)), part, SLOT(onViewPositionTarget(bool)));
            connect(this, SIGNAL(sig_enableControlVelocity(bool)), part, SLOT(onEnableControlVelocity(bool)));
            connect(this, SIGNAL(sig_enableControlMixed(bool)), part, SLOT(onEnableControlMixed(bool)));
            connect(this, SIGNAL(sig_enableControlPositionDirect(bool)), part, SLOT(onEnableControlPositionDirect(bool)));
            connect(this, SIGNAL(sig_enableControlPWM(bool)), part, SLOT(onEnableControlPWM(bool)));
            connect(this, SIGNAL(sig_enableControlCurrent(bool)), part, SLOT(onEnableControlCurrent(bool)));

            scroll->setWidget(part);
            m_tabPanel->addTab(scroll, part_name.c_str());
            if (part_id == 0)
            {
                QString auxName = part_name.c_str();
                auxName.replace(0, 1, QString(part_name.c_str()).at(0).toUpper());
                m_currentPartMenu->setTitle(QString("%1 Commands ").arg(auxName));
                this->m_partName->setText(QString("%1 Commands ").arg(auxName));
            }

            auto* mode = new QTreeWidgetItem();
            mode->setText(0, part_name.c_str());
            QTreeWidgetItem *tp = robots[i_parts.second.robot_name].tree_pointer;
            tp->addChild(mode);
            mode->setExpanded(false);
            part->setTreeWidgetModeNode(mode);
        }
        else
        {
            if(part)
            {
                delete part;
                part = nullptr;
            }
            if(scroll)
            {
                delete scroll;
                scroll = nullptr;
            }
            errorCount++;
        }
    }

    if((unsigned int)errorCount == parts.size())
    {
        return false;
    }

    auto* lay = new QHBoxLayout();
    lay->setMargin(0);
    lay->setSpacing(0);
    m_ui->mainContainer->setLayout(lay);
    m_ui->mainContainer->layout()->addWidget(m_tabPanel);
    connect(m_tabPanel, SIGNAL(currentChanged(int)), this, SLOT(onCurrentPartChanged(int)));

    QSettings settings("YARP","yarpmotorgui");
    bool speedVisible = settings.value("SpeedValuesVisible",false).toBool();
    bool motorPosVisible = settings.value("MotorPositionVisible", false).toBool();
    bool currentVisible = settings.value("CurrentsVisible", false).toBool();
    bool dutyVisible = settings.value("DutyCyclesVisible", false).toBool();

    onViewSpeeds(speedVisible);
    onViewCurrents(currentVisible);
    onViewMotorPositions(motorPosVisible);
    onViewDutyCycles(dutyVisible);
    return true;
}


void MainWindow::onCurrentPartChanged(int index)
{
    if(index < 0){
        return;
    }
    QString partName = m_tabPanel->tabText(index);

    QString auxName = partName;
    auxName.replace(0,1,partName.at(0).toUpper());
    m_currentPartMenu->setTitle(QString("%1 Commands").arg(auxName));
    this->m_partName->setText(QString("%1 Commands").arg(auxName));

    auto* scroll = (QScrollArea *)m_tabPanel->widget(index);
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->resizeWidget(part->width());

}

void MainWindow::onCalibSinglePart()
{
    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    if (QMessageBox::question(this, "Question", QString("Do you really want to recalibrate all joints of this part?")) == QMessageBox::Yes)
    {
        part->calibratePart(); // Error message is thrown inside
    }
}

void MainWindow::onExecuteScript1()
{
    if (m_user_script1 == "")
    {
        QMessageBox::information(this, "Info", QString("user script1 not specified. use --script1 option"));
        return;
    }

    if (QMessageBox::question(this, "Question", QString("Do you really want to execute user script1?")) == QMessageBox::Yes)
    {
        if (system(nullptr))
        {
            std::string script1_file = this->m_finder.findFileByName(m_user_script1);
            if (script1_file != "")
            {
                int r = system(script1_file.c_str());
                yDebug() << "yarpmotorgui_script1 returned value:" << r;
            }
            else
            {
                QMessageBox::information(this, "Info", QString("Unable to find script1 file"));
            }
        }
        else
        {
            QMessageBox::information(this, "Info", QString("System is unable to run script1"));
        }
    }
}

void MainWindow::onExecuteScript2()
{
    if (m_user_script2 == "")
    {
        QMessageBox::information(this, "Info", QString("user script2 not specified. use --script2 option"));
        return;
    }

    if (QMessageBox::question(this, "Question", QString("Do you really want to execute user script2?")) == QMessageBox::Yes)
    {
        if (system(nullptr))
        {
            std::string script2_file = this->m_finder.findFileByName(m_user_script2);
            if (script2_file != "")
            {
                int r = system(script2_file.c_str());
                yDebug() << "yarpmotorgui_script2 returned value:" << r;
            }
            else
            {
                QMessageBox::information(this, "Info", QString("Unable to find script2 file"));
            }
        }
        else
        {
            QMessageBox::information(this, "Info", QString("System is unable to run script2"));
        }
    }
}

void MainWindow::onHomeSinglePart()
{
    if (QMessageBox::question(this, "Question", "Do you really want to home all joints of this part?") != QMessageBox::Yes){
        return;
    }

    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if (!part){
        return;
    }

    part->homePart();
}

void MainWindow::onHomeAllParts()
{
    if (QMessageBox::question(this, "Question", "Do you really want to home all parts?") != QMessageBox::Yes){
        return;
    }

    if (!m_tabPanel){
        return;
    }

    for (int i = 0; i<m_tabPanel->count(); i++)
    {
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if (!part)
        {
            continue;
        }

        part->homePart();
    }
}

void MainWindow::onHomeAllPartsToCustomPosition(const yarp::os::Bottle& positionElement)
{
    if (QMessageBox::question(this, "Question", "Do you really want to home all parts?") != QMessageBox::Yes){
        return;
    }

    if (!m_tabPanel){
        return;
    }

    for (int i = 0; i<m_tabPanel->count(); i++)
    {
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if(!part)
        {
            continue;
        }
        part->homeToCustomPosition(positionElement);
    }
}

void MainWindow::onHomeSinglePartToCustomPosition(const yarp::os::Bottle& positionElement)
{
    if (QMessageBox::question(this, "Question", "Do you really want to home all joints of this part?") != QMessageBox::Yes){
        return;
    }

    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->homeToCustomPosition(positionElement);
}

void MainWindow::onIdleSinglePart()
{
    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->idlePart();
}

void MainWindow::onCycleTimeAllSeq()
{
    if (!m_tabPanel){
        return;
    }

    QString notSelectedParts;

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
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
    if (!m_tabPanel){
        return;
    }

    QString notSelectedParts;

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
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
    if (!m_tabPanel){
        return;
    }

    QString notSelectedParts;

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
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
    if (!m_tabPanel){
        return;
    }

    QString notSelectedParts;

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
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
    if (!m_tabPanel){
        return;
    }


    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->loadSequence();

    }
}

void MainWindow::onSaveAllSeq()
{
    if (!m_tabPanel){
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, QString("Save Sequence for all parts as:"), QDir::homePath());

    for (int i = 0; i<m_tabPanel->count(); i++)
    {
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if(!part)
        {
            continue;
        }
        part->saveSequence(fileName);
        part->closeSequenceWindow();
    }
}

void MainWindow::onStopAllSeq()
{
    if (!m_tabPanel){
        return;
    }


    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->stopSequence();

    }

 }

void MainWindow::onGoAll()
{
    if (!m_tabPanel){
        return;
    }

    QString notSelectedParts;

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
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

void MainWindow::onIdleAllParts()
{
    if (QMessageBox::question(this, "Question", "Do you really want to idle all parts?") != QMessageBox::Yes){
        return;
    }

    if (!m_tabPanel){
        return;
    }

    for (int i = 0; i<m_tabPanel->count(); i++)
    {
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if (!part){
            continue;
        }

        part->idlePart();
    }
}

void MainWindow::onRunAllParts()
{
    if (!m_tabPanel){
        return;
    }

    for (int i = 0; i<m_tabPanel->count(); i++){
        auto* scroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* part = (PartItem*)scroll->widget();
        if(!part){
            continue;
        }

        part->runPart();
    }
}

void MainWindow::onRunSinglePart()
{
    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }

    part->runPart();
}

void MainWindow::onOpenSequenceTab()
{
    if (!m_tabPanel){
        return;
    }

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    auto* part = (PartItem*)scroll->widget();
    if(!part){
        return;
    }
    part->openSequenceWindow();
}



void MainWindow::onUpdate()
{
    if (!m_tabPanel){
        return;
    }
    m_mutex.lock();

    auto* scroll = (QScrollArea *)m_tabPanel->currentWidget();
    if(!scroll){
        m_mutex.unlock();
        return;
    }
    auto* currentPart = (PartItem*)scroll->widget();
    if(!currentPart){
        m_mutex.unlock();
        return;
    }
    for (int i = 0; i<m_tabPanel->count(); i++)
    {
        auto* tabScroll = (QScrollArea *)m_tabPanel->widget(i);
        auto* item = (PartItem*)tabScroll->widget();
        item->updateControlMode();
        updateModesTree(item);
        if(item == currentPart)
        {
            if (item->updatePart() == false)
            {
                //this part is disconnected!
            }
        }
    }
    m_mutex.unlock();
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
    case JointItem::Pwm:{
        mode = pwmColor;
        break;
    }
    case JointItem::Current:{
        mode = currentColor;
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
    case JointItem::Pwm:{
        mode = "PWM";
        break;
    }
    case JointItem::Current:{
        mode = "Current";
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
            auto* jointNode = new QTreeWidgetItem(parentNode);
            jointNode->setText(0,QString("Joint %1").arg(i));
            jointNode->setText(1,mode);
            QColor c = getColorMode(modes.at(i));
            jointNode->setBackground(0,c);
            jointNode->setBackground(1,c);
            jointNode->setForeground(0,QColor(Qt::black));
            jointNode->setForeground(1,QColor(Qt::black));


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
        bool foundFaultPart = false;
        for(int i=0;i<parentNode->childCount();i++){
            QTreeWidgetItem *item = parentNode->child(i);
            QString mode;
            QColor c = getColorMode(modes.at(i));
            mode = getStringMode(modes.at(i));

            if(c == hwFaultColor){
                foundFaultPart = true;
                if(item->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                    item->setIcon(0,QIcon(":/warning.svg"));
                    item->setData(0,Qt::UserRole,TREEMODE_WARN);
                }
            }else{
                item->setIcon(0,QIcon());
                item->setData(0,Qt::UserRole,TREEMODE_OK);
            }

            if(parentNode->isExpanded()){
                if(item->text(1) != mode){
                    item->setText(1,mode);
                }
                if(item->background(0) != c){
                    item->setBackground(0,c);
                    item->setBackground(1,c);
                    item->setForeground(0,QColor(Qt::black));
                    item->setForeground(1,QColor(Qt::black));
                }
            }
        }

        if(!foundFaultPart){
            if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_OK){
                parentNode->setBackground(0,QColor("white"));
                parentNode->setIcon(0,QIcon(":/apply.svg"));
                parentNode->setData(0,Qt::UserRole,TREEMODE_OK);
            }
        }else{
            if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                parentNode->setBackground(0,hwFaultColor);
                parentNode->setIcon(0,QIcon(":/warning.svg"));
                parentNode->setData(0,Qt::UserRole,TREEMODE_WARN);
            }

        }
    }

}



ModesTreeWidget::ModesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}

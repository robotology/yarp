/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 *          Francesco Nori <francesco.nori@iit.it>
 *          Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "partitem.h"
#include "log.h"

#include <yarp/os/all.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <QDebug>
#include <QEvent>
#include <QResizeEvent>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QMessageBox>
#include <QSettings>
#include <cmath>

PartItem::PartItem(QString robotName, int id, QString partName, ResourceFinder *finder,
                   bool debug_param_enabled,
                   bool speedview_param_enabled,
                   bool enable_calib_all, QWidget *parent) :   QWidget(parent)
{
    layout = new FlowLayout();
    setLayout(layout);
    slow_k = 0;

    partId = id;
    this->finder = NULL;
    node = NULL;
    currentPidDlg = NULL;
    sequenceWindow = NULL;
    this->finder = finder;
    this->partName = partName;
    mixedEnabled = false;
    positionDirectEnabled = false;
    openloopEnabled = false;

    //PolyDriver *cartesiandd[MAX_NUMBER_ACTIVATED];

    QString robotPartPort = QString("/%1/%2").arg(robotName).arg(partName);


    QString robotPartDebugPort = QString("/%1/debug/%2").arg(robotName).arg(partName);

    //checking existence of the port
    int ind = 0;
    QString portLocalName = QString("/%1/yarpmotorgui%2/%3").arg(robotName).arg(ind).arg(partName);


    QString nameToCheck = portLocalName;
    nameToCheck += "/rpc:o";

    //   NameClient &nic=NameClient::getNameClient();
    yDebug("Checking the existence of: %s \n", nameToCheck.toLatin1().data());
    //                    Address adr=nic.queryName(nameToCheck.c_str());

    Contact adr=Network::queryName(nameToCheck.toLatin1().data());

    //Contact c = yarp::os::Network::queryName(portLocalName.c_str());
    yDebug("ADDRESS is: %s \n", adr.toString().c_str());

    while(adr.isValid()){
        ind++;

        portLocalName = QString("/%1/yarpmotorgui%2/%3").arg(robotName).arg(ind).arg(partName);;

        nameToCheck = portLocalName;
        nameToCheck += "/rpc:o";
        // adr=nic.queryName(nameToCheck.c_str());
        adr=Network::queryName(nameToCheck.toLatin1().data());
    }

    interfaceError = false;

    // Initializing the polydriver options and instantiating the polydrivers
    partOptions.put(  "local", portLocalName.toLatin1().data());
    partOptions.put( "device", "remote_controlboard");
    partOptions.put( "remote", robotPartPort.toLatin1().data());
    partOptions.put("carrier", "udp");

    partsdd = new PolyDriver();

    // Opening the drivers
    interfaceError = !openPolyDrivers();
    if (interfaceError==true)
    {
        yError("Opening PolyDriver for part %s failed...", robotPartPort.toLatin1().data());
        QMessageBox::critical(0,"Error opening a device", QString("Error while opening device for part ").append(robotPartPort.toLatin1().data()));
    }

    /*********************************************************************/
    /**************** PartMover Content **********************************/

    if (!finder->isNull()){
        yDebug("Setting a valid finder \n");
    }

    QString sequence_portname = QString("/yarpmotorgui/%1/sequence:o").arg(partName);
    sequence_port.open(sequence_portname.toLatin1().data());

    initInterfaces();
    openInterfaces();

    // COPY_STORED_POS=0;
    // COPY_STORED_VEL=0;
    // COPY_SEQUENCE=0;
    // COPY_TIMING=0;

    if (interfaceError == false)
    {
        // yDebug("Allocating memory \n");
        // STORED_POS   = new double* [NUMBER_OF_STORED];
        // STORED_VEL   = new double* [NUMBER_OF_STORED];
        // CURRENT_POS_UPDATE = new bool [NUMBER_OF_STORED];

        // int j,k;
        // for (j = 0; j < NUMBER_OF_STORED; j++){
        //     CURRENT_POS_UPDATE[j] = true;
        //     STORED_POS[j] = new double [MAX_NUMBER_OF_JOINTS];
        //     STORED_VEL[j] = new double [MAX_NUMBER_OF_JOINTS];
        // }

        // SEQUENCE     = new int [NUMBER_OF_STORED];
        // INV_SEQUENCE = new int [NUMBER_OF_STORED];
        // TIMING = new double    [NUMBER_OF_STORED];
        // index  = new int [MAX_NUMBER_OF_JOINTS];
        // SEQUENCE_ITERATOR = new int [0];
        // timeout_seqeunce_id = new uint [0];
        // entry_id = new uint [0];
        // *entry_id = -1;
        // timeout_seqeunce_rate = new uint [0];


        // for (j = 0; j < NUMBER_OF_STORED; j++){
        //     SEQUENCE[j] = -1;
        //     TIMING[j] = -0.1;
        // }

        double positions[MAX_NUMBER_OF_JOINTS];

        bool ret=false;
        Time::delay(0.050);
        do {
            ret=iencs->getEncoders(positions);
            if (!ret) {
                yError("%s iencs->getEncoders() failed, retrying...\n", partName.toLatin1().data());
                Time::delay(0.050);
            }
        }
        while (!ret);

        yInfo("%s iencs->getEncoders() ok!\n", partName.toLatin1().data());
        double min_pos = 0;
        double max_pos = 100;
        double min_vel = 0;
        double max_vel = 100;
        yarp::os::ConstString jointname;
        //char buffer[40] = {'i', 'n', 'i', 't'};

        int NUMBER_OF_JOINTS;
        iPos->getAxes(&NUMBER_OF_JOINTS);

        for (int k = 0; k<NUMBER_OF_JOINTS; k++)
        {
            // //init stored
            // for(j = 0; j < NUMBER_OF_STORED; j++){
            //     STORED_POS[j][k] = 0.0;
            // }

            //index[k]=k;
            bool bpl = iLim->getLimits(k, &min_pos, &max_pos);
            bool bvl = iLim->getVelLimits(k, &min_vel, &max_vel);
            if (bpl == false)
            {
                yError() << "Error while getting position limits, part " << partName.toStdString() << " joint " << k;
            }
            if (bvl == false || (min_vel == 0 && max_vel == 0))
            {
                yError() << "Error while getting velocity limits, part " << partName.toStdString() << " joint " << k;
            }

            QSettings settings("YARP", "yarpmotorgui");
            double max_slider_vel = settings.value("velocity_slider_limit", 100.0).toDouble();
            if (max_vel > max_slider_vel) max_vel = max_slider_vel;

            iinfo->getAxisName(k, jointname);
            yarp::dev::JointTypeEnum jtype = yarp::dev::VOCAB_JOINTTYPE_REVOLUTE;
            bool bjt = iinfo->getJointType(k, jtype);

            Pid myPid(0,0,0,0,0,0);
            yarp::os::Time::delay(0.005);
            iPid->getPid(k, &myPid);
            
            JointItem *joint = new JointItem(k);
            joint->setJointName(jointname.c_str());
            joint->setOpenLoopRange(-myPid.max_output,myPid.max_output);
            layout->addWidget(joint);
            joint->setPositionRange(min_pos, max_pos);
            joint->setVelocityRange(min_vel, max_vel);
            joint->setTrajectoryVelocityRange(max_vel);
            joint->setTorqueRange(5.0);
            joint->setUnits(jtype);
            joint->enableControlPositionDirect(positionDirectEnabled);
            joint->enableControlMixed(mixedEnabled);
            joint->enableControlOpenloop(openloopEnabled);

            int val_pos_choice = settings.value("val_pos_choice", 0).toInt();
            int val_trq_choice = settings.value("val_trq_choice", 0).toInt();
            int val_vel_choice = settings.value("val_vel_choice", 0).toInt();
            double val_pos_custom_step = settings.value("val_pos_custom_step", 1.0).toDouble();
            double val_trq_custom_step = settings.value("val_trq_custom_step", 1.0).toDouble();
            double val_vel_custom_step = settings.value("val_vel_custom_step", 1.0).toDouble();
            onSetPosSliderOptionPI(val_pos_choice, val_pos_custom_step);
            onSetVelSliderOptionPI(val_vel_choice, val_vel_custom_step);
            onSetTrqSliderOptionPI(val_trq_choice, val_trq_custom_step);

            joint->setEnabledOptions(debug_param_enabled,
                                     speedview_param_enabled,
                                     enable_calib_all);

            connect(joint, SIGNAL(changeMode(int,JointItem*)), this, SLOT(onJointChangeMode(int,JointItem*)));
            connect(joint, SIGNAL(changeInteraction(int,JointItem*)), this, SLOT(onJointInteraction(int,JointItem*)));
            connect(joint, SIGNAL(sliderTrajectoryPositionCommand(double, int)), this, SLOT(onSliderTrajectoryPositionCommand(double, int)));
            connect(joint, SIGNAL(sliderTrajectoryVelocityCommand(double, int)), this, SLOT(onSliderTrajectoryVelocityCommand(double, int)));
            connect(joint, SIGNAL(sliderMixedPositionCommand(double, int)), this, SLOT(onSliderMixedPositionCommand(double, int)));
            connect(joint, SIGNAL(sliderMixedVelocityCommand(double, int)), this, SLOT(onSliderMixedVelocityCommand(double, int)));
            connect(joint, SIGNAL(sliderTorqueCommand(double, int)), this, SLOT(onSliderTorqueCommand(double, int)));
            connect(joint, SIGNAL(sliderDirectPositionCommand(double, int)), this, SLOT(onSliderDirectPositionCommand(double, int)));
            connect(joint, SIGNAL(sliderOpenloopCommand(double, int)), this, SLOT(onSliderOpenloopCommand(double, int)));
            connect(joint, SIGNAL(sliderVelocityCommand(double, int)), this, SLOT(onSliderVelocityCommand(double, int)));
            connect(joint, SIGNAL(homeClicked(JointItem*)),this,SLOT(onHomeClicked(JointItem*)));
            connect(joint, SIGNAL(idleClicked(JointItem*)),this,SLOT(onIdleClicked(JointItem*)));
            connect(joint, SIGNAL(runClicked(JointItem*)),this,SLOT(onRunClicked(JointItem*)));
            connect(joint, SIGNAL(pidClicked(JointItem*)),this,SLOT(onPidClicked(JointItem*)));
            connect(joint, SIGNAL(calibClicked(JointItem*)),this,SLOT(onCalibClicked(JointItem*)));
        }
    }

    /*********************************************************************/
    /*********************************************************************/

    cycleTimer.setSingleShot(true);
    cycleTimer.setTimerType(Qt::PreciseTimer);
    connect(&cycleTimer,SIGNAL(timeout()),this,SLOT(onCycleTimerTimeout()),Qt::QueuedConnection);

    cycleTimeTimer.setSingleShot(true);
    cycleTimeTimer.setTimerType(Qt::PreciseTimer);
    connect(&cycleTimeTimer,SIGNAL(timeout()),this,SLOT(onCycleTimeTimerTimeout()),Qt::QueuedConnection);


    runTimeTimer.setSingleShot(true);
    runTimeTimer.setTimerType(Qt::PreciseTimer);
    connect(&runTimeTimer,SIGNAL(timeout()),this,SLOT(onRunTimerTimeout()),Qt::QueuedConnection);

    runTimer.setSingleShot(true);
    runTimer.setTimerType(Qt::PreciseTimer);
    connect(&runTimer,SIGNAL(timeout()),this,SLOT(onRunTimeout()),Qt::QueuedConnection);
}

PartItem::~PartItem()
{
    disconnect(&runTimer,SIGNAL(timeout()),this,SLOT(onRunTimeout()));
    runTimer.stop();

    disconnect(&runTimeTimer,SIGNAL(timeout()),this,SLOT(onRunTimerTimeout()));
    runTimeTimer.stop();

    disconnect(&cycleTimer,SIGNAL(timeout()),this,SLOT(onCycleTimerTimeout()));
    cycleTimer.stop();

    disconnect(&cycleTimeTimer,SIGNAL(timeout()),this,SLOT(onCycleTimeTimerTimeout()));
    cycleTimeTimer.stop();

    if(sequenceWindow){
        sequenceWindow->hide();
        delete sequenceWindow;
    }

    for(int i=0;i<layout->count();i++){
        JointItem *joint = (JointItem *)layout->itemAt(i)->widget();
        if(joint){
            disconnect(joint,SIGNAL(changeMode(int,JointItem*)), this, SLOT(onJointChangeMode(int,JointItem*)));
            disconnect(joint,SIGNAL(changeInteraction(int,JointItem*)), this, SLOT(onJointInteraction(int,JointItem*)));
            disconnect(joint,SIGNAL(homeClicked(JointItem*)),this,SLOT(onHomeClicked(JointItem*)));
            disconnect(joint,SIGNAL(idleClicked(JointItem*)),this,SLOT(onIdleClicked(JointItem*)));
            disconnect(joint,SIGNAL(runClicked(JointItem*)),this,SLOT(onRunClicked(JointItem*)));
            disconnect(joint,SIGNAL(pidClicked(JointItem*)),this,SLOT(onPidClicked(JointItem*)));
            disconnect(joint,SIGNAL(calibClicked(JointItem*)),this,SLOT(onCalibClicked(JointItem*)));
            delete joint;
        }
    }

    if(partsdd){
        partsdd->close();
    }
}

bool PartItem::openPolyDrivers()
{
    partsdd->open(partOptions);
    if (!partsdd->isValid()) {
        return false;
    }

    #ifdef DEBUG_INTERFACE
        if (debug_param_enabled)
        {
            debugdd->open(debugOptions);
            if(!debugdd->isValid()){
                yError("Problems opening the debug client!");
            }
        } else {
            debugdd = NULL;
        }
    #endif
    return true;
}

void PartItem::initInterfaces()
{
    yDebug("Initializing interfaces...");
    //default value for unopened interfaces
    iPos      = NULL;
    iVel      = NULL;
    iVar      = NULL;
    iDir      = NULL;
    iencs     = NULL;
    iAmp      = NULL;
    iPid      = NULL;
    iOpl      = NULL;
    iTrq      = NULL;
    iImp = NULL;
    iLim      = NULL;
    cal       = NULL;
    ctrlmode2 = NULL;
    iinteract = NULL;
    remCalib  = NULL;
}

bool PartItem::openInterfaces()
{
    yDebug("Opening interfaces...");
    bool ok = false;

    if (partsdd->isValid()) {
        ok  = partsdd->view(iPid);
        if(!ok){
            yError("...iPid was not ok...");
        }
        ok &= partsdd->view(iAmp);
        if(!ok){
            yError("...iAmp was not ok...");
        }
        ok &= partsdd->view(iPos);
        if(!ok){
            yError("...iPos was not ok...");
        }
        ok &= partsdd->view(iDir);
        if(!ok){
            yError("...posDirect was not ok...");
        }
        ok &= partsdd->view(iVel);
        if(!ok){
            yError("...iVel was not ok...");
        }
        ok &= partsdd->view(iLim);
        if(!ok){
            yError("...iLim was not ok...");
        }
        ok &= partsdd->view(iencs);
        if(!ok){
            yError("...enc was not ok...");
        }
        ok &= partsdd->view(cal);
        if(!ok){
            yError("...cal was not ok...");
        }
        ok &= partsdd->view(iTrq);
        if(!ok){
            yError("...trq was not ok...");
        }
        ok = partsdd->view(iOpl);
        if(!ok){
            yError("...opl was not ok...");
        }
        ok &= partsdd->view(iImp);
        if(!ok){
            yError("...imp was not ok...");
        }
        ok &= partsdd->view(ctrlmode2);
        if(!ok){
            yError("...ctrlmode2 was not ok...");
        }
        ok &= partsdd->view(iinteract);
        if(!ok){
            yError("...iinteract was not ok...");
        }
        //optional interfaces
        if (!partsdd->view(iVar))
        {
            yError("...iVar was not ok...");
        }

        if (!partsdd->view(remCalib))
        {
            yError("...remCalib was not ok...");
        }

        if (!partsdd->view(iinfo))
        {
            yError("...axisInfo was not ok...");
        }

        if (!ok) {
            yError("Error while acquiring interfaces!");
            QMessageBox::critical(0,"Problems acquiring interfaces.","Check if interface is running");
            interfaceError=true;
        }
    }
    else
    {
        yError("Device driver was not valid!");
        interfaceError=true;
    }

    return !interfaceError;
}

bool PartItem::getInterfaceError()
{
    return interfaceError;
}

QString PartItem::getPartName()
{
    return partName;
}

void PartItem::onSliderOpenloopCommand(double torqueVal, int index)
{
    iOpl->setRefOutput(index, torqueVal);
}

void PartItem::onSliderVelocityCommand(double speedVal, int index)
{
    iVel->velocityMove(index,speedVal);
}

void PartItem::onSliderTorqueCommand(double torqueVal, int index)
{
    iTrq->setRefTorque(index, torqueVal);
}

void PartItem::onSliderTrajectoryVelocityCommand(double trajspeedVal, int index)
{
    iPos->setRefSpeed(index, trajspeedVal);
}


void PartItem::onSliderDirectPositionCommand(double dirpos, int index)
{
    int mode;
    ctrlmode2->getControlMode(index, &mode);
    if (mode == VOCAB_CM_POSITION_DIRECT)
    {
        iDir->setPosition(index, dirpos);
    }
    else
    {
        yWarning("Joint not in position direct mode so cannot send references");
    }
}

void PartItem::onSliderTrajectoryPositionCommand(double posVal, int index)
{
    int mode;
    ctrlmode2->getControlMode(index, &mode);

    if ( mode == VOCAB_CM_POSITION) 
    {
        iPos->positionMove(index, posVal);
    }
    else
    {
        yWarning("Joint not in position mode so cannot send references");
    }
}

void PartItem::onSliderMixedPositionCommand(double posVal, int index)
{
    int mode;
    ctrlmode2->getControlMode(index, &mode);

    if ( mode == VOCAB_CM_MIXED)
    {
        iPos->positionMove(index, posVal);
    }
    else
    {
        LOG_ERROR("Joint not in mixed mode so cannot send references");
    }
}

void PartItem::onSliderMixedVelocityCommand( double vel, int index)
{
    int mode;
    ctrlmode2->getControlMode(index, &mode);

    if (mode == VOCAB_CM_MIXED)
    {
        iVel->velocityMove(index, vel);
    }
    else
    {
        LOG_ERROR("Joint not in mixed mode so cannot send references");
    }
}

void PartItem::onJointInteraction(int interaction,JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    switch (interaction) {
    case JointItem::Compliant:
        yInfo("interaction mode of joint %d set to COMPLIANT", jointIndex);
        iinteract->setInteractionMode(jointIndex, (yarp::dev::InteractionModeEnum) VOCAB_IM_COMPLIANT);
        break;
    case JointItem::Stiff:
        yInfo("interaction mode of joint %d set to STIFF", jointIndex);
        iinteract->setInteractionMode(jointIndex, (yarp::dev::InteractionModeEnum) VOCAB_IM_STIFF);
        break;
    default:
        break;
    }
}


void PartItem::onSendOpenLoop(int jointIndex, int openLoopVal)
{
    double openloop_reference = 0;
    double openloop_current_pwm = 0;

    iOpl->setRefOutput(jointIndex, openLoopVal);

    yarp::os::Time::delay(0.010);
    iOpl->getRefOutput(jointIndex, &openloop_reference);  //This is the reference reference
    yarp::os::Time::delay(0.010);
    iOpl->getOutput(jointIndex, &openloop_current_pwm);  //This is the reak PWM output

    if(currentPidDlg){
        currentPidDlg->initOpenLoop(openloop_reference,openloop_current_pwm);
    }
}

void PartItem::onSendStiffness(int jointIdex,double stiff,double damp,double force)
{
    Q_UNUSED(force);
    double stiff_val=0;
    double damp_val=0;
    double offset_val=0;

    iImp->setImpedance(jointIdex, stiff, damp);
    //imp->setImpedanceOffset(jointIdex, force);
    yarp::os::Time::delay(0.005);
    iImp->getImpedance(jointIdex, &stiff_val, &damp_val);
    iImp->getImpedanceOffset(jointIdex, &offset_val);

    //update the impedance limits
    double stiff_max=0.0;
    double stiff_min=0.0;
    double damp_max=0.0;
    double damp_min=0.0;
    double off_max=0.0;
    double off_min=0.0;
    iImp->getCurrentImpedanceLimit(jointIdex, &stiff_min, &stiff_max, &damp_min, &damp_max);
    iTrq->getTorqueRange(jointIdex, &off_min, &off_max);

    if(currentPidDlg){
        currentPidDlg->initStiffness(stiff_val,stiff_min,stiff_max,
                                     damp_val,damp_min,damp_max,
                                     offset_val,off_min,off_max);
    }


}

void PartItem::onSendTorquePid(int jointIndex,Pid newPid,MotorTorqueParameters newTrqParam)
{
    Pid myTrqPid(0,0,0,0,0,0);
    yarp::dev::MotorTorqueParameters TrqParam;
    iTrq->setTorquePid(jointIndex, newPid);

    iTrq->setMotorTorqueParams(jointIndex, newTrqParam);
    yarp::os::Time::delay(0.005);
    iTrq->getTorquePid(jointIndex, &myTrqPid);
    iTrq->getMotorTorqueParams(jointIndex, &TrqParam);

    if(currentPidDlg){
        currentPidDlg->initTorque(myTrqPid,TrqParam);
    }
}

void PartItem::onSendPositionPid(int jointIndex,Pid newPid)
{
    Pid myPosPid(0,0,0,0,0,0);
    iPid->setPid(jointIndex, newPid);
    yarp::os::Time::delay(0.005);
    iPid->getPid(jointIndex, &myPosPid);

    if(currentPidDlg){
        currentPidDlg->initPosition(myPosPid);
    }
}

void PartItem::onSendVelocityPid(int jointIndex, Pid newPid)
{
    Pid myVelPid(0, 0, 0, 0, 0, 0);
    iVel->setVelPid(jointIndex, newPid);
    yarp::os::Time::delay(0.005);
    iVel->getVelPid(jointIndex, &myVelPid);

    if (currentPidDlg){
        currentPidDlg->initVelocity(myVelPid);
    }
}

void PartItem::onRefreshPids(int jointIndex)
{
    Pid myPosPid(0, 0, 0, 0, 0, 0);
    Pid myTrqPid(0, 0, 0, 0, 0, 0);
    Pid myVelPid(0, 0, 0, 0, 0, 0);
    Pid myCurPid(0, 0, 0, 0, 0, 0);
    MotorTorqueParameters motorTorqueParams;
    double stiff_val = 0;
    double damp_val = 0;
    double stiff_max = 0;
    double damp_max = 0;
    double off_max = 0;
    double stiff_min = 0;
    double damp_min = 0;
    double off_min = 0;
    double impedance_offset_val = 0;
    double openloop_reference = 0;
    double openloop_current_pwm = 0;

    iImp->getCurrentImpedanceLimit(jointIndex, &stiff_min, &stiff_max, &damp_min, &damp_max);
    iTrq->getTorqueRange(jointIndex, &off_min, &off_max);

    // Position
    iPid->getPid(jointIndex, &myPosPid);
    yarp::os::Time::delay(0.005);

    // Velocity
    iVel->getVelPid(jointIndex, &myVelPid);
    yarp::os::Time::delay(0.005);

    // Current
    //???????iCur->getCurPid(jointIndex, &myCurPid);
    yarp::os::Time::delay(0.005);

    // Torque
    iTrq->getTorquePid(jointIndex, &myTrqPid);
    iTrq->getMotorTorqueParams(jointIndex, &motorTorqueParams);
    yarp::os::Time::delay(0.005);

    //Stiff
    iImp->getImpedance(jointIndex, &stiff_val, &damp_val);
    iImp->getImpedanceOffset(jointIndex, &impedance_offset_val);
    yarp::os::Time::delay(0.005);

    // Openloop
    iOpl->getRefOutput(jointIndex, &openloop_reference);
    iOpl->getOutput(jointIndex, &openloop_current_pwm);

    if (currentPidDlg)
    {
        currentPidDlg->initPosition(myPosPid);
        currentPidDlg->initTorque(myTrqPid, motorTorqueParams);
        currentPidDlg->initVelocity(myVelPid);
        currentPidDlg->initCurrent(myCurPid);
        currentPidDlg->initStiffness(stiff_val, stiff_min, stiff_max, damp_val, damp_min, damp_max, impedance_offset_val, off_min, off_max);
        currentPidDlg->initOpenLoop(openloop_reference, openloop_current_pwm);
        currentPidDlg->initRemoteVariables(iVar);
    }
}

void PartItem::onSendCurrentPid(int jointIndex, Pid newPid)
{
    Pid myCurPid(0, 0, 0, 0, 0, 0);
    //????iCur->setCurPid(jointIndex, newPid);
    yarp::os::Time::delay(0.005);
    //????iCur->getCurPid(jointIndex, &myCurPid);

    if (currentPidDlg){
        currentPidDlg->initCurrent(myCurPid);
    }
}

void PartItem::onSendSingleRemoteVariable(std::string key, yarp::os::Bottle val)
{
    iVar->setRemoteVariable(key,val);
    yarp::os::Time::delay(0.005);
}

void PartItem::onUpdateAllRemoteVariables()
{
    if (currentPidDlg){
        currentPidDlg->initRemoteVariables(iVar);
    }
}

void PartItem::onCalibClicked(JointItem *joint)
{
    if(!remCalib)
    {
        QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return;
    }

    if(QMessageBox::question(this,"Question","Do you want really to recalibrate the joint?") != QMessageBox::Yes){
        return;
    }
    if(!remCalib->calibrateSingleJoint(joint->getJointIndex()) )
    {
        // provide better feedback to user by verifying if the calibrator device was set or not
        bool isCalib = false;
        remCalib->isCalibratorDevicePresent(&isCalib);
        if(!isCalib)
            QMessageBox::critical(this,"Calibration failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file has the 'Calibrator' keyword in the attach phase"));
        else
            QMessageBox::critical(this,"Calibration failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
    }

}

void PartItem::onPidClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();

    currentPidDlg = new PidDlg(partName,jointIndex);
    connect(currentPidDlg, SIGNAL(sendPositionPid(int,Pid)),this,SLOT(onSendPositionPid(int,Pid)));
    connect(currentPidDlg, SIGNAL(sendVelocityPid(int, Pid)), this, SLOT(onSendVelocityPid(int, Pid)));
    connect(currentPidDlg, SIGNAL(sendCurrentPid(int, Pid)), this, SLOT(onSendCurrentPid(int, Pid)));
    connect(currentPidDlg, SIGNAL(sendSingleRemoteVariable(std::string, yarp::os::Bottle)), this, SLOT(onSendSingleRemoteVariable(std::string, yarp::os::Bottle)));
    connect(currentPidDlg, SIGNAL(updateAllRemoteVariables()), this, SLOT(onUpdateAllRemoteVariables()));
    connect(currentPidDlg, SIGNAL(sendTorquePid(int,Pid,MotorTorqueParameters)),this,SLOT(onSendTorquePid(int,Pid,MotorTorqueParameters)));
    connect(currentPidDlg, SIGNAL(sendStiffness(int,double,double,double)),this,SLOT(onSendStiffness(int,double,double,double)));
    connect(currentPidDlg, SIGNAL(sendOpenLoop(int,int)),this,SLOT(onSendOpenLoop(int,int)));
    connect(currentPidDlg, SIGNAL(refreshPids(int)), this, SLOT(onRefreshPids(int)));

    this->onRefreshPids(jointIndex);

    currentPidDlg->exec();

    delete currentPidDlg;
    currentPidDlg = NULL;
}

void PartItem::onRunClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    double posJoint;
    while (!iencs->getEncoder(jointIndex, &posJoint)){
        Time::delay(0.001);
    }

    ctrlmode2->setControlMode(jointIndex,VOCAB_CM_POSITION);

    //gtk_range_set_value ((GtkRange *) (sliderAry[*joint]), posJoint);
}

void PartItem::onIdleClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    ctrlmode2->setControlMode(jointIndex,VOCAB_CM_FORCE_IDLE);
}

void PartItem::onHomeClicked(JointItem *joint)
{
    int NUMBER_OF_JOINTS;
    const int jointIndex = joint->getJointIndex();
    iPos->getAxes(&NUMBER_OF_JOINTS);

    QString zero = QString("%1_zero").arg(partName);

    if (!finder->isNull() && !finder->findGroup(zero.toLatin1().data()).isNull()){
        bool ok = true;
        Bottle xtmp;
        xtmp = finder->findGroup(zero.toLatin1().data()).findGroup("PositionZero");
        ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
        double positionZero = xtmp.get(jointIndex+1).asDouble();
        //fprintf(stderr, "%f\n", positionZero);

        xtmp = finder->findGroup(zero.toLatin1().data()).findGroup("VelocityZero");
        //fprintf(stderr, "VALUE VEL is %d \n", fnd->findGroup(buffer2).find("VelocityZero").toString().c_str());
        ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
        double velocityZero = xtmp.get(jointIndex+1).asDouble();
        //fprintf(stderr, "%f\n", velocityZero);

        if(!ok){
            QMessageBox::critical(this,"Error", QString("Check the number of entries in the group %1").arg(zero));
        } else {
            iPos->setRefSpeed(jointIndex, velocityZero);
            iPos->positionMove(jointIndex, positionZero);
        }
    }
    else
    {
        if(!remCalib)
        {
            QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
            return;
        }

        QMessageBox::information(this,"Info", QString("Asking the yarprobotinterface to homing part %1 through the remoteCalibrator interface, since no custom zero group found in the supplied file").arg(partName));
        if(!remCalib->homingSingleJoint(jointIndex) )
        {
            // provide better feedback to user by verifying if the calibrator device was set or not
            bool isCalib = false;
            remCalib->isCalibratorDevicePresent(&isCalib);
            if(!isCalib)
                QMessageBox::critical(this,"Calibration failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(partName));
            else
                QMessageBox::critical(this,"Calibration failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
        }
    }
}

void PartItem::onJointChangeMode(int mode,JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    switch (mode) {
    case JointItem::Idle:{
        yInfo("joint: %d in IDLE mode", jointIndex);
        if(ctrlmode2){
            ctrlmode2->setControlMode(jointIndex, VOCAB_CM_IDLE);
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    case JointItem::Position:{
        yInfo("joint: %d in POSITION mode", jointIndex);
        if(ctrlmode2){
            ctrlmode2->setControlMode(jointIndex, VOCAB_CM_POSITION);
            joint->resetTarget();
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    case JointItem::PositionDirect:{
        //if(positionDirectEnabled){
            yInfo("joint: %d in POSITION DIRECT mode", jointIndex);
            if(ctrlmode2){
                joint->resetTarget();
                ctrlmode2->setControlMode(jointIndex, VOCAB_CM_POSITION_DIRECT);
            } else {
                yError("ERROR: cannot do!");
            }
            break;
        /*}else{
            LOG_ERROR("joint: %d in MIXED mode", jointIndex);
            if(ctrlmode2){
                ctrlmode2->setControlMode(jointIndex, VOCAB_CM_MIXED);
            } else {
                yError("ERROR: cannot do!");
            }
            break;
        }*/
    }
    case JointItem::Mixed:{
        //if(positionDirectEnabled){
            yInfo("joint: %d in MIXED mode", jointIndex);
            if(ctrlmode2){
                joint->resetTarget();
                ctrlmode2->setControlMode(jointIndex, VOCAB_CM_MIXED);
            } else {
                yError("ERROR: cannot do!");
            }
            break;
        /*}else{
            LOG_ERROR("joint: %d in VELOCITY mode", jointIndex);
            if(ctrlmode2){
                ctrlmode2->setVelocityMode(jointIndex);
            } else {
                LOG_ERROR("ERROR: cannot do!");
            }
            break;
        }*/

    }
    case JointItem::Velocity:{
        //if(positionDirectEnabled){
            yInfo("joint: %d in VELOCITY mode", jointIndex);
            if(ctrlmode2)
            {
                ctrlmode2->setControlMode(jointIndex, VOCAB_CM_VELOCITY);
                yInfo() << "Changing reference acceleration of joint " << jointIndex << " to 100000";
                iVel->setRefAcceleration(jointIndex, 100000);
            } else {
                yError("ERROR: cannot do!");
            }
            break;
//        } else {
//            LOG_ERROR("joint: %d in TORQUE mode", jointIndex);
//            if(ctrlmode2){
//                ctrlmode2->setTorqueMode(jointIndex);
//            } else {
//                LOG_ERROR("ERROR: cannot do!");
//            }
//            break;
//        }
    }

    case JointItem::Torque:{
        //if(positionDirectEnabled){
            yInfo("joint: %d in TORQUE mode", jointIndex);
            if(ctrlmode2){
                ctrlmode2->setControlMode(jointIndex, VOCAB_CM_TORQUE);
            } else {
                yError("ERROR: cannot do!");
            }
            break;
//        } else {
//            LOG_ERROR("joint: %d in OPENLOOP mode", jointIndex);
//            if(ctrlmode2){
//                ctrlmode2->setOpenLoopMode(jointIndex);
//            } else {
//                LOG_ERROR("ERROR: cannot do!");
//            }
//            break;
//        }

    }
    case JointItem::OpenLoop:{
        yInfo("joint: %d in OPENLOOP mode", jointIndex);
        if(ctrlmode2){
            ctrlmode2->setControlMode(jointIndex, VOCAB_CM_OPENLOOP);
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    default:
        break;
    }
}

void PartItem::resizeWidget(int w)
{
    int count = layout->count();


    int jointPerLineCount = (w - 20) / (MAX_WIDTH_JOINT + 10);
    if(jointPerLineCount > count){
        jointPerLineCount = count;
    }
    if(jointPerLineCount <= 0){
        return;
    }

    int extraSpace = (w - 20) - jointPerLineCount * (MAX_WIDTH_JOINT + 10);



    for(int i=0;i<count;i++){
        QWidget *widget = layout->itemAt(i)->widget();
        if(widget){
            widget->setMaximumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
            widget->setMinimumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
        }

    }
}

void PartItem::resizeEvent(QResizeEvent *event)
{
    if(!isVisible()){
        return;
    }

    resizeWidget(event->size().width());
}

void PartItem::changeEvent( QEvent *event )
{
    if(event->type() == QEvent::WindowStateChange ){
        qDebug() << "State Changed " << width();
        int count = layout->count();
        int jointPerLineCount = (width() - 20) / (MAX_WIDTH_JOINT + 10);

        if(jointPerLineCount > count){
            jointPerLineCount = count;
        }
        if(jointPerLineCount <= 0){
            return;
        }
        int extraSpace = (width() - 20) - jointPerLineCount * (MAX_WIDTH_JOINT + 10);


        qDebug() << jointPerLineCount;

        for(int i=0;i<count;i++){
            QWidget *widget = layout->itemAt(i)->widget();
            if(widget){
                widget->setMaximumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
                widget->setMinimumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
            }
        }
    }

}

void PartItem::calibrateAll()
{
    if(!remCalib)
    {
        QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return;
    }

    if(QMessageBox::question(this,"Question", QString("Do you want really to recalibrate the whole part?")) == QMessageBox::Yes)
    {
        if(!remCalib->calibrateWholePart() )
        {
            // provide better feedback to user by verifying if the calibrator device was set or not
            bool isCalib = false;
            remCalib->isCalibratorDevicePresent(&isCalib);
            if(!isCalib)
                QMessageBox::critical(this,"Calibration failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(partName));
            else
                QMessageBox::critical(this,"Calibration failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
        }
    }
}

bool PartItem::checkAndHomeAll()
{
    return homeAll();
}

bool PartItem::homeAll()
{
    bool ok = true;
    int NUMBER_OF_JOINTS;
    iPos->getAxes(&NUMBER_OF_JOINTS);

    QString zero = QString("%1_zero").arg(partName);

    if (!finder->isNull() && !finder->findGroup(zero.toLatin1().data()).isNull()){
        Bottle xtmp, ytmp;
        xtmp = finder->findGroup(zero.toLatin1().data()).findGroup("PositionZero");
        ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
        ytmp = finder->findGroup(zero.toLatin1().data()).findGroup("VelocityZero");
        ok = ok && (ytmp.size() == NUMBER_OF_JOINTS+1);
        if(ok){
            for (int jointIndex = 0; jointIndex < NUMBER_OF_JOINTS; jointIndex++){
                double positionZero = xtmp.get(jointIndex+1).asDouble();

                double velocityZero = ytmp.get(jointIndex+1).asDouble();

                iPos->setRefSpeed(jointIndex, velocityZero);
                iPos->positionMove(jointIndex, positionZero);
            }

        }else{
            QMessageBox::critical(this,"Error", QString("Check the number of entries in the group %1").arg(zero));
            ok = false;
        }
    }
    else
    {
        if(!remCalib)
        {
            QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
            return false;
        }

        QMessageBox::information(this,"Info", QString("Asking the yarprobotinterface to homing part %1 through the remoteCalibrator interface, since no custom zero group found in the supplied file.").arg(partName));
        ok = remCalib->homingWholePart();
        if(!ok)
        {
            // provide better feedback to user by verifying if the calibrator device was set or not
            bool isCalib = false;
            remCalib->isCalibratorDevicePresent(&isCalib);
            if(!isCalib)
                QMessageBox::critical(this,"Homing failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(partName));
            else
                QMessageBox::critical(this,"Homing failed", QString("The remote calibrator reported that something went wrong during the homing procedure"));
        }
    }
    return ok;
}

void PartItem::idleAll()
{

    int NUMBER_OF_JOINTS;
    iPos->getAxes(&NUMBER_OF_JOINTS);

    for (int joint=0; joint < NUMBER_OF_JOINTS; joint++){
        ctrlmode2->setControlMode(joint,VOCAB_CM_IDLE);
    }
}

bool PartItem::checkAndRunAllSeq()
{
    if(!sequenceWindow){
        return false;
    }

    return sequenceWindow->checkAndRun();


}

bool PartItem::checkAndRunTimeAllSeq()
{
    if(!sequenceWindow){
        return false;
    }

    return sequenceWindow->checkAndRunTime();


}

bool PartItem::checkAndCycleTimeAllSeq()
{
    if(!sequenceWindow){
        return false;
    }

    return sequenceWindow->checkAndCycleTimeSeq();
}

bool PartItem::checkAndCycleAllSeq()
{
    if(!sequenceWindow){
        return false;
    }

    return sequenceWindow->checkAndCycleSeq();
}

void PartItem::runAll()
{
    int NUMBER_OF_JOINTS;
    iPos->getAxes(&NUMBER_OF_JOINTS);

    for (int joint=0; joint < NUMBER_OF_JOINTS; joint++){
        //iencs->getEncoder(joint, &posJoint);
        ctrlmode2->setControlMode(joint,VOCAB_CM_POSITION);
        //gtk_range_set_value ((GtkRange *) (sliderAry[joint]), posJoint);
    }
}

void PartItem::loadSequence()
{
    openSequenceWindow();
    onOpenSequence();
}

void PartItem::saveSequence()
{
    openSequenceWindow();
    sequenceWindow->save();
}

void PartItem::openSequenceWindow()
{
    if(!sequenceWindow){
        sequenceWindow = new SequenceWindow(partName,layout->count());
        connect(sequenceWindow,SIGNAL(itemDoubleClicked(int)),this,SLOT(onSequenceWindowDoubleClicked(int)),Qt::DirectConnection);
        connect(this,SIGNAL(sendPartJointsValues(int,QList<double>,QList<double>)),sequenceWindow,SLOT(onReceiveValues(int,QList<double>,QList<double>)),Qt::DirectConnection);
        connect(sequenceWindow,SIGNAL(goToPosition(SequenceItem)),this,SLOT(onGo(SequenceItem)));
        connect(sequenceWindow,SIGNAL(runTime(QList<SequenceItem>)),this,SLOT(onSequenceRunTime(QList<SequenceItem>)),Qt::QueuedConnection);
        connect(sequenceWindow,SIGNAL(run(QList<SequenceItem>)),this,SLOT(onSequenceRun(QList<SequenceItem>)),Qt::QueuedConnection);
        connect(sequenceWindow,SIGNAL(saveSequence(QList<SequenceItem>)),this,SLOT(onSaveSequence(QList<SequenceItem>)),Qt::QueuedConnection);
        connect(sequenceWindow,SIGNAL(openSequence()),this,SLOT(onOpenSequence()));
        connect(sequenceWindow,SIGNAL(cycle(QList<SequenceItem>)),this,SLOT(onSequenceCycle(QList<SequenceItem>)),Qt::QueuedConnection);
        connect(sequenceWindow,SIGNAL(cycleTime(QList<SequenceItem>)),this,SLOT(onSequenceCycleTime(QList<SequenceItem>)),Qt::QueuedConnection);
        connect(sequenceWindow,SIGNAL(stopSequence()),this,SLOT(onStopSequence()),Qt::QueuedConnection);

        connect(this,SIGNAL(runTimeSequence()),sequenceWindow,SLOT(onRunTimeSequence()));
        connect(this,SIGNAL(cycleTimeSequence()),sequenceWindow,SLOT(onCycleTimeSequence()));
        connect(this,SIGNAL(cycleSequence()),sequenceWindow,SLOT(onCycleSequence()));
        connect(this,SIGNAL(stoppedSequence()),sequenceWindow,SLOT(onStoppedSequence()));
        connect(this,SIGNAL(setCurrentIndex(int)),sequenceWindow,SLOT(onSetCurrentSequenceIndex(int)));

        connect(this,SIGNAL(runTimeSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(cycleTimeSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(cycleSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(stoppedSequence()),this,SLOT(onSequenceStopped()));


    }

    if(!sequenceWindow->isVisible()){
        sequenceWindow->show();
    }else{
        sequenceWindow->setFocus();
        sequenceWindow->raise();
        sequenceWindow->setWindowState(Qt::WindowActive);
    }

}

bool PartItem::checkAndGo()
{
    if(!sequenceWindow){
        return false;
    }

    return sequenceWindow->checkAndGo();
}
void PartItem::stopSequence()
{
    cycleTimer.stop();
    runTimer.stop();
    runTimeTimer.stop();
    cycleTimeTimer.stop();
    stoppedSequence();
}

void PartItem::onStopSequence()
{
    stopSequence();
}

void PartItem::onOpenSequence()
{
    QString fileName = QFileDialog::getOpenFileName(sequenceWindow,QString("Load Sequence for part %1 As").arg(partName),QDir::homePath());

    QFileInfo fInfo(fileName);
    if(!fInfo.exists()){
        return;
    }

    QString desiredExtension = QString("pos%1").arg(partName);
    QString extension = fInfo.suffix();

    if(desiredExtension != extension){
        QMessageBox::critical(this,"Error Loading The Sequence",
                              QString("Wrong format (check estensions) of the file associated with: ").arg(partName));
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)){
        QString msg = QString("Error: Cannot read file %1: %2").arg(qPrintable(fileName))
                .arg(qPrintable(file.errorString()));
        QMessageBox::critical(this,"Error Loading The Sequence",msg);
        return;
    }

    QXmlStreamReader reader(&file);
    reader.readNext();

    int totPositions = 0;
    QString referencePart;

    QList<SequenceItem> sequenceItems;
    SequenceItem item;
    while(!reader.atEnd()){
        reader.readNext();

        if(reader.isStartElement()){
            if(reader.name().contains("Sequence_")){ //Sequence_
                QXmlStreamAttributes attributes = reader.attributes();
                totPositions = attributes.value("TotPositions").toInt();
                referencePart = attributes.value("ReferencePart").toString();
            }

            if(reader.name() == "Position"){ //Position
                QXmlStreamAttributes attributes = reader.attributes();
                int index = attributes.value("Index").toInt();
                double timing = attributes.value("Timing").toDouble();
                item.setTiming(timing);
                item.setSequenceNumber(index);
            }

            if(reader.name() == "JointPositions"){
                QXmlStreamAttributes attributes = reader.attributes();
                int count = attributes.value("Count").toInt();

                reader.readNext();
                for(int i=0; i<count;i++){
                    reader.readNext();
                    if(reader.name() == "PosValue"){    //PosValue
                        double pos = reader.readElementText().toDouble();
                        item.addPositionValue(pos);
                    }
                    reader.readNext();
                }

            }

            if(reader.name() == "JointVelocities"){
                QXmlStreamAttributes attributes = reader.attributes();
                int count = attributes.value("Count").toInt();

                reader.readNext();
                for(int i=0; i<count;i++){
                    reader.readNext();
                    if(reader.name() == "SpeedValue"){    //SpeedValue
                        double speed = reader.readElementText().toDouble();
                        item.addSpeedValue(speed);
                    }
                    reader.readNext();
                }

            }

        }

        if(reader.isEndElement()){
            if(reader.name() == "Position"){
                sequenceItems.append(item);
                item = SequenceItem();
            }
        }
    }

    file.close();

    if (reader.hasError())
    {
        QString msg = QString("Error: Failed to parse file %1: %2").arg(qPrintable(fileName)).arg(qPrintable(reader.errorString()));
        QMessageBox::critical(this,"Error Loading The Sequence",msg);
        return;
    } else if (file.error() != QFile::NoError) {
        QString msg = QString("Error: Cannot read file %1: %2").arg(qPrintable(fileName)).arg(qPrintable(file.errorString()));
        QMessageBox::critical(this,"Error Loading The Sequence",msg);
        return;
    }

    if(sequenceWindow){
        sequenceWindow->loadSequence(sequenceItems);
    }

}

void PartItem::onSaveSequence(QList<SequenceItem> values)
{
    QString fileName = QFileDialog::getSaveFileName(this,QString("Save Sequence for part %1 As").arg(partName),QDir::homePath());

    if(fileName.isEmpty()){
        return;
    }

    QFileInfo fInfo(fileName);
    QString completeFileName = QString("%1/%2.pos%3").arg(fInfo.absolutePath()).arg(fInfo.baseName()).arg(partName);

    //QFile file(completeFileName);
    yInfo("Saving file %s\n", completeFileName.toLatin1().data());

    QFile file(completeFileName);
    if(!file.open(QIODevice::WriteOnly)){
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement(QString("Sequence_pos%1").arg(partName));

    writer.writeAttribute("TotPositions", QString("%1").arg(values.count()));
    writer.writeAttribute("ReferencePart", partName);

    for(int i=0;i<values.count();i++){
        SequenceItem sequenceItem = values.at(i);
        writer.writeStartElement("Position");
        writer.writeAttribute("Index",QString("%1").arg(sequenceItem.getSequenceNumber()));
        writer.writeAttribute("Timing",QString("%L1").arg(sequenceItem.getTiming(),0,'f',2));

        writer.writeStartElement("JointPositions");
        writer.writeAttribute("Count",QString("%1").arg(sequenceItem.getPositions().count()));
        for(int j=0;j<sequenceItem.getPositions().count(); j++){
            QString s = QString("%L1").arg(sequenceItem.getPositions().at(j),0,'f',2);
            writer.writeTextElement("PosValue",s);
        }
        writer.writeEndElement();

        writer.writeStartElement("JointVelocities");
        writer.writeAttribute("Count",QString("%1").arg(sequenceItem.getSpeeds().count()));
        for(int j=0;j<sequenceItem.getSpeeds().count(); j++){
            QString s = QString("%L1").arg(sequenceItem.getSpeeds().at(j),0,'f',2);
            writer.writeTextElement("SpeedValue",s);
        }
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
    LOG_ERROR("File saved and closed\n");


//    if(file.open(QIODevice::WriteOnly)){
//        for(int i=0;i<values.count();i++){
//            SequenceItem sequenceItem = values.at(i);
//            QString s = QString("[POSITION%1] \n").arg(sequenceItem.getSequenceNumber());
//            file.write(s.toLatin1().data(),s.length());

//            s = QString("jointPositions ");
//            file.write(s.toLatin1().data(),s.length());

//            for(int j=0;j<sequenceItem.getPositions().count(); j++){
//                s = QString("%L1 ").arg(sequenceItem.getPositions().at(j),0,'f',2);
//                file.write(s.toLatin1().data(),s.length());
//            }

//            s = QString("\njointVelocities ");
//            file.write(s.toLatin1().data(),s.length());
//            for(int j=0;j<sequenceItem.getSpeeds().count(); j++){
//                s = QString("%L1 ").arg(sequenceItem.getSpeeds().at(j),0,'f',2);
//                file.write(s.toLatin1().data(),s.length());
//            }

//            s = QString("\ntiming ");
//            file.write(s.toLatin1().data(),s.length());
//            s = QString("%L1 \n").arg(sequenceItem.getTiming(),0,'f',2);
//            file.write(s.toLatin1().data(),s.length());
//        }
//        file.flush();
//        file.close();

//        LOG_ERROR("File saved and closed\n");
//    }

}

void PartItem::onSequenceCycleTime(QList<SequenceItem> values)
{
    // Remove items after the first timing with value < 0
    if(cycleTimeTimer.isActive() || cycleTimer.isActive() || runTimeTimer.isActive() || runTimer.isActive()){
        return;
    }

    cycleTimeValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            cycleTimeValues.append(it);
        }
    }

    SequenceItem vals;
    if(cycleTimeValues.count() > 0){
        vals = cycleTimeValues.takeFirst();

        cycleTimeValues.append(vals);

//        double *cmdPositions = new double[vals.getPositions().count()];
//        double *cmdVelocities = new double[vals.getSpeeds().count()];

//        for(int i=0;i<vals.getPositions().count();i++){
//            cmdPositions[i] = vals.getPositions().at(i);
//            cmdVelocities[i] = vals.getSpeeds().at(i);
//            qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
//        }

        //fixedTimeMove(vals.getPositions());

        setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
//        iPos->setRefSpeeds(cmdVelocities);
//        iPos->positionMove(cmdPositions);
        cycleTimeTimer.start(vals.getTiming() * 1000);

        cycleTimeSequence();
    }
}

void PartItem::onCycleTimeTimerTimeout()
{
    if(cycleTimeValues.count() > 0){
        SequenceItem vals = cycleTimeValues.takeFirst();

        cycleTimeValues.append(vals);

//        double *cmdPositions = new double[vals.getPositions().count()];
//        double *cmdVelocities = new double[vals.getSpeeds().count()];

//        for(int i=0;i<vals.getPositions().count();i++){
//            cmdPositions[i] = vals.getPositions().at(i);
//            cmdVelocities[i] = vals.getSpeeds().at(i);
//            qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
//        }

        //fixedTimeMove(vals.getPositions());
        setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
//        iPos->setRefSpeeds(cmdVelocities);
//        iPos->positionMove(cmdPositions);
        cycleTimeTimer.start(vals.getTiming() * 1000);
    }
}

void PartItem::onSequenceCycle(QList<SequenceItem> values)
{
    // Remove items after the first timing with value < 0
    if(cycleTimeTimer.isActive() || cycleTimer.isActive() || runTimeTimer.isActive() || runTimer.isActive()){
        return;
    }

    cycleValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            cycleValues.append(it);
        }
    }

    SequenceItem vals;
    if(cycleValues.count() > 0){
        vals = cycleValues.takeFirst();

        cycleValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        iPos->setRefSpeeds(cmdVelocities);
        iPos->positionMove(cmdPositions);
        cycleTimer.start(vals.getTiming() * 1000);

        cycleSequence();
    }
}

void PartItem::onCycleTimerTimeout()
{
    if(cycleValues.count() > 0){
        SequenceItem vals = cycleValues.takeFirst();

        cycleValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
        }

        setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        iPos->setRefSpeeds(cmdVelocities);
        iPos->positionMove(cmdPositions);


        cycleTimer.start(vals.getTiming() * 1000);
    }
}

void PartItem::onSequenceRun(QList<SequenceItem> values)
{
    // Remove items after the first timing with value < 0
    if(cycleTimeTimer.isActive() || cycleTimer.isActive() || runTimeTimer.isActive() || runTimer.isActive()){
        return;
    }

    runValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            runValues.append(it);
        }
    }

    SequenceItem vals;
    if(runValues.count() > 0){
        vals = runValues.takeFirst();

        runValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        iPos->setRefSpeeds(cmdVelocities);
        iPos->positionMove(cmdPositions);
        runTimer.start(vals.getTiming() * 1000);

        runSequence();
    }
}
void PartItem::onRunTimeout()
{
    if(runValues.count() > 0){
        SequenceItem vals = runValues.takeFirst();
        if(vals.getTiming() < 0){
            stoppedSequence();
            return;
        }
        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        iPos->setRefSpeeds(cmdVelocities);
        iPos->positionMove(cmdPositions);


        runTimer.start(vals.getTiming() * 1000);
    }else{
        stoppedSequence();
    }
}



void PartItem::onSequenceRunTime(QList<SequenceItem> values)
{
    if(cycleTimeTimer.isActive() || cycleTimer.isActive() || runTimeTimer.isActive() || runTimer.isActive()){
        return;
    }

    runTimeValues.clear();

    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            runTimeValues.append(it);
        }
    }

    SequenceItem vals;
    if(runTimeValues.count() > 0){
        vals = runTimeValues.takeFirst();

        setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        runTimeTimer.start(vals.getTiming() * 1000);

        runTimeSequence();
    }

}

void PartItem::onRunTimerTimeout()
{
    if(runTimeValues.count() > 0){
        SequenceItem vals = runTimeValues.takeFirst();
        if(vals.getTiming() < 0){
            stoppedSequence();
            return;
        }
        setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        runTimeTimer.start(vals.getTiming() * 1000);
    }else{
        stoppedSequence();
    }
}

void PartItem::fixedTimeMove(SequenceItem sequence)
{
    int NUM_JOINTS;
    iPos->getAxes(&NUM_JOINTS);
    double *cmdPositions = new double[NUM_JOINTS];
    double *cmdVelocities = new double[NUM_JOINTS];
    double *startPositions = new double[NUM_JOINTS];
    double cmdTime = sequence.getTiming();

    while (!iencs->getEncoders(startPositions)){
        Time::delay(0.001);
    }


    for(int k=0; k<NUM_JOINTS; k++){
        cmdVelocities[k] = 0;
        cmdPositions[k] = sequence.getPositions().at(k);

        if (fabs(startPositions[k] - cmdPositions[k]) > 0.01){
            cmdVelocities[k] = fabs(startPositions[k] - cmdPositions[k])/cmdTime;
        } else {
            cmdVelocities[k] = 1.0;
        }
    }

  iPos->setRefSpeeds(cmdVelocities);
  iPos->positionMove(cmdPositions);

  sequence_port_stamp.update();
  sequence_port.setEnvelope(sequence_port_stamp);
  Vector v(NUM_JOINTS,cmdPositions);
  sequence_port.write(v);
  delete[] cmdVelocities;
  delete[] startPositions;
  delete[] cmdPositions;
  return;
}

void PartItem::onGo(SequenceItem sequenceItem)
{
    if(sequenceItem.getPositions().isEmpty() || sequenceItem.getSpeeds().isEmpty()){
        QMessageBox::critical(this,"Error", "Select an entry in the table before performing a movement");
        return;
    }

    if(sequenceItem.getTiming() < 0){
        return;
    }

    int NUMBER_OF_JOINTS;
    iPos->getAxes(&NUMBER_OF_JOINTS);

    for(int i=0;i<NUMBER_OF_JOINTS;i++){
        iPos->setRefSpeed(i,sequenceItem.getSpeeds().at(i));
        iPos->positionMove(i,sequenceItem.getPositions().at(i));
    }
}

void PartItem::onSequenceActivated()
{
    for(int i=0;i<layout->count();i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if(joint){
            joint->sequenceActivated();
        }
    }

    sequenceActivated();

}

void PartItem::onSequenceStopped()
{
    for(int i=0;i<layout->count();i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if(joint){
            joint->sequenceStopped();
        }
    }
    sequenceStopped();
}

void PartItem::onSequenceWindowDoubleClicked(int sequenceNum)
{
    QList<double>values;
    QList<double>speeds;
    for(int i=0;i<layout->count();i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        values.append(joint->getTrajectoryPositionValue());
        speeds.append(joint->getTrajectoryVelocityValue());
    }

    sendPartJointsValues(sequenceNum,values,speeds);
}

void PartItem::onEnableControlVelocity(bool control)
{
    for(int i=0;i<layout->count();i++){
        JointItem *joint  = (JointItem*)layout->itemAt(i)->widget();
        joint->enableControlVelocity(control);
    }
}

void PartItem::onEnableControlMixed(bool control)
{
    for (int i = 0; i<layout->count(); i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        joint->enableControlMixed(control);
    }
}

void PartItem::onEnableControlPositionDirect(bool control)
{
    for (int i = 0; i<layout->count(); i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        joint->enableControlPositionDirect(control);
    }
}

void PartItem::onEnableControlOpenloop(bool control)
{
    for (int i = 0; i<layout->count(); i++){
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        joint->enableControlOpenloop(control);
    }
}

void PartItem::onViewSpeedValues(bool view)
{
    for(int i=0;i<layout->count();i++){
        JointItem *joint  = (JointItem*)layout->itemAt(i)->widget();
        joint->setSpeedVisible(view);
    }
}

void PartItem::onViewPositionTarget(bool ena)
{
    for (int i = 0; i<layout->count(); i++)
    {
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if (ena)
        {
            joint->viewPositionTarget(ena);
        }
        else
        {
            joint->viewPositionTarget(ena);
        }
    }
}

void PartItem::onSetPosSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<layout->count(); i++)
    {
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if (mode==0)
        {
            joint->enablePositionSliderDoubleAuto();
        }
        else if(mode ==1)
        {
            joint->enablePositionSliderDoubleValue(step);
        }
        else if (mode == 2)
        {
            joint->enablePositionSliderDoubleValue(1.0);
        }
        else 
        {
            joint->disablePositionSliderDouble();
        }
    }
}
void PartItem::onSetVelSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<layout->count(); i++)
    {
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if (mode == 0)
        {
            joint->enableVelocitySliderDoubleAuto();
            joint->enableTrajectoryVelocitySliderDoubleAuto();
        }
        else if (mode == 1)
        {
            joint->enableVelocitySliderDoubleValue(step);
            joint->enableTrajectoryVelocitySliderDoubleValue(step);
        }
        else if (mode == 2)
        {
            joint->enableVelocitySliderDoubleValue(1.0);
            joint->enableTrajectoryVelocitySliderDoubleValue(1.0);
        }
        else 
        {
            joint->disableVelocitySliderDouble();
            joint->disableTrajectoryVelocitySliderDouble();
        }
    }
}
void PartItem::onSetTrqSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<layout->count(); i++)
    {
        JointItem *joint = (JointItem*)layout->itemAt(i)->widget();
        if (mode == 0)
        {
            joint->enableTorqueSliderDoubleAuto();
        }
        else if(mode == 1)
        {
            joint->enableTorqueSliderDoubleValue(step);
        }
        else if(mode == 2)
        {
            joint->enableTorqueSliderDoubleValue(1.0);
        }
        else 
        {
            joint->disableTorqueSliderDouble();
        }
    }
}
QTreeWidgetItem *PartItem::getTreeWidgetModeNode()
{
    return node;
}

void PartItem::setTreeWidgetModeNode(QTreeWidgetItem *node)
{
    this->node = node;
}

QList<int> PartItem::getPartMode()
{
    QList <int> modes;

    for (int k = 0; k < layout->count(); k++){
        switch (controlModes[k])
        {
        case VOCAB_CM_IDLE:
            modes.append(JointItem::Idle);
            break;
        case VOCAB_CM_POSITION:
            modes.append(JointItem::Position);
            break;
        case VOCAB_CM_POSITION_DIRECT:
            modes.append(JointItem::PositionDirect);
            break;
        case VOCAB_CM_MIXED:
            modes.append(JointItem::Mixed);
            break;
        case VOCAB_CM_VELOCITY:
            modes.append(JointItem::Velocity);
            break;
        case VOCAB_CM_TORQUE:
            modes.append(JointItem::Torque);
            break;
        case VOCAB_CM_OPENLOOP:
            modes.append(JointItem::OpenLoop);
            break;
        case VOCAB_CM_HW_FAULT:
            modes.append(JointItem::HwFault);
            break;
        case VOCAB_CM_CALIBRATING:
            modes.append(JointItem::Calibrating);
            break;
        case VOCAB_CM_CALIB_DONE:
            modes.append(JointItem::CalibDone);
            break;
        case VOCAB_CM_NOT_CONFIGURED:
            modes.append(JointItem::NotConfigured);
            break;
        case VOCAB_CM_CONFIGURED:
            modes.append(JointItem::Configured);
            break;
        default:
        case VOCAB_CM_UNKNOWN:
            modes.append(JointItem::Unknown);
            break;
        }
    }

    return modes;
}

void PartItem::updateControlMode()
{
    bool ret=ctrlmode2->getControlModes(controlModes);


    if(ret==false){
        LOG_ERROR("ictrl->getControlMode failed\n" );
    }
}

bool PartItem::updatePart()
{
    static double refTrajectorySpeeds[MAX_NUMBER_OF_JOINTS];
    static double refTrajectoryPositions[MAX_NUMBER_OF_JOINTS];
    static double refTorques[MAX_NUMBER_OF_JOINTS];
    static double refVelocitySpeeds[MAX_NUMBER_OF_JOINTS];
    static double torques[MAX_NUMBER_OF_JOINTS];
    static double positions[MAX_NUMBER_OF_JOINTS];
    static double speeds[MAX_NUMBER_OF_JOINTS];
    static bool   done[MAX_NUMBER_OF_JOINTS];
    //static int controlModes[MAX_NUMBER_OF_JOINTS];
    static yarp::dev::InteractionModeEnum interactionModes[MAX_NUMBER_OF_JOINTS];

    bool ret = false;
    int number_of_joints=0;
    iPos->getAxes(&number_of_joints);
    if (slow_k >= number_of_joints-1) slow_k = 0;
    else slow_k++;

    if (number_of_joints == 0)
    {
        LOG_ERROR("Lost connection with the robot. You should save and restart.\n" );
        Time::delay(0.1);

        for(int i=0;i<layout->count();i++){
            JointItem *joint  = (JointItem*)layout->itemAt(i)->widget();
            joint->setJointState(JointItem::Disconnected);
        }
        return false;
    }

    // *** update measured encoders, velocity, torques ***
    if (!iencs->getEncoders(positions))   { yWarning("Unable to update encoders"); return false; }
    if (!iTrq->getTorques(torques))       { yWarning("Unable to update torques"); }
    if (!iencs->getEncoderSpeeds(speeds)) { yWarning("Unable to update speeds"); }
    
    // *** update checkMotionDone, refTorque, refTrajectorySpeed, refSpeed ***
    // (only one at a time in order to save badwidth)
    bool b_motdone     = iPos->checkMotionDone(slow_k, &done[slow_k]); //using k to save bandwidth
    bool b_refTrq      = iTrq->getRefTorque(slow_k, &refTorques[slow_k]); //using k to save bandwidth
    bool b_refPosSpeed = iPos->getRefSpeed(slow_k, &refTrajectorySpeeds[slow_k]); //using k to save bandwidth
    bool b_refVel      = iVel->getRefVelocity(slow_k,&refVelocitySpeeds[slow_k]); //this interface is missing!
    bool b_refPos      = iPos->getTargetPosition(slow_k, &refTrajectoryPositions[slow_k]);

    if (!b_refPos)
    {
        yError() << "Missing Implementation of getTargetPosition()";
    }
    if (!b_refVel)
    {
        yError() << "Missing Implementation of getRefVelocity()";
    }
    if (!b_refPosSpeed)
    {
        yError() << "Missing Implementation of getRefSpeed()";
    }
    if (!b_refTrq)
    {
        yError() << "Missing Implementation of getRefTorque()";
    }
    if (!b_motdone)
    {
        yError() << "Missing Implementation of checkMotionDone()";
    }

    // *** update the widget every cycle ***
    for (int jk = 0; jk < number_of_joints; jk++)
    {
        JointItem *joint = (JointItem*)layout->itemAt(jk)->widget();
        if (1) { joint->setPosition(positions[jk]); }
        else {}
        if (1) { joint->setTorque(torques[jk]); }
        else {}
        if (1) { joint->setSpeed(speeds[jk]); }
        else {}
    }
    
    // *** update the widget NOT every cycle ***
    {
        JointItem *joint_slow_k = (JointItem*)layout->itemAt(slow_k)->widget();
        if (b_refTrq) { joint_slow_k->setRefTorque(refTorques[slow_k]); }
        else {}
        if (b_refPosSpeed) { joint_slow_k->setRefTrajectorySpeed(refTrajectorySpeeds[slow_k]); }
        else {}
        if (b_refPos) { joint_slow_k->setRefTrajectoryPosition(refTrajectoryPositions[slow_k]); }
        else {}
        if (b_refVel) { joint_slow_k->setRefVelocitySpeed(refVelocitySpeeds[slow_k]); }
        else {}
        if (b_motdone) { joint_slow_k->updateMotionDone(done[slow_k]); }
        else {}
    }


    // *** update the controlMode, interactionMode ***
    // this is already done by updateControlMode() (because it also needs to update the tree, not only the single joint widget)
    //    ret=ctrlmode2->getControlModes(controlModes);
    //    if(ret==false){
    //        LOG_ERROR("ictrl->getControlMode failed\n" );
    //    }
    ret=iinteract->getInteractionModes(interactionModes);
    if(ret==false){
        LOG_ERROR("iint->getInteractionlMode failed\n" );
    }

    for (int k = 0; k < number_of_joints; k++)
    {
        JointItem *joint  = (JointItem*)layout->itemAt(k)->widget();
        switch (controlModes[k])
        {
            case VOCAB_CM_IDLE:
                joint->setJointState(JointItem::Idle);
                break;
            case VOCAB_CM_POSITION:
                joint->setJointState(JointItem::Position);
                break;
            case VOCAB_CM_POSITION_DIRECT:
                joint->setJointState(JointItem::PositionDirect);
                break;
            case VOCAB_CM_MIXED:
                joint->setJointState(JointItem::Mixed);
                break;
            case VOCAB_CM_VELOCITY:
                joint->setJointState(JointItem::Velocity);
                break;
            case VOCAB_CM_TORQUE:
                joint->setJointState(JointItem::Torque);
                break;
            case VOCAB_CM_OPENLOOP:
            {
                joint->setJointState(JointItem::OpenLoop);
                double openLoopValue = 0;
                iOpl->getRefOutput(k, &openLoopValue);
                joint->setOpenLoop(openLoopValue);
                break;
            }
            case VOCAB_CM_HW_FAULT:
                joint->setJointState(JointItem::HwFault);
                break;
            case VOCAB_CM_CALIBRATING:
                joint->setJointState(JointItem::Calibrating);
                break;
            case VOCAB_CM_CALIB_DONE:
                joint->setJointState(JointItem::CalibDone);
                break;
            case VOCAB_CM_NOT_CONFIGURED:
                joint->setJointState(JointItem::NotConfigured);
                break;
            case VOCAB_CM_CONFIGURED:
                joint->setJointState(JointItem::Configured);
                break;
            default:
            case VOCAB_CM_UNKNOWN:
                joint->setJointState(JointItem::Unknown);
                break;
        }
        switch (interactionModes[k])
        {
            case VOCAB_IM_STIFF:
                joint->setJointInteraction(JointItem::Stiff);
                break;
            case VOCAB_IM_COMPLIANT:
                joint->setJointInteraction(JointItem::Compliant);
                break;
            default:
            case VOCAB_IM_UNKNOWN:
                //joint->setJointInteraction(JointItem::Stiff); TODO
                break;
        }
    }
    return true;
}

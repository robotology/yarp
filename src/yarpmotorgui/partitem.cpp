/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "partitem.h"
#include "log.h"

#include <yarp/os/LogStream.h>
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
#include <cstdio>

PartItem::PartItem(QString robotName, int id, QString partName, ResourceFinder& _finder,
                   bool debug_param_enabled,
                   bool speedview_param_enabled,
                   bool enable_calib_all, QWidget *parent) :
    QWidget(parent),
    m_node(nullptr),
    m_sequenceWindow(nullptr),
    m_partId(id),
    m_mixedEnabled(false),
    m_positionDirectEnabled(false),
    m_pwmEnabled(false),
    m_currentEnabled(false),
    m_currentPidDlg(nullptr),
    m_controlModes(nullptr),
    m_refTrajectorySpeeds(nullptr),
    m_refTrajectoryPositions(nullptr),
    m_refTorques(nullptr),
    m_refVelocitySpeeds(nullptr),
    m_torques(nullptr),
    m_positions(nullptr),
    m_speeds(nullptr),
    m_currents(nullptr),
    m_motorPositions(nullptr),
    m_dutyCycles(nullptr),
    m_done(nullptr),
    m_part_speedVisible(false),
    m_part_motorPositionVisible(false),
    m_part_dutyVisible(false),
    m_part_currentVisible(false),
    m_interactionModes(nullptr),
    m_finder(&_finder),
    m_iMot(nullptr),
    m_iinfo(nullptr),
    m_slow_k(0)
{
    m_layout = new FlowLayout();
    setLayout(m_layout);

    //PolyDriver *cartesiandd[MAX_NUMBER_ACTIVATED];

    if (robotName.at(0) == '/') robotName.remove(0, 1);
    if (partName.at(0) == '/')  partName.remove(0, 1);
    m_robotPartPort = QString("/%1/%2").arg(robotName).arg(partName);
    m_partName = partName;
    m_robotName = robotName;

    //checking existence of the port
    int ind = 0;
    QString portLocalName = QString("/yarpmotorgui%1/%2").arg(ind).arg(m_robotPartPort);


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

        portLocalName = QString("/yarpmotorgui%1/%2").arg(ind).arg(m_robotPartPort);

        nameToCheck = portLocalName;
        nameToCheck += "/rpc:o";
        // adr=nic.queryName(nameToCheck.c_str());
        adr=Network::queryName(nameToCheck.toLatin1().data());
    }

    m_interfaceError = false;

    // Initializing the polydriver options and instantiating the polydrivers
    m_partOptions.put("local", portLocalName.toLatin1().data());
    m_partOptions.put("device", "remote_controlboard");
    m_partOptions.put("remote", m_robotPartPort.toLatin1().data());
    m_partOptions.put("carrier", "udp");

    m_partsdd = new PolyDriver();

    // Opening the drivers
    m_interfaceError = !openPolyDrivers();
    if (m_interfaceError == true)
    {
        yError("Opening PolyDriver for part %s failed...", m_robotPartPort.toLatin1().data());
        QMessageBox::critical(nullptr, "Error opening a device", QString("Error while opening device for part ").append(m_robotPartPort.toLatin1().data()));
    }

    /*********************************************************************/
    /**************** PartMover Content **********************************/

    if (!m_finder->isNull()){
        yDebug("Setting a valid finder \n");
    }

    QString sequence_portname = QString("/yarpmotorgui/%1/sequence:o").arg(partName);
    m_sequence_port.open(sequence_portname.toLatin1().data());

    initInterfaces();
    openInterfaces();

    if (m_interfaceError == false)
    {
        int i = 0;
        std::string jointname;
        int number_of_joints;
        m_iPos->getAxes(&number_of_joints);

        m_controlModes = new int[number_of_joints]; //for (i = 0; i < number_of_joints; i++) m_controlModes = 0;
        m_refTrajectorySpeeds = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_refTrajectorySpeeds[i] = std::nan("");
        m_refTrajectoryPositions = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_refTrajectoryPositions[i] = std::nan("");
        m_refTorques = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_refTorques[i] = std::nan("");
        m_refVelocitySpeeds = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_refVelocitySpeeds[i] = std::nan("");
        m_torques = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_torques[i] = std::nan("");
        m_positions = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_positions[i] = std::nan("");
        m_speeds = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_speeds[i] = std::nan("");
        m_currents = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_currents[i] = std::nan("");
        m_motorPositions = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_motorPositions[i] = std::nan("");
        m_dutyCycles = new double[number_of_joints]; for (i = 0; i < number_of_joints; i++) m_dutyCycles[i] = std::nan("");
        m_done = new bool[number_of_joints];
        m_interactionModes = new yarp::dev::InteractionModeEnum[number_of_joints];

        bool ret = false;
        SystemClock::delaySystem(0.050);
        do {
            ret = m_iencs->getEncoders(m_positions);
            if (!ret) {
                yError("%s iencs->getEncoders() failed, retrying...\n", partName.toLatin1().data());
                SystemClock::delaySystem(0.050);
            }
        } while (!ret);

        yInfo("%s iencs->getEncoders() ok!\n", partName.toLatin1().data());

        double min_pos = 0;
        double max_pos = 100;
        double min_vel = 0;
        double max_vel = 100;
        double min_cur = -2.0;
        double max_cur = +2.0;
        for (int k = 0; k<number_of_joints; k++)
        {
            bool bpl = m_iLim->getLimits(k, &min_pos, &max_pos);
            bool bvl = m_iLim->getVelLimits(k, &min_vel, &max_vel);
            bool bcr = m_iCur->getCurrentRange(k, &min_cur, &max_cur);
            if (bpl == false)
            {
                yError() << "Error while getting position limits, part " << partName.toStdString() << " joint " << k;
            }
            if (bvl == false || (min_vel == 0 && max_vel == 0))
            {
                yError() << "Error while getting velocity limits, part " << partName.toStdString() << " joint " << k;
            }
            if (bcr == false || (min_cur == 0 && max_cur == 0))
            {
                yError() << "Error while getting current range, part " << partName.toStdString() << " joint " << k;
            }

            QSettings settings("YARP", "yarpmotorgui");
            double max_slider_vel = settings.value("velocity_slider_limit", 100.0).toDouble();
            if (max_vel > max_slider_vel) max_vel = max_slider_vel;

            m_iinfo->getAxisName(k, jointname);
            yarp::dev::JointTypeEnum jtype = yarp::dev::VOCAB_JOINTTYPE_REVOLUTE;
            m_iinfo->getJointType(k, jtype);

            Pid myPid(0,0,0,0,0,0);
            yarp::os::SystemClock::delaySystem(0.005);
            m_iPid->getPid(VOCAB_PIDTYPE_POSITION, k, &myPid);

            auto* joint = new JointItem(k);
            joint->setJointName(jointname.c_str());
            joint->setPWMRange(-100.0, 100.0);
            joint->setCurrentRange(min_cur, max_cur);
            m_layout->addWidget(joint);
            joint->setPositionRange(min_pos, max_pos);
            joint->setVelocityRange(min_vel, max_vel);
            joint->setTrajectoryVelocityRange(max_vel);
            joint->setTorqueRange(5.0);
            joint->setUnits(jtype);
            joint->enableControlPositionDirect(m_positionDirectEnabled);
            joint->enableControlMixed(m_mixedEnabled);
            joint->enableControlPWM(m_pwmEnabled);
            joint->enableControlCurrent(m_currentEnabled);

            int val_pos_choice = settings.value("val_pos_choice", 0).toInt();
            int val_trq_choice = settings.value("val_trq_choice", 0).toInt();
            int val_vel_choice = settings.value("val_vel_choice", 0).toInt();
            double val_pos_custom_step = settings.value("val_pos_custom_step", 1.0).toDouble();
            double val_trq_custom_step = settings.value("val_trq_custom_step", 1.0).toDouble();
            double val_vel_custom_step = settings.value("val_vel_custom_step", 1.0).toDouble();
            onSetPosSliderOptionPI(val_pos_choice, val_pos_custom_step);
            onSetVelSliderOptionPI(val_vel_choice, val_vel_custom_step);
            onSetTrqSliderOptionPI(val_trq_choice, val_trq_custom_step);
            onSetCurSliderOptionPI(val_trq_choice, val_trq_custom_step);

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
            connect(joint, SIGNAL(sliderPWMCommand(double, int)), this, SLOT(onSliderPWMCommand(double, int)));
            connect(joint, SIGNAL(sliderCurrentCommand(double, int)), this, SLOT(onSliderCurrentCommand(double, int)));
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

    m_cycleTimer.setSingleShot(true);
    m_cycleTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_cycleTimer, SIGNAL(timeout()), this, SLOT(onCycleTimerTimeout()), Qt::QueuedConnection);

    m_cycleTimeTimer.setSingleShot(true);
    m_cycleTimeTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_cycleTimeTimer, SIGNAL(timeout()), this, SLOT(onCycleTimeTimerTimeout()), Qt::QueuedConnection);


    m_runTimeTimer.setSingleShot(true);
    m_runTimeTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_runTimeTimer, SIGNAL(timeout()), this, SLOT(onRunTimerTimeout()), Qt::QueuedConnection);

    m_runTimer.setSingleShot(true);
    m_runTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_runTimer, SIGNAL(timeout()), this, SLOT(onRunTimeout()), Qt::QueuedConnection);
}

PartItem::~PartItem()
{
    disconnect(&m_runTimer, SIGNAL(timeout()), this, SLOT(onRunTimeout()));
    m_runTimer.stop();

    disconnect(&m_runTimeTimer, SIGNAL(timeout()), this, SLOT(onRunTimerTimeout()));
    m_runTimeTimer.stop();

    disconnect(&m_cycleTimer, SIGNAL(timeout()), this, SLOT(onCycleTimerTimeout()));
    m_cycleTimer.stop();

    disconnect(&m_cycleTimeTimer, SIGNAL(timeout()), this, SLOT(onCycleTimeTimerTimeout()));
    m_cycleTimeTimer.stop();

    if (m_sequenceWindow){
        m_sequenceWindow->hide();
        delete m_sequenceWindow;
    }

    for (int i = 0; i<m_layout->count(); i++){
        auto* joint = (JointItem *)m_layout->itemAt(i)->widget();
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

    if (m_partsdd){
        m_partsdd->close();
    }

    if (m_controlModes) { delete[] m_controlModes; m_controlModes = nullptr; }
    if (m_refTrajectorySpeeds) { delete[] m_refTrajectorySpeeds; m_refTrajectorySpeeds = nullptr; }
    if (m_refTrajectoryPositions) { delete[] m_refTrajectoryPositions; m_refTrajectoryPositions = nullptr; }
    if (m_refTorques) { delete[] m_refTorques; m_refTorques = nullptr; }
    if (m_refVelocitySpeeds) { delete[] m_refVelocitySpeeds; m_refVelocitySpeeds = nullptr; }
    if (m_torques) { delete[] m_torques; m_torques = nullptr; }
    if (m_positions) { delete[] m_positions; m_positions = nullptr; }
    if (m_speeds) { delete[] m_speeds; m_speeds = nullptr; }
    if (m_currents) { delete[] m_currents; m_currents = nullptr; }
    if (m_motorPositions) { delete[] m_motorPositions; m_motorPositions = nullptr; }
    if (m_dutyCycles) { delete[] m_dutyCycles; m_dutyCycles = nullptr; }
    if (m_done) { delete[] m_done; m_done = nullptr; }
}

bool PartItem::openPolyDrivers()
{
    m_partsdd->open(m_partOptions);
    if (!m_partsdd->isValid()) {
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
    m_iPos = nullptr;
    m_iVel = nullptr;
    m_iVar = nullptr;
    m_iDir = nullptr;
    m_iencs = nullptr;
    m_iAmp = nullptr;
    m_iPid = nullptr;
    m_iCur = nullptr;
    m_iPWM = nullptr;
    m_iTrq = nullptr;
    m_iImp = nullptr;
    m_iLim = nullptr;
    m_ical = nullptr;
    m_ictrlmode = nullptr;
    m_iinteract = nullptr;
    m_iremCalib = nullptr;
}

bool PartItem::openInterfaces()
{
    yDebug("Opening interfaces...");
    bool ok = false;

    if (m_partsdd->isValid()) {
        ok = m_partsdd->view(m_iPid);
        if(!ok){
            yError("...iPid was not ok...");
        }
        ok &= m_partsdd->view(m_iAmp);
        if(!ok){
            yError("...iAmp was not ok...");
        }
        ok &= m_partsdd->view(m_iPos);
        if(!ok){
            yError("...iPos was not ok...");
        }
        ok &= m_partsdd->view(m_iDir);
        if(!ok){
            yError("...posDirect was not ok...");
        }
        ok &= m_partsdd->view(m_iVel);
        if(!ok){
            yError("...iVel was not ok...");
        }
        ok &= m_partsdd->view(m_iLim);
        if(!ok){
            yError("...iLim was not ok...");
        }
        ok &= m_partsdd->view(m_iencs);
        if(!ok){
            yError("...enc was not ok...");
        }
        ok &= m_partsdd->view(m_ical);
        if(!ok){
            yError("...cal was not ok...");
        }
        ok &= m_partsdd->view(m_iTrq);
        if(!ok){
            yError("...trq was not ok...");
        }
        ok = m_partsdd->view(m_iPWM);
        if(!ok){
            yError("...opl was not ok...");
        }
        ok &= m_partsdd->view(m_iImp);
        if(!ok){
            yError("...imp was not ok...");
        }
        ok &= m_partsdd->view(m_ictrlmode);
        if(!ok){
            yError("...ctrlmode2 was not ok...");
        }
        ok &= m_partsdd->view(m_iinteract);
        if(!ok){
            yError("...iinteract was not ok...");
        }

        //optional interfaces
        if (!m_partsdd->view(m_iVar))
        {
            yError("...iVar was not ok...");
        }

        if (!m_partsdd->view(m_iMot))
        {
            yError("...iMot was not ok...");
        }

        if (!m_partsdd->view(m_iremCalib))
        {
            yError("...remCalib was not ok...");
        }

        if (!m_partsdd->view(m_iinfo))
        {
            yError("...axisInfo was not ok...");
        }

        if (!m_partsdd->view(m_iCur))
        {
            yError("...iCur was not ok...");
        }

        if (!ok) {
            yError("Error while acquiring interfaces!");
            QMessageBox::critical(nullptr,"Problems acquiring interfaces.","Check if interface is running");
            m_interfaceError = true;
        }
    }
    else
    {
        yError("Device driver was not valid!");
        m_interfaceError = true;
    }

    return !m_interfaceError;
}

bool PartItem::getInterfaceError()
{
    return m_interfaceError;
}

QString PartItem::getPartName()
{
    return m_partName;
}

void PartItem::onSliderPWMCommand(double torqueVal, int index)
{
    m_iPWM->setRefDutyCycle(index, torqueVal);
}

void PartItem::onSliderCurrentCommand(double currentVal, int index)
{
    m_iCur->setRefCurrent(index, currentVal);
}

void PartItem::onSliderVelocityCommand(double speedVal, int index)
{
    m_iVel->velocityMove(index, speedVal);
}

void PartItem::onSliderTorqueCommand(double torqueVal, int index)
{
    m_iTrq->setRefTorque(index, torqueVal);
}

void PartItem::onSliderTrajectoryVelocityCommand(double trajspeedVal, int index)
{
    m_iPos->setRefSpeed(index, trajspeedVal);
}


void PartItem::onSliderDirectPositionCommand(double dirpos, int index)
{
    int mode;
    m_ictrlmode->getControlMode(index, &mode);
    if (mode == VOCAB_CM_POSITION_DIRECT)
    {
        m_iDir->setPosition(index, dirpos);
    }
    else
    {
        yWarning("Joint not in position direct mode so cannot send references");
    }
}

void PartItem::onDumpAllRemoteVariables()
{
    if (m_iVar == nullptr)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, QString("Save Dump for Remote Variables as:"), QDir::homePath()+QString("/RemoteVariablesDump.txt"));

    FILE* dumpfile = fopen(fileName.toStdString().c_str(), "w");
    if (dumpfile != nullptr)
    {
        yarp::os::Bottle keys;
        if (m_iVar->getRemoteVariablesList(&keys))
        {
            std::string s = keys.toString();
            size_t keys_size = keys.size();
            for (size_t i = 0; i < keys_size; i++)
            {
                std::string key_name;
                if (keys.get(i).isString())
                {
                    yarp::os::Bottle val;
                    key_name = keys.get(i).asString();
                    if (m_iVar->getRemoteVariable(key_name, val))
                    {
                        std::string key_value = val.toString();
                        std::string p_value = std::string(key_name).append(" ").append(key_value).append("\n");
                        fputs(p_value.c_str(), dumpfile);
                    }
                }
            }
        }
        fclose(dumpfile);
    }
}


void PartItem::onSliderTrajectoryPositionCommand(double posVal, int index)
{
    int mode;
    m_ictrlmode->getControlMode(index, &mode);

    if ( mode == VOCAB_CM_POSITION)
    {
        m_iPos->positionMove(index, posVal);
    }
    else
    {
        yWarning("Joint not in position mode so cannot send references");
    }
}

void PartItem::onSliderMixedPositionCommand(double posVal, int index)
{
    int mode;
    m_ictrlmode->getControlMode(index, &mode);

    if ( mode == VOCAB_CM_MIXED)
    {
        m_iPos->positionMove(index, posVal);
    }
    else
    {
        LOG_ERROR("Joint not in mixed mode so cannot send references");
    }
}

void PartItem::onSliderMixedVelocityCommand( double vel, int index)
{
    int mode;
    m_ictrlmode->getControlMode(index, &mode);

    if (mode == VOCAB_CM_MIXED)
    {
        m_iVel->velocityMove(index, vel);
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
        m_iinteract->setInteractionMode(jointIndex, (yarp::dev::InteractionModeEnum) VOCAB_IM_COMPLIANT);
        break;
    case JointItem::Stiff:
        yInfo("interaction mode of joint %d set to STIFF", jointIndex);
        m_iinteract->setInteractionMode(jointIndex, (yarp::dev::InteractionModeEnum) VOCAB_IM_STIFF);
        break;
    default:
        break;
    }
}


void PartItem::onSendPWM(int jointIndex, double pwmVal)
{
    double pwm_reference = 0;
    double current_pwm = 0;

    m_iPWM->setRefDutyCycle(jointIndex, pwmVal);

    yarp::os::SystemClock::delaySystem(0.010);
    m_iPWM->getRefDutyCycle(jointIndex, &pwm_reference);  //This is the reference
    yarp::os::SystemClock::delaySystem(0.010);
    m_iPWM->getDutyCycle(jointIndex, &current_pwm);  //This is the real PWM output

    if (m_currentPidDlg){
        m_currentPidDlg->initPWM(pwm_reference, current_pwm);
    }
}

void PartItem::onSendStiffness(int jointIdex,double stiff,double damp,double force)
{
    Q_UNUSED(force);
    double stiff_val=0;
    double damp_val=0;
    double offset_val=0;

    m_iImp->setImpedance(jointIdex, stiff, damp);
    //imp->setImpedanceOffset(jointIdex, force);
    yarp::os::SystemClock::delaySystem(0.005);
    m_iImp->getImpedance(jointIdex, &stiff_val, &damp_val);
    m_iImp->getImpedanceOffset(jointIdex, &offset_val);

    //update the impedance limits
    double stiff_max=0.0;
    double stiff_min=0.0;
    double damp_max=0.0;
    double damp_min=0.0;
    double off_max=0.0;
    double off_min=0.0;
    m_iImp->getCurrentImpedanceLimit(jointIdex, &stiff_min, &stiff_max, &damp_min, &damp_max);
    m_iTrq->getTorqueRange(jointIdex, &off_min, &off_max);

    if (m_currentPidDlg)
    {
        m_currentPidDlg->initStiffness(stiff_val, stiff_min, stiff_max,
                                     damp_val,damp_min,damp_max,
                                     offset_val,off_min,off_max);
    }


}

void PartItem::onSendTorquePid(int jointIndex,Pid newPid,MotorTorqueParameters newTrqParam)
{
    Pid myTrqPid(0,0,0,0,0,0);
    yarp::dev::MotorTorqueParameters TrqParam;
    m_iPid->setPid(VOCAB_PIDTYPE_TORQUE, jointIndex, newPid);

    m_iTrq->setMotorTorqueParams(jointIndex, newTrqParam);
    yarp::os::SystemClock::delaySystem(0.005);
    m_iPid->getPid(VOCAB_PIDTYPE_TORQUE,jointIndex, &myTrqPid);
    m_iTrq->getMotorTorqueParams(jointIndex, &TrqParam);

    if (m_currentPidDlg){
        m_currentPidDlg->initTorque(myTrqPid, TrqParam);
    }
}

void PartItem::onSendPositionPid(int jointIndex,Pid newPid)
{
    Pid myPosPid(0,0,0,0,0,0);
    m_iPid->setPid(VOCAB_PIDTYPE_POSITION, jointIndex, newPid);
    yarp::os::SystemClock::delaySystem(0.005);
    m_iPid->getPid(VOCAB_PIDTYPE_POSITION, jointIndex, &myPosPid);

    if (m_currentPidDlg){
        m_currentPidDlg->initPosition(myPosPid);
    }
}

void PartItem::onSendVelocityPid(int jointIndex, Pid newPid)
{
    Pid myVelPid(0, 0, 0, 0, 0, 0);
    m_iPid->setPid(VOCAB_PIDTYPE_VELOCITY, jointIndex, newPid);
    yarp::os::SystemClock::delaySystem(0.005);
    m_iPid->getPid(VOCAB_PIDTYPE_VELOCITY, jointIndex, &myVelPid);

    if (m_currentPidDlg){
        m_currentPidDlg->initVelocity(myVelPid);
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
    double pwm_reference = 0;
    double current_pwm = 0;

    m_iImp->getCurrentImpedanceLimit(jointIndex, &stiff_min, &stiff_max, &damp_min, &damp_max);
    m_iTrq->getTorqueRange(jointIndex, &off_min, &off_max);

    // Position
    m_iPid->getPid(VOCAB_PIDTYPE_POSITION, jointIndex, &myPosPid);
    yarp::os::SystemClock::delaySystem(0.005);

    // Velocity
    m_iPid->getPid(VOCAB_PIDTYPE_VELOCITY, jointIndex, &myVelPid);
    yarp::os::SystemClock::delaySystem(0.005);

    // Current
    if (m_iCur)
    {
        m_iPid->getPid(VOCAB_PIDTYPE_CURRENT, jointIndex, &myCurPid);
        yarp::os::SystemClock::delaySystem(0.005);
    }

    // Torque
    m_iPid->getPid(VOCAB_PIDTYPE_TORQUE, jointIndex, &myTrqPid);
    m_iTrq->getMotorTorqueParams(jointIndex, &motorTorqueParams);
    yarp::os::SystemClock::delaySystem(0.005);

    //Stiff
    m_iImp->getImpedance(jointIndex, &stiff_val, &damp_val);
    m_iImp->getImpedanceOffset(jointIndex, &impedance_offset_val);
    yarp::os::SystemClock::delaySystem(0.005);

    // PWM
    m_iPWM->getRefDutyCycle(jointIndex, &pwm_reference);
    m_iPWM->getDutyCycle(jointIndex, &current_pwm);

    if (m_currentPidDlg)
    {
        m_currentPidDlg->initPosition(myPosPid);
        m_currentPidDlg->initTorque(myTrqPid, motorTorqueParams);
        m_currentPidDlg->initVelocity(myVelPid);
        m_currentPidDlg->initCurrent(myCurPid);
        m_currentPidDlg->initStiffness(stiff_val, stiff_min, stiff_max, damp_val, damp_min, damp_max, impedance_offset_val, off_min, off_max);
        m_currentPidDlg->initPWM(pwm_reference, current_pwm);
        m_currentPidDlg->initRemoteVariables(m_iVar);
    }
}

void PartItem::onSendCurrentPid(int jointIndex, Pid newPid)
{
    if (m_iCur == nullptr)
    {
        yError() << "iCurrent interface not opened";
        return;
    }
    Pid myCurPid(0, 0, 0, 0, 0, 0);
    m_iPid->setPid(VOCAB_PIDTYPE_CURRENT, jointIndex, newPid);
    yarp::os::SystemClock::delaySystem(0.005);
    m_iPid->getPid(VOCAB_PIDTYPE_CURRENT, jointIndex, &myCurPid);

    if (m_currentPidDlg){
        m_currentPidDlg->initCurrent(myCurPid);
    }
}

void PartItem::onSendSingleRemoteVariable(std::string key, yarp::os::Bottle val)
{
    m_iVar->setRemoteVariable(key, val);
    yarp::os::SystemClock::delaySystem(0.005);
}

void PartItem::onUpdateAllRemoteVariables()
{
    if (m_currentPidDlg){
        m_currentPidDlg->initRemoteVariables(m_iVar);
    }
}

void PartItem::onCalibClicked(JointItem *joint)
{
    if (!m_iremCalib)
    {
        QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return;
    }

    if(QMessageBox::question(this,"Question","Do you really want to recalibrate the joint?") != QMessageBox::Yes){
        return;
    }
    if (!m_iremCalib->calibrateSingleJoint(joint->getJointIndex()))
    {
        // provide better feedback to user by verifying if the calibrator device was set or not
        bool isCalib = false;
        m_iremCalib->isCalibratorDevicePresent(&isCalib);
        if(!isCalib)
            QMessageBox::critical(this,"Calibration failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file has the 'Calibrator' keyword in the attach phase"));
        else
            QMessageBox::critical(this,"Calibration failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
    }

}

void PartItem::onPidClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    QString jointName = joint->getJointName();
    m_currentPidDlg = new PidDlg(m_partName, jointIndex, jointName);
    connect(m_currentPidDlg, SIGNAL(sendPositionPid(int, Pid)), this, SLOT(onSendPositionPid(int, Pid)));
    connect(m_currentPidDlg, SIGNAL(sendVelocityPid(int, Pid)), this, SLOT(onSendVelocityPid(int, Pid)));
    connect(m_currentPidDlg, SIGNAL(sendCurrentPid(int, Pid)), this, SLOT(onSendCurrentPid(int, Pid)));
    connect(m_currentPidDlg, SIGNAL(sendSingleRemoteVariable(std::string, yarp::os::Bottle)), this, SLOT(onSendSingleRemoteVariable(std::string, yarp::os::Bottle)));
    connect(m_currentPidDlg, SIGNAL(updateAllRemoteVariables()), this, SLOT(onUpdateAllRemoteVariables()));
    connect(m_currentPidDlg, SIGNAL(sendTorquePid(int, Pid, MotorTorqueParameters)), this, SLOT(onSendTorquePid(int, Pid, MotorTorqueParameters)));
    connect(m_currentPidDlg, SIGNAL(sendStiffness(int, double, double, double)), this, SLOT(onSendStiffness(int, double, double, double)));
    connect(m_currentPidDlg, SIGNAL(sendPWM(int, double)), this, SLOT(onSendPWM(int, double)));
    connect(m_currentPidDlg, SIGNAL(refreshPids(int)), this, SLOT(onRefreshPids(int)));
    connect(m_currentPidDlg, SIGNAL(dumpRemoteVariables()), this, SLOT(onDumpAllRemoteVariables()));

    this->onRefreshPids(jointIndex);

    m_currentPidDlg->exec();

    delete m_currentPidDlg;
    m_currentPidDlg = nullptr;
}

void PartItem::onRunClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    double posJoint;
    while (!m_iencs->getEncoder(jointIndex, &posJoint)){
        SystemClock::delaySystem(0.001);
    }

    m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_POSITION);
}

void PartItem::onIdleClicked(JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_FORCE_IDLE);
}

void PartItem::onHomeClicked(JointItem *joint)
{
    int NUMBER_OF_JOINTS;
    const int jointIndex = joint->getJointIndex();
    m_iPos->getAxes(&NUMBER_OF_JOINTS);

    this->homeJoint(jointIndex);
}

void PartItem::onJointChangeMode(int mode,JointItem *joint)
{
    const int jointIndex = joint->getJointIndex();
    switch (mode) {
    case JointItem::Idle:{
        yInfo("joint: %d in IDLE mode", jointIndex);
        if (m_ictrlmode){
            m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_IDLE);
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    case JointItem::Position:{
        yInfo("joint: %d in POSITION mode", jointIndex);
        if (m_ictrlmode){
            m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_POSITION);
            joint->resetTarget();
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    case JointItem::PositionDirect:{
        //if(positionDirectEnabled){
            yInfo("joint: %d in POSITION DIRECT mode", jointIndex);
            if (m_ictrlmode){
                joint->resetTarget();
                m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_POSITION_DIRECT);
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
            if (m_ictrlmode){
                joint->resetTarget();
                m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_MIXED);
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
            if (m_ictrlmode)
            {
                m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_VELOCITY);
                yInfo() << "Changing reference acceleration of joint " << jointIndex << " to 100000";
                m_iVel->setRefAcceleration(jointIndex, 100000);
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
            if (m_ictrlmode){
                m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_TORQUE);
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
    case JointItem::Pwm:{
        yInfo("joint: %d in PWM mode", jointIndex);
        if (m_ictrlmode){
            m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_PWM);
        } else {
            yError("ERROR: cannot do!");
        }
        break;
    }
    case JointItem::Current:{
        yInfo("joint: %d in CURRENT mode", jointIndex);
        if (m_ictrlmode){
            m_ictrlmode->setControlMode(jointIndex, VOCAB_CM_CURRENT);
        }
        else {
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
    int count = m_layout->count();


    int jointPerLineCount = (w - 20) / (MAX_WIDTH_JOINT + 10);
    if(jointPerLineCount > count){
        jointPerLineCount = count;
    }
    if(jointPerLineCount <= 0){
        return;
    }

    int extraSpace = (w - 20) - jointPerLineCount * (MAX_WIDTH_JOINT + 10);



    for(int i=0;i<count;i++){
        QWidget *widget = m_layout->itemAt(i)->widget();
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
        int count = m_layout->count();
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
            QWidget *widget = m_layout->itemAt(i)->widget();
            if(widget){
                widget->setMaximumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
                widget->setMinimumWidth(MAX_WIDTH_JOINT + (extraSpace/jointPerLineCount));
            }
        }
    }

}

void PartItem::calibratePart()
{
    if (!m_iremCalib)
    {
        QMessageBox::critical(this,"Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return;
    }

    if (!m_iremCalib->calibrateWholePart())
    {
        // provide better feedback to user by verifying if the calibrator device was set or not
        bool isCalib = false;
        m_iremCalib->isCalibratorDevicePresent(&isCalib);
        if(!isCalib)
            QMessageBox::critical(this, "Calibration failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(m_partName));
        else
            QMessageBox::critical(this,"Calibration failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
    }
}

bool PartItem::homeJoint(int jointIndex)
{
    if (!m_iremCalib)
    {
        QMessageBox::critical(this, "Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return false;
    }

    if (!m_iremCalib->homingSingleJoint(jointIndex))
    {
        // provide better feedback to user by verifying if the calibrator device was set or not
        bool isCalib = false;
        m_iremCalib->isCalibratorDevicePresent(&isCalib);
        if (!isCalib)
        {
            QMessageBox::critical(this, "Operation failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(m_partName));
            return false;
        }
        else
        {
            QMessageBox::critical(this, "Operation failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
            return false;
        }
    }
    return true;
}

bool PartItem::homePart()
{
    if (!m_iremCalib)
    {
        QMessageBox::critical(this, "Operation not supported", QString("The IRemoteCalibrator interface was not found on this application"));
        return false;
    }

    if (!m_iremCalib->homingWholePart())
    {
        // provide better feedback to user by verifying if the calibrator device was set or not
        bool isCalib = false;
        m_iremCalib->isCalibratorDevicePresent(&isCalib);
        if (!isCalib)
        {
            QMessageBox::critical(this, "Operation failed", QString("No calibrator device was configured to perform this action, please verify that the wrapper config file for part %1 has the 'Calibrator' keyword in the attach phase").arg(m_partName));
            return false;
        }
        else
        {
            QMessageBox::critical(this, "Operation failed", QString("The remote calibrator reported that something went wrong during the calibration procedure"));
            return false;
        }
    }
    return true;
}

bool PartItem::homeToCustomPosition(const yarp::os::Bottle& positionElement)
{
    bool ok = true;
    int NUMBER_OF_JOINTS;
    m_iPos->getAxes(&NUMBER_OF_JOINTS);

    if (positionElement.isNull()) {
        QMessageBox::critical(this, "Operation failed", QString("No custom position supplied in configuration file for part ") + QString(m_partName));
        return false;
    }

    //Look for group called m_robotPartPort_Position and m_robotPartPort_Velocity
    Bottle xtmp, ytmp;
    xtmp = positionElement.findGroup(m_robotPartPort.toStdString() + "_Position");
    ok = ok && (xtmp.size() == (size_t) NUMBER_OF_JOINTS + 1);
    ytmp = positionElement.findGroup(m_robotPartPort.toStdString() + "_Velocity");
    ok = ok && (ytmp.size() == (size_t) NUMBER_OF_JOINTS + 1);

    if(ok)
    {
        for (int jointIndex = 0; jointIndex < NUMBER_OF_JOINTS; jointIndex++)
        {
            double position = xtmp.get(jointIndex+1).asFloat64();
            double velocity = ytmp.get(jointIndex + 1).asFloat64();
            m_iPos->setRefSpeed(jointIndex, velocity);
            m_iPos->positionMove(jointIndex, position);
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", QString("Check the number of entries in the group %1").arg(m_robotPartPort));
        ok = false;
    }

    return ok;
}

void PartItem::idlePart()
{
    int NUMBER_OF_JOINTS;
    m_iPos->getAxes(&NUMBER_OF_JOINTS);

    for (int joint=0; joint < NUMBER_OF_JOINTS; joint++){
        m_ictrlmode->setControlMode(joint, VOCAB_CM_IDLE);
    }
}

bool PartItem::checkAndRunAllSeq()
{
    if (!m_sequenceWindow){
        return false;
    }

    return m_sequenceWindow->checkAndRun();


}

bool PartItem::checkAndRunTimeAllSeq()
{
    if (!m_sequenceWindow){
        return false;
    }

    return m_sequenceWindow->checkAndRunTime();


}

bool PartItem::checkAndCycleTimeAllSeq()
{
    if (!m_sequenceWindow){
        return false;
    }

    return m_sequenceWindow->checkAndCycleTimeSeq();
}

bool PartItem::checkAndCycleAllSeq()
{
    if (!m_sequenceWindow){
        return false;
    }

    return m_sequenceWindow->checkAndCycleSeq();
}

void PartItem::runPart()
{
    int NUMBER_OF_JOINTS;
    m_iPos->getAxes(&NUMBER_OF_JOINTS);

    for (int joint=0; joint < NUMBER_OF_JOINTS; joint++){
        //iencs->getEncoder(joint, &posJoint);
        m_ictrlmode->setControlMode(joint, VOCAB_CM_POSITION);
    }
}

void PartItem::loadSequence()
{
    openSequenceWindow();
    onOpenSequence();
}

void PartItem::saveSequence(QString global_filename)
{
    openSequenceWindow();
    m_sequenceWindow->save(global_filename);
}

void PartItem::closeSequenceWindow()
{
    if (m_sequenceWindow)
    {
        m_sequenceWindow->close();
    }
}

void PartItem::openSequenceWindow()
{
    if (!m_sequenceWindow){
        m_sequenceWindow = new SequenceWindow(m_partName, m_layout->count());
        connect(m_sequenceWindow, SIGNAL(itemDoubleClicked(int)), this, SLOT(onSequenceWindowDoubleClicked(int)), Qt::DirectConnection);
        connect(this, SIGNAL(sendPartJointsValues(int, QList<double>, QList<double>)), m_sequenceWindow, SLOT(onReceiveValues(int, QList<double>, QList<double>)), Qt::DirectConnection);
        connect(m_sequenceWindow, SIGNAL(goToPosition(SequenceItem)), this, SLOT(onGo(SequenceItem)));
        connect(m_sequenceWindow, SIGNAL(runTime(QList<SequenceItem>)), this, SLOT(onSequenceRunTime(QList<SequenceItem>)), Qt::QueuedConnection);
        connect(m_sequenceWindow, SIGNAL(run(QList<SequenceItem>)), this, SLOT(onSequenceRun(QList<SequenceItem>)), Qt::QueuedConnection);
        connect(m_sequenceWindow, SIGNAL(saveSequence(QList<SequenceItem>, QString)), this, SLOT(onSaveSequence(QList<SequenceItem>, QString)), Qt::QueuedConnection);
        connect(m_sequenceWindow, SIGNAL(openSequence()), this, SLOT(onOpenSequence()));
        connect(m_sequenceWindow, SIGNAL(cycle(QList<SequenceItem>)), this, SLOT(onSequenceCycle(QList<SequenceItem>)), Qt::QueuedConnection);
        connect(m_sequenceWindow, SIGNAL(cycleTime(QList<SequenceItem>)), this, SLOT(onSequenceCycleTime(QList<SequenceItem>)), Qt::QueuedConnection);
        connect(m_sequenceWindow, SIGNAL(stopSequence()), this, SLOT(onStopSequence()), Qt::QueuedConnection);

        connect(this, SIGNAL(runTimeSequence()), m_sequenceWindow, SLOT(onRunTimeSequence()));
        connect(this, SIGNAL(cycleTimeSequence()), m_sequenceWindow, SLOT(onCycleTimeSequence()));
        connect(this, SIGNAL(cycleSequence()), m_sequenceWindow, SLOT(onCycleSequence()));
        connect(this, SIGNAL(stoppedSequence()), m_sequenceWindow, SLOT(onStoppedSequence()));
        connect(this, SIGNAL(setCurrentIndex(int)), m_sequenceWindow, SLOT(onSetCurrentSequenceIndex(int)));

        connect(this,SIGNAL(runTimeSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(cycleTimeSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(cycleSequence()),this,SLOT(onSequenceActivated()));
        connect(this,SIGNAL(stoppedSequence()),this,SLOT(onSequenceStopped()));


    }

    if (!m_sequenceWindow->isVisible()){
        m_sequenceWindow->show();
    }else{
        m_sequenceWindow->setFocus();
        m_sequenceWindow->raise();
        m_sequenceWindow->setWindowState(Qt::WindowActive);
    }

}

bool PartItem::checkAndGo()
{
    if (!m_sequenceWindow){
        return false;
    }

    return m_sequenceWindow->checkAndGo();
}
void PartItem::stopSequence()
{
    m_cycleTimer.stop();
    m_runTimer.stop();
    m_runTimeTimer.stop();
    m_cycleTimeTimer.stop();
    emit stoppedSequence();
}

void PartItem::onStopSequence()
{
    stopSequence();
}

void PartItem::onOpenSequence()
{
    QString fileName = QFileDialog::getOpenFileName(m_sequenceWindow, QString("Load Sequence for part %1 As").arg(m_partName), QDir::homePath());

    QFileInfo fInfo(fileName);
    if(!fInfo.exists()){
        return;
    }

    QString desiredExtension = QString("pos%1").arg(m_partName);
    QString extension = fInfo.suffix();

    if(desiredExtension != extension){
        QMessageBox::critical(this,"Error Loading The Sequence",
            QString("Wrong format (check extensions) of the file associated with: ").arg(m_partName));
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

    QString referencePart;

    QList<SequenceItem> sequenceItems;
    SequenceItem item;
    while(!reader.atEnd()){
        reader.readNext();

        if(reader.isStartElement()){
            if(reader.name().contains("Sequence_")){ //Sequence_
                QXmlStreamAttributes attributes = reader.attributes();
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

    if (m_sequenceWindow){
        m_sequenceWindow->loadSequence(sequenceItems);
    }

}

void PartItem::onSaveSequence(QList<SequenceItem> values, QString fileName)
{
    if (fileName=="")
    {
        fileName = QFileDialog::getSaveFileName(this, QString("Save Sequence for part %1 As").arg(m_partName), QDir::homePath());
    }

    if(fileName.isEmpty()){
        return;
    }

    QFileInfo fInfo(fileName);
    QString completeFileName = QString("%1/%2.pos%3").arg(fInfo.absolutePath()).arg(fInfo.baseName()).arg(m_partName);
    std::string completeFileName_s = completeFileName.toStdString();

    //QFile file(completeFileName);
    yInfo("Saving file %s\n", completeFileName_s.c_str());

    QFile file(completeFileName);
    if(!file.open(QIODevice::WriteOnly)){
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement(QString("Sequence_pos%1").arg(m_partName));

    writer.writeAttribute("TotPositions", QString("%1").arg(values.count()));
    writer.writeAttribute("ReferencePart", m_partName);

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
    LOG_INFO("File saved and closed\n");


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
    if (m_cycleTimeTimer.isActive() || m_cycleTimer.isActive() || m_runTimeTimer.isActive() || m_runTimer.isActive()){
        return;
    }

    m_cycleTimeValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            m_cycleTimeValues.append(it);
        }
    }

    SequenceItem vals;
    if (m_cycleTimeValues.count() > 0)
    {
        vals = m_cycleTimeValues.takeFirst();
        m_cycleTimeValues.append(vals);
        emit setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        m_cycleTimeTimer.start(vals.getTiming() * 1000);
        emit cycleTimeSequence();
    }
}

void PartItem::onCycleTimeTimerTimeout()
{
    if (m_cycleTimeValues.count() > 0)
    {
        SequenceItem vals = m_cycleTimeValues.takeFirst();
        m_cycleTimeValues.append(vals);
        emit setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        m_cycleTimeTimer.start(vals.getTiming() * 1000);
    }
}

void PartItem::onSequenceCycle(QList<SequenceItem> values)
{
    // Remove items after the first timing with value < 0
    if (m_cycleTimeTimer.isActive() || m_cycleTimer.isActive() || m_runTimeTimer.isActive() || m_runTimer.isActive()){
        return;
    }

    m_cycleValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            m_cycleValues.append(it);
        }
    }

    SequenceItem vals;
    if (m_cycleValues.count() > 0){
        vals = m_cycleValues.takeFirst();

        m_cycleValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        emit setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        m_iPos->setRefSpeeds(cmdVelocities);
        m_iPos->positionMove(cmdPositions);
        m_cycleTimer.start(vals.getTiming() * 1000);

        emit cycleSequence();
    }
}

void PartItem::onCycleTimerTimeout()
{
    if (m_cycleValues.count() > 0){
        SequenceItem vals = m_cycleValues.takeFirst();

        m_cycleValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
        }

        emit setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        m_iPos->setRefSpeeds(cmdVelocities);
        m_iPos->positionMove(cmdPositions);

        m_cycleTimer.start(vals.getTiming() * 1000);
    }
}

void PartItem::onSequenceRun(QList<SequenceItem> values)
{
    // Remove items after the first timing with value < 0
    if (m_cycleTimeTimer.isActive() || m_cycleTimer.isActive() || m_runTimeTimer.isActive() || m_runTimer.isActive()){
        return;
    }

    m_runValues.clear();
    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            m_runValues.append(it);
        }
    }

    SequenceItem vals;
    if (m_runValues.count() > 0){
        vals = m_runValues.takeFirst();

        m_runValues.append(vals);

        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        emit setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        m_iPos->setRefSpeeds(cmdVelocities);
        m_iPos->positionMove(cmdPositions);
        m_runTimer.start(vals.getTiming() * 1000);

        emit runSequence();
    }
}
void PartItem::onRunTimeout()
{
    if (m_runValues.count() > 0){
        SequenceItem vals = m_runValues.takeFirst();
        if(vals.getTiming() < 0){
            emit stoppedSequence();
            return;
        }
        double *cmdPositions = new double[vals.getPositions().count()];
        double *cmdVelocities = new double[vals.getSpeeds().count()];

        for(int i=0;i<vals.getPositions().count();i++){
            cmdPositions[i] = vals.getPositions().at(i);
            cmdVelocities[i] = vals.getSpeeds().at(i);
            //qDebug() << "vals.getSpeeds().at(i)" << vals.getSpeeds().at(i);
        }

        emit setCurrentIndex(vals.getSequenceNumber());
        //fixedTimeMove(vals.getPositions());
        m_iPos->setRefSpeeds(cmdVelocities);
        m_iPos->positionMove(cmdPositions);


        m_runTimer.start(vals.getTiming() * 1000);
    }else{
        emit stoppedSequence();
    }
}



void PartItem::onSequenceRunTime(QList<SequenceItem> values)
{
    if (m_cycleTimeTimer.isActive() || m_cycleTimer.isActive() || m_runTimeTimer.isActive() || m_runTimer.isActive()){
        return;
    }

    m_runTimeValues.clear();

    for(int i=0;i<values.count();i++){
        SequenceItem it = values.at(i);
        if(it.getTiming() > 0){
            m_runTimeValues.append(it);
        }
    }

    SequenceItem vals;
    if (m_runTimeValues.count() > 0){
        vals = m_runTimeValues.takeFirst();

        emit setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        m_runTimeTimer.start(vals.getTiming() * 1000);

        emit runTimeSequence();
    }

}

void PartItem::onRunTimerTimeout()
{
    if (m_runTimeValues.count() > 0){
        SequenceItem vals = m_runTimeValues.takeFirst();
        if(vals.getTiming() < 0){
            emit stoppedSequence();
            return;
        }
        emit setCurrentIndex(vals.getSequenceNumber());
        fixedTimeMove(vals);
        m_runTimeTimer.start(vals.getTiming() * 1000);
    }else{
        emit stoppedSequence();
    }
}

void PartItem::fixedTimeMove(SequenceItem sequence)
{
    int NUM_JOINTS;
    m_iPos->getAxes(&NUM_JOINTS);
    auto* cmdPositions = new double[NUM_JOINTS];
    auto* cmdVelocities = new double[NUM_JOINTS];
    auto* startPositions = new double[NUM_JOINTS];
    double cmdTime = sequence.getTiming();

    while (!m_iencs->getEncoders(startPositions)){
        SystemClock::delaySystem(0.001);
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

    m_iPos->setRefSpeeds(cmdVelocities);
  m_iPos->positionMove(cmdPositions);

  m_sequence_port_stamp.update();
  m_sequence_port.setEnvelope(m_sequence_port_stamp);
  Vector v(NUM_JOINTS,cmdPositions);
  m_sequence_port.write(v);
  delete[] cmdVelocities;
  delete[] startPositions;
  delete[] cmdPositions;
  return;
}

void PartItem::onGo(SequenceItem sequenceItem)
{
    if(sequenceItem.getPositions().isEmpty() || sequenceItem.getSpeeds().isEmpty())
    {
        QMessageBox::critical(this,"Error", "Select an entry in the table before performing a movement");
        return;
    }

    int NUMBER_OF_JOINTS;
    m_iPos->getAxes(&NUMBER_OF_JOINTS);

    for(int i=0;i<NUMBER_OF_JOINTS;i++)
    {
        m_iPos->setRefSpeed(i, sequenceItem.getSpeeds().at(i));
        m_iPos->positionMove(i, sequenceItem.getPositions().at(i));
    }
}

void PartItem::onSequenceActivated()
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        if(joint){
            joint->sequenceActivated();
        }
    }

    emit sequenceActivated();

}

void PartItem::onSequenceStopped()
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        if(joint){
            joint->sequenceStopped();
        }
    }
    emit sequenceStopped();
}

void PartItem::onSequenceWindowDoubleClicked(int sequenceNum)
{
    QList<double>values;
    QList<double>speeds;
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        values.append(this->m_positions[i]);
        speeds.append(joint->getTrajectoryVelocityValue());
    }

    emit sendPartJointsValues(sequenceNum,values,speeds);
}

void PartItem::onEnableControlVelocity(bool control)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->enableControlVelocity(control);
    }
}

void PartItem::onEnableControlMixed(bool control)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->enableControlMixed(control);
    }
}

void PartItem::onEnableControlPositionDirect(bool control)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->enableControlPositionDirect(control);
    }
}

void PartItem::onEnableControlPWM(bool control)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->enableControlPWM(control);
    }
}

void PartItem::onEnableControlCurrent(bool control)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->enableControlCurrent(control);
    }
}

void PartItem::onViewSpeedValues(bool view)
{
    m_part_speedVisible = view;
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->setSpeedVisible(view);
    }
}

void PartItem::onViewMotorPositions(bool view)
{
    m_part_motorPositionVisible = view;
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->setMotorPositionVisible(view);
    }
}

void PartItem::onViewDutyCycles(bool view)
{
    m_part_dutyVisible = view;
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->setDutyVisible(view);
    }
}

void PartItem::onViewCurrentValues(bool view)
{
    m_part_currentVisible = view;
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->setCurrentsVisible(view);
    }
}

void PartItem::onViewPositionTarget(bool ena)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        joint->viewPositionTarget(ena);
    }
}

void PartItem::onSetPosSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
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
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
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

void PartItem::onSetCurSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
        if (mode == 0)
        {
            joint->enableCurrentSliderDoubleAuto();
        }
        else if (mode == 1)
        {
            joint->enableCurrentSliderDoubleValue(step);
        }
        else if (mode == 2)
        {
            joint->enableCurrentSliderDoubleValue(1.0);
        }
        else
        {
            joint->disableCurrentSliderDouble();
        }
    }
}

void PartItem::onSetTrqSliderOptionPI(int mode, double step)
{
    for (int i = 0; i<m_layout->count(); i++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
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
    return m_node;
}

void PartItem::setTreeWidgetModeNode(QTreeWidgetItem *node)
{
    m_node = node;
}

QList<int> PartItem::getPartMode()
{
    QList <int> modes;

    for (int k = 0; k < m_layout->count(); k++){
        switch (m_controlModes[k])
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
        case VOCAB_CM_PWM:
            modes.append(JointItem::Pwm);
            break;
        case VOCAB_CM_CURRENT:
            modes.append(JointItem::Current);
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
    bool ret = m_ictrlmode->getControlModes(m_controlModes);


    if(ret==false){
        LOG_ERROR("ictrl->getControlMode failed\n" );
    }
}

bool PartItem::updatePart()
{
    bool ret = false;
    int number_of_joints=0;
    m_iPos->getAxes(&number_of_joints);
    if (m_slow_k >= number_of_joints - 1) m_slow_k = 0;
    else m_slow_k++;

    if (number_of_joints == 0)
    {
        LOG_ERROR("Lost connection with the robot. You should save and restart.\n" );
        SystemClock::delaySystem(0.1);

        for (int i = 0; i<m_layout->count(); i++){
            auto* joint = (JointItem*)m_layout->itemAt(i)->widget();
            joint->setJointState(JointItem::Disconnected);
        }
        return false;
    }

    // *** update measured encoders, velocity, torques ***
    bool b = true;
    if (true)
    {
        b = m_iencs->getEncoders(m_positions);
        if (!b) { yWarning("Unable to update encoders"); return false; }
    }
    if (true)
    {
        b = m_iTrq->getTorques(m_torques);
        if (!b) { yWarning("Unable to update torques"); }
    }
    if (this->m_part_currentVisible)
    {
        b = m_iCur->getCurrents(m_currents);
        if (!b) { yWarning("Unable to update currents"); }
    }
    if (this->m_part_speedVisible)
    {
        b = m_iencs->getEncoderSpeeds(m_speeds);
        if (!b) { yWarning("Unable to update speeds"); }
    }
    if (this->m_part_motorPositionVisible)
    {
        b = m_iMot->getMotorEncoders(m_motorPositions);
        if (!b) { yWarning("Unable to update motorPositions"); }
    }
    if (this->m_part_dutyVisible)
    {
        b = m_iPWM->getDutyCycles(m_dutyCycles);
        if (!b) { yWarning("Unable to update dutyCycles"); }
    }

    // *** update checkMotionDone, refTorque, refTrajectorySpeed, refSpeed ***
    // (only one at a time in order to save bandwidth)
    bool b_motdone = m_iPos->checkMotionDone(m_slow_k, &m_done[m_slow_k]); //using k to save bandwidth
    bool b_refTrq = m_iTrq->getRefTorque(m_slow_k, &m_refTorques[m_slow_k]); //using k to save bandwidth
    bool b_refPosSpeed = m_iPos->getRefSpeed(m_slow_k, &m_refTrajectorySpeeds[m_slow_k]); //using k to save bandwidth
    bool b_refVel = m_iVel->getRefVelocity(m_slow_k, &m_refVelocitySpeeds[m_slow_k]); //this interface is missing!
    bool b_refPos = m_iPos->getTargetPosition(m_slow_k, &m_refTrajectoryPositions[m_slow_k]);

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
        auto* joint = (JointItem*)m_layout->itemAt(jk)->widget();
        if (true) { joint->setPosition(m_positions[jk]); }
        else {}
        if (true) { joint->setTorque(m_torques[jk]); }
        else {}
        if (true) { joint->setSpeed(m_speeds[jk]); }
        else {}
        if (true) { joint->setCurrent(m_currents[jk]); }
        else {}
        if (true) { joint->setMotorPosition(m_motorPositions[jk]); }
        else {}
        if (true) { joint->setDutyCycles(m_dutyCycles[jk]); }
        else {}
    }

    // *** update the widget NOT every cycle ***
    {
        auto* joint_slow_k = (JointItem*)m_layout->itemAt(m_slow_k)->widget();
        if (b_refTrq) { joint_slow_k->setRefTorque(m_refTorques[m_slow_k]); }
        else {}
        if (b_refPosSpeed) { joint_slow_k->setRefTrajectorySpeed(m_refTrajectorySpeeds[m_slow_k]); }
        else {}
        if (b_refPos) { joint_slow_k->setRefTrajectoryPosition(m_refTrajectoryPositions[m_slow_k]); }
        else {}
        if (b_refVel) { joint_slow_k->setRefVelocitySpeed(m_refVelocitySpeeds[m_slow_k]); }
        else {}
        if (b_motdone) { joint_slow_k->updateMotionDone(m_done[m_slow_k]); }
        else {}
    }


    // *** update the controlMode, interactionMode ***
    // this is already done by updateControlMode() (because it also needs to update the tree, not only the single joint widget)
    //    ret=ctrlmode2->getControlModes(controlModes);
    //    if(ret==false){
    //        LOG_ERROR("ictrl->getControlMode failed\n" );
    //    }
    ret = m_iinteract->getInteractionModes(m_interactionModes);
    if(ret==false){
        LOG_ERROR("iint->getInteractionlMode failed\n" );
    }

    for (int k = 0; k < number_of_joints; k++)
    {
        auto* joint = (JointItem*)m_layout->itemAt(k)->widget();
        switch (m_controlModes[k])
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
            case VOCAB_CM_CURRENT:
            {
                joint->setJointState(JointItem::Current);
                double ref_current = 0;
                m_iCur->getRefCurrent(k, &ref_current);
                joint->setRefCurrent(ref_current);
                break;
            }
            case VOCAB_CM_PWM:
            {
                joint->setJointState(JointItem::Pwm);
                double ref_duty = 0;
                m_iPWM->getRefDutyCycle(k, &ref_duty);
                joint->setRefPWM(ref_duty);
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
        switch (m_interactionModes[k])
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

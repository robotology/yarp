/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef PARTITEM_H
#define PARTITEM_H

#include "flowlayout.h"
#include "sequencewindow.h"
#include "jointitem.h"
#include "piddlg.h"
#include "yarpmotorgui.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlLimits2.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Stamp.h>

#include <QWidget>
#include <QTimer>



using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

#define     MAX_WIDTH_JOINT 240



class PartItem : public QWidget
{
    Q_OBJECT
public:
    explicit PartItem(QString robotName,
                      int partId,
                      QString partName,
                      ResourceFinder& _finder,
                      bool debug_param_enabled,
                      bool speedview_param_enabled,
                      bool enable_calib_all,
                      QWidget *parent = 0);


    ~PartItem();
    bool openPolyDrivers();
    void initInterfaces();
    bool openInterfaces();
    bool getInterfaceError();
    void openSequenceWindow();
    void closeSequenceWindow();
    bool cycleAllSeq();
    bool checkAndRunAllSeq();
    bool checkAndRunTimeAllSeq();
    bool checkAndCycleAllSeq();
    bool checkAndCycleTimeAllSeq();
    void runPart();
    void idlePart();
    bool homeJoint(int joint);
    bool homePart();
    bool homeToCustomPosition(std::string suffix);
    void calibratePart();
    bool checkAndGo();
    void stopSequence();
    void setTreeWidgetModeNode(QTreeWidgetItem *node);
    void loadSequence();
    void saveSequence(QString global_filename);
    QTreeWidgetItem *getTreeWidgetModeNode();
    QString getPartName();
    QList<int> getPartMode();
    void resizeWidget(int w);

private:
    void fixedTimeMove(SequenceItem sequence);

protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *event );

private:
    QTreeWidgetItem *m_node;
    FlowLayout *m_layout;
    SequenceWindow *m_sequenceWindow;
    QString m_robotPartPort;
    QString m_robotName;
    QString m_partName;
    int     m_partId;
    bool   m_mixedEnabled;
    bool   m_positionDirectEnabled;
    bool   m_openloopEnabled;
    PidDlg *m_currentPidDlg;
    Stamp  m_sequence_port_stamp;
    QTimer m_runTimer;
    QTimer m_runTimeTimer;
    QTimer m_cycleTimer;
    QTimer m_cycleTimeTimer;

    QList<SequenceItem> m_runValues;
    QList<SequenceItem> m_runTimeValues;
    QList<SequenceItem> m_cycleValues;
    QList<SequenceItem> m_cycleTimeValues;
    int*    m_controlModes;
    double* m_refTrajectorySpeeds;
    double* m_refTrajectoryPositions;
    double* m_refTorques;
    double* m_refVelocitySpeeds;
    double* m_torques;
    double* m_positions;
    double* m_speeds;
    double* m_motorPositions;
    bool*   m_done;
    bool    m_part_speedVisible;
    bool    m_part_motorPositionVisible;
    yarp::dev::InteractionModeEnum* m_interactionModes;

    ResourceFinder* m_finder;
    PolyDriver*     m_partsdd;
    Property        m_partOptions;
    Port            m_sequence_port;
    bool            m_interfaceError;

    IPositionControl2  *m_iPos;
    IPositionDirect    *m_iDir;
    IVelocityControl2  *m_iVel;
    IRemoteVariables   *m_iVar;
    IEncoders          *m_iencs;
    IMotorEncoders     *m_iMot;
    IAmplifierControl  *m_iAmp;
    IPidControl        *m_iPid;
    IOpenLoopControl   *m_iOpl;
    ITorqueControl     *m_iTrq;
    IImpedanceControl  *m_iImp;
    IAxisInfo         *m_iinfo;
    IControlLimits2          *m_iLim;
    IControlCalibration2     *m_ical;
    IControlMode2           *m_ictrlmode2;
    IInteractionMode        *m_iinteract;
    IRemoteCalibrator   *m_iremCalib;
    int m_slow_k;

signals:
    void sendPartJointsValues(int,QList<double>,QList<double>);
    void stoppedSequence();
    void cycleSequence();
    void cycleTimeSequence();
    void runTimeSequence();
    void runSequence();
    void setCurrentIndex(int);
    void sequenceActivated();
    void sequenceStopped();

public slots:
    void updateControlMode();
    bool updatePart();
    void onViewSpeedValues(bool);
    void onViewMotorPositions(bool);
    void onSetPosSliderOptionPI(int mode, double step);
    void onSetVelSliderOptionPI(int mode, double step);
    void onSetTrqSliderOptionPI(int mode, double step);
    void onViewPositionTarget(bool);
    void onEnableControlVelocity(bool control);
    void onEnableControlMixed(bool control);
    void onEnableControlPositionDirect(bool control);
    void onEnableControlOpenloop(bool control);

private slots:
    void onSequenceActivated();
    void onSequenceStopped();
    void onStopSequence();
    void onCycleTimeTimerTimeout();
    void onCycleTimerTimeout();
    void onRunTimerTimeout();
    void onRunTimeout();
    void onGo(SequenceItem);
    void onOpenSequence();
    void onSaveSequence(QList<SequenceItem> values, QString fileName);
    void onSequenceRunTime(QList<SequenceItem>);
    void onSequenceRun(QList<SequenceItem> values);
    void onSequenceCycle(QList<SequenceItem>);
    void onSequenceCycleTime(QList<SequenceItem>);
    void onCalibClicked(JointItem *joint);
    void onJointChangeMode(int mode,JointItem *joint);
    void onJointInteraction(int interaction,JointItem *joint);
    void onSliderDirectPositionCommand(double dirpos, int index);
    void onSliderMixedPositionCommand(double pos, int index);
    void onSliderMixedVelocityCommand(double vel, int index);
    void onSliderTorqueCommand(double torqueVal, int index);
    void onSliderTrajectoryPositionCommand(double pos, int index);
    void onSliderTrajectoryVelocityCommand(double speedVal, int index);
    void onSliderOpenloopCommand(double openloopVal, int index);
    void onSliderVelocityCommand(double speedVal, int index);
    void onSequenceWindowDoubleClicked(int sequenceNum);
    void onHomeClicked(JointItem *joint);
    void onIdleClicked(JointItem *joint);
    void onRunClicked(JointItem *joint);
    void onPidClicked(JointItem *joint);
    void onSendPositionPid(int jointIndex, Pid newPid);
    void onSendVelocityPid(int jointIndex, Pid newPid);
    void onSendCurrentPid(int jointIndex, Pid newPid);
    void onSendSingleRemoteVariable(std::string key, yarp::os::Bottle val);
    void onUpdateAllRemoteVariables();
    void onSendTorquePid(int jointIndex, Pid newPid, MotorTorqueParameters newTorqueParam);
    void onSendStiffness(int jointIdex, double stiff, double damp, double force);
    void onSendOpenLoop(int jointIndex, int openLoopVal);
    void onRefreshPids(int jointIndex);


};

#endif // PARTITEM_H

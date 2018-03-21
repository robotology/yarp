/*
 * Copyright (C) 2010 RobotCub Consortium
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see LICENSE
 */


#ifndef JOINTITEM_H
#define JOINTITEM_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <yarp/dev/ControlBoardInterfaces.h>
#include "sliderWithTarget.h"

namespace Ui {
class JointItem;
}

class WheelEventFilter;



#define     STIFF           0
#define     COMPLIANT       1

class JointItem : public QWidget
{
    Q_OBJECT

    public:
    enum JointState {Idle = 0,Position,PositionDirect,Mixed,Velocity,Torque,Pwm,Current,
                     Disconnected, HwFault, Calibrating, CalibDone, NotConfigured, Configured,Unknown, StateStarting} ;
    enum JointInteraction {Stiff, Compliant, InteractionStarting} ;
    explicit JointItem(int index, QWidget *parent = 0);
    ~JointItem();
    void setJointInteraction(JointInteraction interaction);
    void setJointState(JointState);
    void setPosition(double val);
    void setTorque(double val);
    void setRefTorque(double val);
    void setRefVelocitySpeed(double val);
    void setRefTrajectorySpeed(double val);
    void setRefTrajectoryPosition(double val);
    void setSpeed(double val);
    void setMotorPosition(double val);
    void setPWM(double val);
    void setCurrent(double val);
    void updateMotionDone(bool done);
    void setJointName(QString name);
    QString getJointName();
    int getJointIndex();
    void setPositionRange(double min, double max);
    void setVelocityRange(double min, double max);
    void setTrajectoryVelocityRange(double max);
    void setPWMRange(double min, double max);
    void setCurrentRange(double min, double max);
    void setTorqueRange(double max);
    double getTrajectoryPositionValue();
    double getTrajectoryVelocityValue();
    void setEnabledOptions(bool debug_param_enabled,
                           bool speedview_param_enabled,
                           bool enable_calib_all);

    void setSpeedVisible(bool);
    void setMotorPositionVisible(bool);
    void setUnits(yarp::dev::JointTypeEnum t);
    void viewPositionTarget(bool);
    void enableControlVelocity(bool control);
    void enableControlMixed(bool control);
    void enableControlPositionDirect(bool control);
    void enableControlPWM(bool control);
    void enableControlCurrent(bool control);
    void sequenceActivated();
    void sequenceStopped();

    void enablePositionSliderDoubleAuto();
    void enablePositionSliderDoubleValue(double value);
    void disablePositionSliderDouble();
    void enableVelocitySliderDoubleAuto();
    void enableVelocitySliderDoubleValue(double value);
    void disableVelocitySliderDouble();
    void enableTorqueSliderDoubleAuto();
    void enableTorqueSliderDoubleValue(double value);
    void disableTorqueSliderDouble();
    void enableCurrentSliderDoubleAuto();
    void enableCurrentSliderDoubleValue(double value);
    void disableCurrentSliderDouble();
    void enableTrajectoryVelocitySliderDoubleAuto();
    void enableTrajectoryVelocitySliderDoubleValue(double value);
    void disableTrajectoryVelocitySliderDouble();
    void resetTarget();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void enableAll();
    void installFilter();
    void setJointInternalState(int mode);
    void setJointInternalInteraction(int interaction);

    void updateSliderPosition                (SliderWithTarget *slider, double val);
    void updateSliderVelocity                (double val);
    void updateSliderTrajectoryVelocity      (double val);
    void updateSliderPWM                     (double val);
    void updateSliderCurrent                 (double val);
    void updateSliderTorque                  (double val);

    void updateTrajectoryPositionTarget      (double val);
    void updateMixedPositionTarget           (double val);

private:
    Ui::JointItem *ui;
    QString comboStyle1;
    QString comboStyle2;
    int jointIndex;
    QString jointName;
    bool sliderVelocityPressed;
    bool sliderDirectPositionPressed;
    bool sliderMixedPositionPressed;
    bool sliderMixedVelocityPressed;
    bool sliderTrajectoryVelocityPressed;
    bool sliderTrajectoryPositionPressed;
    bool sliderTorquePressed;
    bool sliderPWMPressed;
    bool sliderCurrentPressed;
    bool motionDone;
    QString movingSliderStyle;
    bool enableCalib;
    bool joint_speedVisible;
    bool joint_motorPositionVisible;
    QTimer velocityTimer;
    double lastVelocity;
    bool velocityModeEnabled;

    int     IDLE;
    int     POSITION;
    int     POSITION_DIR;
    int     MIXED;
    int     VELOCITY;
    int     TORQUE;
    int     PWM;
    int     CURRENT;

    int shiftPositions;

    JointState internalState;
    JointInteraction internalInteraction;

    double max_current;
    double min_current;
    double max_torque;
    double min_torque;
    double max_position;
    double min_position;
    double max_velocity;
    double min_velocity;
    double max_trajectory_velocity;

    //double speed;

    double ref_speed;
    double ref_torque;
    double ref_pwm;
    double ref_current;
    double ref_trajectory_velocity;



private slots:
    void onModeChanged(int index);
    void onInteractionChanged(int index);

    void onSliderTrajectoryPositionPressed();
    void onSliderTrajectoryPositionReleased();

    void onSliderDirectPositionPressed();
    void onSliderDirectPositionReleased();

    void onSliderMixedPositionPressed();
    void onSliderMixedPositionReleased();

    void onSliderTrajectoryVelocityReleased();
    void onSliderTrajectoryVelocityPressed();

    void onSliderMixedVelocityReleased();
    void onSliderMixedVelocityPressed();

    void onSliderTorquePressed();
    void onSliderTorqueReleased();

    void onSliderCurrentPressed();
    void onSliderCurrentReleased();

    void onSliderPWMPressed();
    void onSliderPWMReleased();

    void onSliderVelocityReleased();
    void onSliderVelocityPressed();

    void onCalibClicked();
    void onHomeClicked();
    void onIdleClicked();
    void onRunClicked();
    void onPidClicked();
    void onVelocityTimer();
    void onStackedWidgetChanged(int);

signals:
    void calibClicked(JointItem *joint);
    void pidClicked(JointItem *joint);
    void homeClicked(JointItem *joint);
    void idleClicked(JointItem *joint);
    void runClicked(JointItem *joint);
    void changeMode(int mode,JointItem *joint);
    void changeInteraction(int interaction,JointItem *joint);

    void sliderTrajectoryPositionCommand(double val, int jointIndex);
    void sliderMixedPositionCommand(double val,  int jointIndex);
    void sliderMixedVelocityCommand(double val, int jointIndex);
    void sliderDirectPositionCommand(double val, int jointIndex);
    void sliderTrajectoryVelocityCommand(double val, int jointIndex);
    void sliderTorqueCommand(double val, int jointIndex);
    void sliderPWMCommand(double val, int jointIndex);
    void sliderCurrentCommand(double val, int jointIndex);
    void sliderVelocityCommand(double val, int jointIndex);
};


class WheelEventFilter : public QObject
{
    Q_OBJECT


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};


#endif // JOINTITEM_H

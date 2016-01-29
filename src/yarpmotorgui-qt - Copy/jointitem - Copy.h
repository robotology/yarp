/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef JOINTITEM_H
#define JOINTITEM_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>

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
    enum JointState {Idle = 0,Position,PositionDirect,Mixed,Velocity,Torque,OpenLoop,
                     Disconnected, HwFault, Calibrating, CalibDone, NotConfigured, Configured,Unknown, StateStarting} ;
    enum JointInteraction {Stiff, Compliant, InteractionStarting} ;
    explicit JointItem(int index, QWidget *parent = 0);
    ~JointItem();
    void setJointInteraction(JointInteraction interaction);
    void setJointState(JointState);
    void setTorque(double max,double min,double val);
    void setPosition(double val);
    void setTorque(double val);
    void setRefTorque(double val);
    void setSpeed(double val);
    void setOpenLoop(double val);
    void setMotionDone(bool done);
    void setJointName(QString name);
    int getJointIndex();
    void setPositionRange(double min, double max);
    void setVelocityRange(double min, double max);
    void setTrajectoryVelocityRange(double max);
    void setOpenLoopRange(double min, double max);
    double getPositionValue();
    double getPositionSpeed();
    double getPositionSliderSpeed();
    void setEnabledOptions(bool debug_param_enabled,
                           bool speedview_param_enabled,
                           bool enable_calib_all,
                           bool position_direct_enabled,
                           bool openloop_enabled);

    void setSpeedVisible(bool);
    void controlVelocity(bool control);
    void sequenceActivated();
    void sequenceStopped();

    void enablePositionSliderDoubleAuto();
    void enablePositionSliderDoubleValue(double value);
    void disablePositionSliderDouble();
    void enableVelocitySliderDoubleAuto();
    void enableVelocitySliderDoubleValue(double value);
    void disableVelocitySliderDouble();
    void enableTrajectoryVelocitySliderDoubleAuto();
    void enableTrajectoryVelocitySliderDoubleValue(double value);
    void disableTrajectoryVelocitySliderDouble();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void enableAll();
    void installFilter();
    void setJointInternalState(int mode);
    void setJointInternalInteraction(int interaction);

    void updateSliderPosition                (QSlider *slider, QLabel *label, double val);
    void updateSliderVelocity                (QSlider *slider, QLabel *label, double val);
    void updateSliderTrajectoryVelocity      (QSlider *slider, QLabel *label, double val);
    void updateSliderOpenloop                (QSlider *slider, QLabel *label, double val);
    void updateSliderTorque                  (double val);

    void updateSliderPositionLabel           (QSlider *slider, QLabel *label, double val);
    void updateSliderVelocityLabel           (QSlider *slider, QLabel *label, double val);
    void updateSliderMixedVelocityLabel      (QSlider *slider, QLabel *label, double val);
    void updateSliderTrajectoryVelocityLabel (QSlider *slider, QLabel *label, double val);
    void updateSliderOpenloopLabel           (QSlider *slider, QLabel *label, double val);
    void updateSliderTorqueLabel             (QSlider *slider, QLabel *label, double val);
    void updateTarget                        (double val);

private:
    Ui::JointItem *ui;
    QString comboStyle1;
    QString comboStyle2;
    int jointIndex;
    QString jointName;
    bool sliderVelocityPressed;
    bool sliderMixedVelocityPressed;
    bool sliderTrajectoryVelocityPressed;
    bool sliderPositionPressed;
    bool sliderTorquePressed;
    bool sliderOpenloopPressed;
    QString movingSliderStyle;
    bool enableCalib;
    bool speedVisible;
    QTimer velocityTimer;
    double lastVelocity;
    bool motionDone;
    bool velocityModeEnabled;
    bool positionSliderIsDouble;
    bool positionSliderStepIsAuto;
    bool velocitySliderIsDouble;
    bool velocitySliderStepIsAuto;
    bool trajectoryVelocitySliderIsDouble;
    bool trajectoryVelocitySliderStepIsAuto;
    double positionSliderStep;
    double velocitySliderStep;
    double trajectoryVelocitySliderStep;
    double torqueSliderStep;

    int     IDLE;
    int     POSITION;
    int     POSITION_DIR;
    int     MIXED;
    int     VELOCITY;
    int     TORQUE;
    int     OPENLOOP;

    int shiftPositions;

//    QColor idleColor;
//    QColor positionColor;
//    QColor positionDirectColor;
//    QColor mixedColor;
//    QColor velocityColor;
//    QColor torqueColor;
//    QColor openLoopColor;
//    QColor errorColor;
//    QColor disconnectColor;
//    QColor hwFaultColor;
//    QColor calibratingColor;

    JointState internalState;
    JointInteraction internalInteraction;

    double max_torque;
    double min_torque;
    double max_position;
    double min_position;
    double max_velocity;
    double min_velocity;
    double max_trajectory_velocity;
    double torque;
    double position;
    double speed;



private slots:
    void onModeChanged(int index);
    void onInteractionChanged(int index);

    void onSliderPositionPressed();
    void onSliderPositionReleased();
    void onSliderPositionValueChanged(int);
    void onSliderPositionActionTriggered(int);
    void onSliderPositionMoved(int);

    void onSliderTrajectoryVelocityReleased();
    void onSliderTrajectoryVelocityPressed();
    void onSliderTrajectoryVelocityMoved(int val);
    void onSliderTrajectoryVelocityChanged(int val);

    void onSliderMixedVelocityReleased();
    void onSliderMixedVelocityPressed();
    void onSliderMixedVelocityMoved(int val);
    void onSliderMixedVelocityChanged(int val);

    void onSliderTorquePressed();
    void onSliderTorqueReleased();
    void onSliderTorqueMoved(int val);

    void onSliderOpenloopPressed();
    void onSliderOpenloopReleased();
    void onSliderOpenloopMoved(int val);

    void onSliderVelocityReleased();
    void onSliderVelocityPressed();
    void onSliderVelocityMoved(int val);
    void onSliderVelocityChanged(int val);

  //  void onSliderVelocityValueChanged(int);
  //  void onSliderVelocityActionTriggered(int);

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
    void sliderPositionMoved(double val, double speedVal, int jointIndex);
    void sliderTrajectoryVelocityMoved(double val, int jointIndex);
    void sliderTorqueMoved(double val, int jointIndex);
    void sliderOpenloopMoved(double val, int jointIndex);
    void sliderVelocityMoved(double val, int jointIndex);
};


class WheelEventFilter : public QObject
{
    Q_OBJECT


protected:
    bool eventFilter(QObject *obj, QEvent *event);
};


#endif // JOINTITEM_H

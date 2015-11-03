/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
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

class SliderWithTarget : public QSlider
{
public:
    bool enableViewLabel;
    bool enableViewTarget;
    bool disableClickOutOfHandle;
    SliderWithTarget(QWidget * parent = 0, bool _hasTargetOption = true);
    ~SliderWithTarget() { if (sliderLabel) { delete sliderLabel; sliderLabel = 0; } }
    void updateSliderTarget(double val);
    void resetTarget();
    void setSliderStep(double val);
    double getSliderStep();
    void setValue(double val);
    void setIsDouble(bool b);
    bool getIsDouble();

protected:
    bool isDouble;
    QLabel* sliderLabel;
    double target;
    double width_at_target;
    double sliderStep;
    bool hasTargetOption;
    void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event);
};

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
    void setPosition(double val);
    void setTorque(double val);
    void setRefTorque(double val);
    void setRefTrajectorySpeed(double val);
    void setSpeed(double val);
    void setOpenLoop(double val);
    void updateMotionDone(bool done);
    void setJointName(QString name);
    int getJointIndex();
    void setPositionRange(double min, double max);
    void setVelocityRange(double min, double max);
    void setTrajectoryVelocityRange(double max);
    void setOpenLoopRange(double min, double max);
    void setTorqueRange(double max);
    double getTrajectoryPositionValue();
    double getTrajectoryVelocityValue();
    void setEnabledOptions(bool debug_param_enabled,
                           bool speedview_param_enabled,
                           bool enable_calib_all,
                           bool position_direct_enabled,
                           bool openloop_enabled);

    void setSpeedVisible(bool);
    void viewPositionTarget(bool);
    void controlVelocity(bool control);
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
    void enableTrajectoryVelocitySliderDoubleAuto();
    void enableTrajectoryVelocitySliderDoubleValue(double value);
    void disableTrajectoryVelocitySliderDouble();
    void resetTarget();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void enableAll();
    void installFilter();
    void setJointInternalState(int mode);
    void setJointInternalInteraction(int interaction);

    void updateSliderPosition                (SliderWithTarget *slider, double val);
    void updateSliderVelocity                (SliderWithTarget *slider, double val);
    void updateSliderTrajectoryVelocity      (double val);
    void updateSliderOpenloop                (double val);
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
    bool sliderOpenloopPressed;
    bool motionDone;
    QString movingSliderStyle;
    bool enableCalib;
    bool speedVisible;
    QTimer velocityTimer;
    double lastVelocity;
    bool velocityModeEnabled;
    bool positionSliderStepIsAuto;
    bool velocitySliderStepIsAuto;
    bool torqueSliderStepIsAuto;
    bool trajectoryVelocitySliderStepIsAuto;

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

    //double speed;

    double ref_torque;
    double ref_openloop;
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

    void onSliderOpenloopPressed();
    void onSliderOpenloopReleased();

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
    void sliderOpenloopCommand(double val, int jointIndex);
    void sliderVelocityCommand(double val, int jointIndex);
};


class WheelEventFilter : public QObject
{
    Q_OBJECT


protected:
    bool eventFilter(QObject *obj, QEvent *event);
};


#endif // JOINTITEM_H

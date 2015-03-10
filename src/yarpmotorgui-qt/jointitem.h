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
    void setSpeed(double val);
    void setOpenLoop(double val);
    void setMotionDone(bool done);
    int getJointIndex();
    void setPositionRange(double min, double max);
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

    void sequenceActivated();
    void sequenceStopped();

private:
    void enableAll();
    void installFilter();
    void setJointInternalState(int mode);
    void setJointInternalInteraction(int interaction);
    void updateSlider(QSlider *slider, QLabel *label, double val);
    void updateSliderTorque(double val);
    void updateSliderLabel(QSlider *slider,QLabel *label,double val);
    void updateSliderTorqueLabel(QSlider *slider,QLabel *label,double val);

private:
    Ui::JointItem *ui;
    QString comboStyle1;
    QString comboStyle2;
    int jointIndex;
    bool sliderVelocityPressed;
    bool sliderPositionPressed;
    bool sliderTorquePressed;
    bool sliderOpenloopPressed;
    QString movingSliderStyle;
    bool enableCalib;
    bool speedVisible;
    QTimer velocityTimer;
    double lastVelocity;

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
    double torque;
    double position;
    double speed;



private slots:
    void onModeChanged(int index);
    void onInteractionChanged(int index);
    void onSliderPositionPressed();
    void onSliderPositionReleased();
    void onSliderTorquePressed();
    void onSliderTorqueReleased();
    void onSliderOpenloopPressed();
    void onSliderOpenloopReleased();
    void onSliderVelocityReleased();
    void onSliderVelocityPressed();
    void onSliderMoved(int);
    void onSliderTorqueMoved(int val);
    void onSliderOpenloopMoved(int val);
    void onSliderVelocityMoved(int val);
    void onVelocitySliderMoved(int val);
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


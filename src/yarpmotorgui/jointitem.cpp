/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "jointitem.h"
#include "ui_jointitem.h"

#include "log.h"
#include "yarpmotorgui.h"

#include <cmath>

#include <yarp/os/Log.h>
#include <QDebug>
#include <QKeyEvent>

void JointItem::resetTarget()
{
    ui->sliderTrajectoryPosition->resetTarget();
    ui->sliderMixedPosition->resetTarget();
}

void JointItem::updateTrajectoryPositionTarget(double val)
{
    int w = ui->sliderTrajectoryPosition->width()- 30;
    double mmin = this->min_position;
    double mmax = this->max_position;
    double totValues = fabs(mmax - mmin);
    double cursor = fabs(val - mmin);
    double newX = cursor * (double)w / (double)totValues;
    ui->sliderTrajectoryPosition->updateSliderTarget(newX);
}

void JointItem::updateMixedPositionTarget(double val)
{
    int w = ui->sliderMixedPosition->width() - 30;
    double mmin = this->min_position;
    double mmax = this->max_position;
    double totValues = fabs(mmax - mmin);
    double cursor = fabs(val - mmin);
    double newX = cursor * (double)w / (double)totValues;
    ui->sliderMixedPosition->updateSliderTarget(newX);
}

JointItem::JointItem(int index,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JointItem)
{
    ui->setupUi(this);
    internalState = StateStarting;
    internalInteraction = InteractionStarting;
    jointIndex = index;
    sliderDirectPositionPressed = false;
    sliderMixedPositionPressed = false;
    sliderMixedVelocityPressed = false;
    sliderTrajectoryPositionPressed = false;
    sliderTrajectoryVelocityPressed = false;
    sliderTorquePressed = false;
    sliderPWMPressed = false;
    sliderCurrentPressed = false;
    sliderVelocityPressed = false;
    enableCalib = true;
    joint_speedVisible = false;
    joint_motorPositionVisible = false;
    joint_currentVisible = false;
    joint_dutyVisible = false;
    lastVelocity = 0;
    velocityModeEnabled = false;
    motionDone = true;

    max_position = 0;
    min_position = 0;
    max_velocity = 0;
    min_velocity = 0;
    max_trajectory_velocity = 0;
    max_torque = 0;
    min_torque = 0;
    max_current = 0;
    min_current = 0;
    ref_torque = 0;
    ref_pwm = 0;
    ref_current = 0;

    IDLE            = 0;
    POSITION        = 1;
    POSITION_DIR    = 2;
    MIXED           = 3;
    VELOCITY        = 4;
    TORQUE          = 5;
    PWM             = 6;
    CURRENT         = 7;



    connect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));
    connect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));

    ui->sliderTrajectoryPosition->installEventFilter(this);
    connect(ui->sliderTrajectoryPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderTrajectoryPositionPressed()));
    connect(ui->sliderTrajectoryPosition, SIGNAL(sliderReleased()), this, SLOT(onSliderTrajectoryPositionReleased()));
    ui->sliderTrajectoryPosition->disableClickOutOfHandle=true;
    ui->sliderTrajectoryPosition->enableViewTarget = true;

    ui->sliderTorqueTorque->installEventFilter(this);
    connect(ui->sliderTorqueTorque,SIGNAL(sliderPressed()),this,SLOT(onSliderTorquePressed()));
    connect(ui->sliderTorqueTorque,SIGNAL(sliderReleased()),this,SLOT(onSliderTorqueReleased()));
    ui->sliderTorqueTorque->disableClickOutOfHandle = true;
    ui->sliderTorqueTorque->enableViewTarget = false;

    ui->sliderPWMOutput->installEventFilter(this);
    connect(ui->sliderPWMOutput, SIGNAL(sliderPressed()), this, SLOT(onSliderPWMPressed()));
    connect(ui->sliderPWMOutput, SIGNAL(sliderReleased()), this, SLOT(onSliderPWMReleased()));
    ui->sliderPWMOutput->disableClickOutOfHandle = true;
    ui->sliderPWMOutput->enableViewTarget = false;

    ui->sliderCurrentOutput->installEventFilter(this);
    connect(ui->sliderCurrentOutput, SIGNAL(sliderPressed()), this, SLOT(onSliderCurrentPressed()));
    connect(ui->sliderCurrentOutput, SIGNAL(sliderReleased()), this, SLOT(onSliderCurrentReleased()));
    ui->sliderCurrentOutput->disableClickOutOfHandle = true;
    ui->sliderCurrentOutput->enableViewTarget = false;

    ui->sliderDirectPosition->installEventFilter(this);
    connect(ui->sliderDirectPosition, SIGNAL(sliderPressed()), this, SLOT(onSliderDirectPositionPressed()));
    connect(ui->sliderDirectPosition, SIGNAL(sliderReleased()), this, SLOT(onSliderDirectPositionReleased()));
    ui->sliderDirectPosition->disableClickOutOfHandle = true;
    ui->sliderDirectPosition->enableViewTarget = false;

    ui->sliderMixedPosition->installEventFilter(this);
    connect(ui->sliderMixedPosition, SIGNAL(sliderPressed()), this, SLOT(onSliderMixedPositionPressed()));
    connect(ui->sliderMixedPosition, SIGNAL(sliderReleased()), this, SLOT(onSliderMixedPositionReleased()));
    ui->sliderMixedPosition->disableClickOutOfHandle = true;
    ui->sliderMixedPosition->enableViewTarget = true;

    ui->sliderVelocityVelocity->installEventFilter(this);
    connect(ui->sliderVelocityVelocity,SIGNAL(sliderPressed()),this,SLOT(onSliderVelocityPressed()));
    connect(ui->sliderVelocityVelocity,SIGNAL(sliderReleased()),this,SLOT(onSliderVelocityReleased()));
    ui->sliderVelocityVelocity->disableClickOutOfHandle = true;
    ui->sliderVelocityVelocity->enableViewTarget = false;

    ui->sliderTrajectoryVelocity->installEventFilter(this);
    connect(ui->sliderTrajectoryVelocity, SIGNAL(sliderPressed()), this, SLOT(onSliderTrajectoryVelocityPressed()));
    connect(ui->sliderTrajectoryVelocity, SIGNAL(sliderReleased()), this, SLOT(onSliderTrajectoryVelocityReleased()));
    ui->sliderTrajectoryVelocity->disableClickOutOfHandle = true;
    ui->sliderTrajectoryVelocity->enableViewTarget = false;

    ui->sliderMixedVelocity->installEventFilter(this);
    connect(ui->sliderMixedVelocity, SIGNAL(sliderPressed()), this, SLOT(onSliderMixedVelocityPressed()));
    connect(ui->sliderMixedVelocity, SIGNAL(sliderReleased()), this, SLOT(onSliderMixedVelocityReleased()));
    ui->sliderMixedVelocity->disableClickOutOfHandle = true;
    ui->sliderMixedVelocity->enableViewTarget = false;

    connect(ui->buttonHome,SIGNAL(clicked()),this,SLOT(onHomeClicked()));
    connect(ui->buttonIdle,SIGNAL(clicked()),this,SLOT(onIdleClicked()));
    connect(ui->buttonRun,SIGNAL(clicked()),this,SLOT(onRunClicked()));
    connect(ui->buttonPid,SIGNAL(clicked()),this,SLOT(onPidClicked()));
    connect(ui->buttonCalib,SIGNAL(clicked()),this,SLOT(onCalibClicked()));

    ui->groupBox->setTitle(QString("JOINT %1 (%2)").arg(index).arg(jointName));
    // ui->groupBox->setAlignment(Qt::AlignHCenter);




    comboStyle1 = "QComboBox {"
            "border: 1px solid gray;"
            "border-radius: 3px;"
            "padding: 1px 18px 1px 3px;"
            "min-width: 6em;}"
            "QComboBox:editable {"
            "background: white;"
            "foreground: rgb(35, 38, 41);}"
            "QComboBox::down-arrow {"
            "image: url(:/images/downArrow.png);}";


    comboStyle2 = "QComboBox:on { "
            "padding-top: 3px;"
            "padding-left: 4px;}"
            "QComboBox::drop-down {"
            "subcontrol-origin: padding;"
            "subcontrol-position: top right;"
            "width: 15px;"
            "border-left-width: 1px;"
            "border-left-color: darkgray;"
            "border-left-style: solid; "
            "border-top-right-radius: 3px; "
            "border-bottom-right-radius: 3px;"
            "background-color: darkgray;}";



    installFilter();

    ui->comboInteraction->setItemData(0,QColor(Qt::darkGray), Qt::BackgroundRole);
    ui->comboInteraction->setItemData(1,QColor(0,80,255), Qt::BackgroundRole);
    ui->comboInteraction->setItemData(1,QColor(35, 38, 41), Qt::ForegroundRole);

    ui->comboMode->setItemData( IDLE,           idleColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( POSITION,       positionColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( POSITION_DIR,   positionDirectColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( MIXED,          mixedColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( VELOCITY,       velocityColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( TORQUE,         torqueColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( PWM,            pwmColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( CURRENT,        currentColor, Qt::BackgroundRole);

    ui->comboMode->setItemData( IDLE,           QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( POSITION,       QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( POSITION_DIR,   QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( MIXED,          QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( VELOCITY,       QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( TORQUE,         QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( PWM,            QColor(35, 38, 41), Qt::ForegroundRole );
    ui->comboMode->setItemData( CURRENT,        QColor(35, 38, 41), Qt::ForegroundRole );

    ui->comboMode->setItemData( IDLE,           Idle, Qt::UserRole);
    ui->comboMode->setItemData( POSITION,       Position, Qt::UserRole );
    ui->comboMode->setItemData( POSITION_DIR,   PositionDirect, Qt::UserRole );
    ui->comboMode->setItemData( MIXED,          Mixed, Qt::UserRole );
    ui->comboMode->setItemData( VELOCITY,       Velocity, Qt::UserRole );
    ui->comboMode->setItemData( TORQUE,         Torque, Qt::UserRole );
    ui->comboMode->setItemData( PWM,            Pwm, Qt::UserRole);
    ui->comboMode->setItemData( CURRENT,        Current, Qt::UserRole);

    setJointInternalState(IDLE);

    ui->stackedWidget->widget(VELOCITY)->setEnabled(false);
    velocityTimer.setInterval(50);
    velocityTimer.setSingleShot(false);
    connect(&velocityTimer,SIGNAL(timeout()),this,SLOT(onVelocityTimer()));
}

bool JointItem::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        auto* keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();
        if(key == Qt::Key_Left || key == Qt::Key_Right  || key == Qt::Key_Up ||
           key == Qt::Key_Down || key == Qt::Key_PageUp || key == Qt::Key_PageDown){

            //SliderWithTarget *slider=0;
            QSlider* slider = nullptr;

            if(obj == ui->sliderTrajectoryPosition){
                slider = ui->sliderTrajectoryPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderTrajectoryPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderTrajectoryPositionReleased();
                }
            }
            if(obj == ui->sliderTrajectoryVelocity){
                slider = ui->sliderTrajectoryVelocity;
                if (keyEvent->type() == QEvent::KeyPress){
                    onSliderTrajectoryVelocityPressed();
                }
                if (keyEvent->type() == QEvent::KeyRelease){
                    onSliderTrajectoryVelocityReleased();
                }
            }
            if(obj == ui->sliderDirectPosition){
                slider = ui->sliderDirectPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderDirectPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderDirectPositionReleased();
                }
            }
            if(obj == ui->sliderMixedPosition){
                slider = ui->sliderMixedPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderMixedPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderMixedPositionReleased();
                }
            }
            if(obj == ui->sliderMixedVelocity){
                slider = ui->sliderMixedVelocity;
            }
            if(obj == ui->sliderTorqueTorque){
                slider = ui->sliderTorqueTorque;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderTorquePressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderTorqueReleased();
                }
            }
            if (obj == ui->sliderPWMOutput){
                slider = ui->sliderPWMOutput;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderPWMPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderPWMReleased();
                }
            }
            if (obj == ui->sliderCurrentOutput){
                slider = ui->sliderCurrentOutput;
                if (keyEvent->type() == QEvent::KeyPress){
                    onSliderCurrentPressed();
                }
                if (keyEvent->type() == QEvent::KeyRelease){
                    onSliderCurrentReleased();
                }
            }
            if(obj == ui->sliderVelocityVelocity){
                slider = ui->sliderVelocityVelocity;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderVelocityPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderVelocityReleased();
                }
            }

            if (slider == nullptr) {
                return false;
            }


            if(keyEvent->type() == QEvent::KeyPress){
                if(key == Qt::Key_Left || key == Qt::Key_Down){
                    slider->setValue(slider->value() - 1);
                }
                if(key == Qt::Key_Right || key == Qt::Key_Up){
                    slider->setValue(slider->value() + 1);
                }
                if(key == Qt::Key_PageUp){
                    slider->setValue(slider->value() + 10);
                }
                if(key == Qt::Key_PageDown){
                    slider->setValue(slider->value() - 10);
                }
            }


            return true;
        }
        return QObject::eventFilter(obj, event);

    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void JointItem::enableControlVelocity(bool control)
{
    velocityModeEnabled = control;
    ui->stackedWidget->widget(VELOCITY)->setEnabled(velocityModeEnabled);
    if(ui->stackedWidget->currentIndex() == VELOCITY && velocityModeEnabled){
        velocityTimer.start();
    }
}

void JointItem::enableControlMixed(bool control)
{
    ui->stackedWidget->widget(MIXED)->setEnabled(control);
}

void JointItem::enableControlPositionDirect(bool control)
{
    ui->stackedWidget->widget(POSITION_DIR)->setEnabled(control);
}

void JointItem::enableControlPWM(bool control)
{
    ui->stackedWidget->widget(PWM)->setEnabled(control);
}

void JointItem::enableControlCurrent(bool control)
{
    ui->stackedWidget->widget(CURRENT)->setEnabled(control);
}

void JointItem::viewPositionTarget(bool visible)
{
    ui->sliderTrajectoryPosition->enableViewTarget = visible;
    ui->sliderMixedPosition->enableViewTarget = visible;
}

void JointItem::setUnits(yarp::dev::JointTypeEnum t)
{
    QString pos_metric_revolute("deg");
    QString trq_metric_revolute("Nm");
    QString trq_metric_revolute_title("Torque:");
    QString vel_metric_revolute("deg/s");
    QString pos_metric_prism("m");
    QString trq_metric_prism("N");
    QString trq_metric_prism_title("Force:");
    QString vel_metric_prism("m/s");

    if (t == yarp::dev::VOCAB_JOINTTYPE_REVOLUTE)
    {
        ui->labelIdlevelUnits->setText(vel_metric_revolute);
        ui->labelPositionvelUnits->setText(vel_metric_revolute);
        ui->labelPositionDirvelUnits->setText(vel_metric_revolute);
        ui->labelMixedvelUnits->setText(vel_metric_revolute);
        ui->labelTorquevelUnits->setText(vel_metric_revolute);
        ui->labelPWMvelUnits->setText(vel_metric_revolute);
        ui->labelCurrentvelUnits->setText(vel_metric_revolute);
        ui->labelVelocityvelUnits->setText(vel_metric_revolute);

        ui->labelIdleTorque->setText(trq_metric_revolute_title);
        ui->labelPositionTorque->setText(trq_metric_revolute_title);
        ui->labelPositionDirTorque->setText(trq_metric_revolute_title);
        ui->labelMixedTorque->setText(trq_metric_revolute_title);
        ui->labelTorqueTorque->setText(trq_metric_revolute_title);
        ui->labelPWMTorque->setText(trq_metric_revolute_title);
        ui->labelCurrentTorque->setText(trq_metric_revolute_title);
        ui->labelVelocityTorque->setText(trq_metric_revolute_title);

        ui->labelIdleposUnits->setText(pos_metric_revolute);
        ui->labelPositionposUnits->setText(pos_metric_revolute);
        ui->labelPositionDirposUnits->setText(pos_metric_revolute);
        ui->labelMixedposUnits->setText(pos_metric_revolute);
        ui->labelTorqueposUnits->setText(pos_metric_revolute);
        ui->labelPWMposUnits->setText(pos_metric_revolute);
        ui->labelCurrentposUnits->setText(pos_metric_revolute);
        ui->labelVelocityposUnits->setText(pos_metric_revolute);

        ui->labelIdletrqUnits->setText(trq_metric_revolute);
        ui->labelPositiontrqUnits->setText(trq_metric_revolute);
        ui->labelPositionDirtrqUnits->setText(trq_metric_revolute);
        ui->labelMixedtrqUnits->setText(trq_metric_revolute);
        ui->labelTorquetrqUnits->setText(trq_metric_revolute);
        ui->labelPWMtrqUnits->setText(trq_metric_revolute);
        ui->labelCurrenttrqUnits->setText(pos_metric_revolute);
        ui->labelVelocitytrqUnits->setText(trq_metric_revolute);
    }
    else if (t == yarp::dev::VOCAB_JOINTTYPE_PRISMATIC)
    {
        ui->labelIdlevelUnits->setText(vel_metric_prism);
        ui->labelPositionvelUnits->setText(vel_metric_prism);
        ui->labelPositionDirvelUnits->setText(vel_metric_prism);
        ui->labelMixedvelUnits->setText(vel_metric_prism);
        ui->labelTorquevelUnits->setText(vel_metric_prism);
        ui->labelPWMvelUnits->setText(vel_metric_prism);
        ui->labelCurrentvelUnits->setText(vel_metric_prism);
        ui->labelVelocityvelUnits->setText(vel_metric_prism);

        ui->labelIdleTorque->setText(trq_metric_prism_title);
        ui->labelPositionTorque->setText(trq_metric_prism_title);
        ui->labelPositionDirTorque->setText(trq_metric_prism_title);
        ui->labelMixedTorque->setText(trq_metric_prism_title);
        ui->labelTorqueTorque->setText(trq_metric_prism_title);
        ui->labelPWMTorque->setText(trq_metric_prism_title);
        ui->labelCurrentTorque->setText(trq_metric_prism_title);
        ui->labelVelocityTorque->setText(trq_metric_prism_title);

        ui->labelIdleposUnits->setText(pos_metric_prism);
        ui->labelPositionposUnits->setText(pos_metric_prism);
        ui->labelPositionDirposUnits->setText(pos_metric_prism);
        ui->labelMixedposUnits->setText(pos_metric_prism);
        ui->labelTorqueposUnits->setText(pos_metric_prism);
        ui->labelPWMposUnits->setText(pos_metric_prism);
        ui->labelCurrentposUnits->setText(pos_metric_prism);
        ui->labelVelocityposUnits->setText(pos_metric_prism);

        ui->labelIdletrqUnits->setText(trq_metric_prism);
        ui->labelPositiontrqUnits->setText(trq_metric_prism);
        ui->labelPositionDirtrqUnits->setText(trq_metric_prism);
        ui->labelMixedtrqUnits->setText(trq_metric_prism);
        ui->labelTorquetrqUnits->setText(trq_metric_prism);
        ui->labelPWMtrqUnits->setText(trq_metric_prism);
        ui->labelCurrenttrqUnits->setText(trq_metric_prism);
        ui->labelVelocitytrqUnits->setText(trq_metric_prism);
    }
    else
    {
        yFatal("Unspecified joint type");
    }
}

void JointItem::setMotorPositionVisible(bool visible)
{
    joint_motorPositionVisible = visible;
    ui->editIdleMotorPos->setVisible(visible);
    ui->editPositionMotorPos->setVisible(visible);
    ui->editPositionDirMotorPos->setVisible(visible);
    ui->editMixedMotorPos->setVisible(visible);
    ui->editTorqueMotorPos->setVisible(visible);
    ui->editPWMMotorPos->setVisible(visible);
    ui->editCurrentMotorPos->setVisible(visible);
    ui->editVelocityMotorPos->setVisible(visible);

    ui->labelIdleMotorPos->setVisible(visible);
    ui->labelPositionMotorPos->setVisible(visible);
    ui->labelPositionDirMotorPos->setVisible(visible);
    ui->labelMixedMotorPos->setVisible(visible);
    ui->labelTorqueMotorPos->setVisible(visible);
    ui->labelPWMMotorPos->setVisible(visible);
    ui->labelCurrentMotorPos->setVisible(visible);
    ui->labelVelocityMotorPos->setVisible(visible);

    ui->labelIdleMotorPosUnits->setVisible(visible);
    ui->labelPositionMotorPosUnits->setVisible(visible);
    ui->labelPositionDirMotorPosUnits->setVisible(visible);
    ui->labelMixedMotorPosUnits->setVisible(visible);
    ui->labelTorqueMotorPosUnits->setVisible(visible);
    ui->labelPWMMotorPosUnits->setVisible(visible);
    ui->labelCurrentMotorPosUnits->setVisible(visible);
    ui->labelVelocityMotorPosUnits->setVisible(visible);

    if (!visible) {
        ui->editIdleMotorPos->setMinimumHeight(0);
        ui->editPositionMotorPos->setMinimumHeight(0);
        ui->editPositionDirMotorPos->setMinimumHeight(0);
        ui->editMixedMotorPos->setMinimumHeight(0);
        ui->editTorqueMotorPos->setMinimumHeight(0);
        ui->editPWMMotorPos->setMinimumHeight(0);
        ui->editCurrentMotorPos->setMinimumHeight(0);
        ui->editVelocityMotorPos->setMinimumHeight(0);

        ui->labelPositionMotorPos->setMinimumHeight(0);
        ui->labelPositionMotorPosUnits->setMinimumHeight(0);
        ui->labelPositionDirMotorPos->setMinimumHeight(0);
        ui->labelPositionDirMotorPosUnits->setMinimumHeight(0);
        ui->labelMixedMotorPos->setMinimumHeight(0);
        ui->labelMixedMotorPosUnits->setMinimumHeight(0);
        ui->labelTorqueMotorPos->setMinimumHeight(0);
        ui->labelTorqueMotorPosUnits->setMinimumHeight(0);
        ui->labelPWMMotorPos->setMinimumHeight(0);
        ui->labelPWMMotorPosUnits->setMinimumHeight(0);
        ui->labelCurrentMotorPos->setMinimumHeight(0);
        ui->labelCurrentMotorPosUnits->setMinimumHeight(0);
        ui->labelVelocityMotorPos->setMinimumHeight(0);
        ui->labelVelocityMotorPosUnits->setMinimumHeight(0);
        ui->labelIdleMotorPos->setMinimumHeight(0);
        ui->labelIdleMotorPosUnits->setMinimumHeight(0);
    }
    else {
        ui->editIdleMotorPos->setMinimumHeight(20);
        ui->editPositionMotorPos->setMinimumHeight(20);
        ui->editPositionDirMotorPos->setMinimumHeight(20);
        ui->editMixedMotorPos->setMinimumHeight(20);
        ui->editTorqueMotorPos->setMinimumHeight(20);
        ui->editPWMMotorPos->setMinimumHeight(20);
        ui->editCurrentMotorPos->setMinimumHeight(20);
        ui->editVelocityMotorPos->setMinimumHeight(20);

        ui->labelPositionMotorPos->setMinimumHeight(20);
        ui->labelPositionMotorPosUnits->setMinimumHeight(20);
        ui->labelPositionDirMotorPos->setMinimumHeight(20);
        ui->labelPositionDirMotorPosUnits->setMinimumHeight(20);
        ui->labelMixedMotorPos->setMinimumHeight(20);
        ui->labelMixedMotorPosUnits->setMinimumHeight(20);
        ui->labelTorqueMotorPos->setMinimumHeight(20);
        ui->labelTorqueMotorPosUnits->setMinimumHeight(20);
        ui->labelPWMMotorPos->setMinimumHeight(20);
        ui->labelPWMMotorPosUnits->setMinimumHeight(20);
        ui->labelCurrentMotorPos->setMinimumHeight(20);
        ui->labelCurrentMotorPosUnits->setMinimumHeight(20);
        ui->labelVelocityMotorPos->setMinimumHeight(20);
        ui->labelVelocityMotorPosUnits->setMinimumHeight(20);
        ui->labelIdleMotorPos->setMinimumHeight(20);
        ui->labelIdleMotorPosUnits->setMinimumHeight(20);
    }
}

void JointItem::setDutyVisible(bool visible)
{
    joint_dutyVisible = visible;
    //ui->editIdleDuty->setVisible(visible);
    //ui->editPositionDuty->setVisible(visible);
    //ui->editPositionDirDuty->setVisible(visible);
   // ui->editMixedDuty->setVisible(visible);
    ui->editTorqueDuty->setVisible(visible);
    ui->editCurrentDuty->setVisible(visible);
    //ui->editPWMDuty->setVisible(visible);
    //ui->editDutyDuty->setVisible(visible);
    //ui->editVelocityDuty->setVisible(visible);

    //ui->labelIdleDuty->setVisible(visible);
    //ui->labelIdleDutyUnits->setVisible(visible);
    //ui->labelPositionDuty->setVisible(visible);
    //ui->labelPositionDutyUnits->setVisible(visible);
    //ui->labelPositionDirDuty->setVisible(visible);
    //ui->labelPositionDirDutyUnits->setVisible(visible);
    //ui->labelMixedDuty->setVisible(visible);
    //ui->labelMixedDutyUnits->setVisible(visible);
    ui->labelTorqueDuty->setVisible(visible);
    ui->labelTorqueDutyUnits->setVisible(visible);
    //ui->labelPWMDuty->setVisible(visible);
    ui->labelCurrentDuty->setVisible(visible);
    //ui->labelPWMDutyUnits->setVisible(visible);
    ui->labelCurrentDutyUnits->setVisible(visible);
    //ui->labelVelocityDuty->setVisible(visible);
    //ui->labelVelocityDutyUnits->setVisible(visible);


    if (!visible) {
        //ui->editIdleDuty->setMinimumHeight(0);
        //ui->editPositionDuty->setMinimumHeight(0);
        //ui->editPositionDirDuty->setMinimumHeight(0);
        //ui->editMixedDuty->setMinimumHeight(0);
        //ui->editTorqueDuty->setMinimumHeight(0);
        //ui->editPWMDuty->setMinimumHeight(0);
        //ui->editDutyDuty->setMinimumHeight(0);
        //ui->editVelocityDuty->setMinimumHeight(0);

        //ui->labelPositionDuty->setMinimumHeight(0);
        //ui->labelPositionDutyUnits->setMinimumHeight(0);
        //ui->labelPositionDirDuty->setMinimumHeight(0);
        //ui->labelPositionDirDutyUnits->setMinimumHeight(0);
        //ui->labelMixedDuty->setMinimumHeight(0);
        //ui->labelMixedDutyUnits->setMinimumHeight(0);
        ui->labelTorqueDuty->setMinimumHeight(0);
        ui->labelTorqueDutyUnits->setMinimumHeight(0);
        //ui->labelPWMDuty->setMinimumHeight(0);
        //ui->labelPWMDutyUnits->setMinimumHeight(0);
        ui->labelCurrentDuty->setMinimumHeight(0);
        ui->labelCurrentDutyUnits->setMinimumHeight(0);
        //ui->labelVelocityDuty->setMinimumHeight(0);
        //ui->labelVelocityDutyUnits->setMinimumHeight(0);
        //ui->labelIdleDuty->setMinimumHeight(0);
        //ui->labelIdleDutyUnits->setMinimumHeight(0);
    }
    else {
        //ui->editIdleDuty->setMinimumHeight(20);
        //ui->editPositionDuty->setMinimumHeight(20);
        //ui->editPositionDirDuty->setMinimumHeight(20);
        //ui->editMixedDuty->setMinimumHeight(20);
        ui->editTorqueDuty->setMinimumHeight(20);
        //ui->editPWMDuty->setMinimumHeight(20);
        ui->editCurrentDuty->setMinimumHeight(20);
        //ui->editVelocityDuty->setMinimumHeight(20);

        //ui->labelPositionDuty->setMinimumHeight(20);
        //ui->labelPositionDutyUnits->setMinimumHeight(20);
        //ui->labelPositionDirDuty->setMinimumHeight(20);
        //ui->labelPositionDirDutyUnits->setMinimumHeight(20);
        //ui->labelMixedDuty->setMinimumHeight(20);
        //ui->labelMixedDutyUnits->setMinimumHeight(20);
        ui->labelTorqueDuty->setMinimumHeight(20);
        ui->labelTorqueDutyUnits->setMinimumHeight(20);
        //ui->labelPWMDuty->setMinimumHeight(20);
        //ui->labelPWMDutyUnits->setMinimumHeight(20);
        ui->labelCurrentDuty->setMinimumHeight(20);
        ui->labelCurrentDutyUnits->setMinimumHeight(20);
        //ui->labelVelocityDuty->setMinimumHeight(20);
        //ui->labelVelocityDutyUnits->setMinimumHeight(20);
        //ui->labelIdleDuty->setMinimumHeight(20);
        //ui->labelIdleDutyUnits->setMinimumHeight(20);
    }
}

void JointItem::setCurrentsVisible(bool visible)
{
    joint_currentVisible = visible;
    ui->editIdleCurrent->setVisible(visible);
    ui->editPositionCurrent->setVisible(visible);
    ui->editPositionDirCurrent->setVisible(visible);
    ui->editMixedCurrent->setVisible(visible);
    ui->editTorqueCurrent->setVisible(visible);
    ui->editPWMCurrent->setVisible(visible);
    //ui->editCurrentCurrent->setVisible(visible);
    ui->editVelocityCurrent->setVisible(visible);

    ui->labelIdleCurrent->setVisible(visible);
    ui->labelIdleCurrentUnits->setVisible(visible);
    ui->labelPositionCurrent->setVisible(visible);
    ui->labelPositionCurrentUnits->setVisible(visible);
    ui->labelPositionDirCurrent->setVisible(visible);
    ui->labelPositionDirCurrentUnits->setVisible(visible);
    ui->labelMixedCurrent->setVisible(visible);
    ui->labelMixedCurrentUnits->setVisible(visible);
    ui->labelTorqueCurrent->setVisible(visible);
    ui->labelTorqueCurrentUnits->setVisible(visible);
    ui->labelPWMCurrent->setVisible(visible);
    //ui->labelCurrentCurrent->setVisible(visible);
    ui->labelPWMCurrentUnits->setVisible(visible);
    //ui->labelCurrentCurrentUnits->setVisible(visible);
    ui->labelVelocityCurrent->setVisible(visible);
    ui->labelVelocityCurrentUnits->setVisible(visible);


    if (!visible) {
        ui->editIdleCurrent->setMinimumHeight(0);
        ui->editPositionCurrent->setMinimumHeight(0);
        ui->editPositionDirCurrent->setMinimumHeight(0);
        ui->editMixedCurrent->setMinimumHeight(0);
        ui->editTorqueCurrent->setMinimumHeight(0);
        ui->editPWMCurrent->setMinimumHeight(0);
        //ui->editCurrentCurrent->setMinimumHeight(0);
        ui->editVelocityCurrent->setMinimumHeight(0);

        ui->labelPositionCurrent->setMinimumHeight(0);
        ui->labelPositionCurrentUnits->setMinimumHeight(0);
        ui->labelPositionDirCurrent->setMinimumHeight(0);
        ui->labelPositionDirCurrentUnits->setMinimumHeight(0);
        ui->labelMixedCurrent->setMinimumHeight(0);
        ui->labelMixedCurrentUnits->setMinimumHeight(0);
        ui->labelTorqueCurrent->setMinimumHeight(0);
        ui->labelTorqueCurrentUnits->setMinimumHeight(0);
        ui->labelPWMCurrent->setMinimumHeight(0);
        ui->labelPWMCurrentUnits->setMinimumHeight(0);
        //ui->labelCurrentCurrent->setMinimumHeight(0);
        //ui->labelCurrentCurrentUnits->setMinimumHeight(0);
        ui->labelVelocityCurrent->setMinimumHeight(0);
        ui->labelVelocityCurrentUnits->setMinimumHeight(0);
        ui->labelIdleCurrent->setMinimumHeight(0);
        ui->labelIdleCurrentUnits->setMinimumHeight(0);
    }
    else {
        ui->editIdleCurrent->setMinimumHeight(20);
        ui->editPositionCurrent->setMinimumHeight(20);
        ui->editPositionDirCurrent->setMinimumHeight(20);
        ui->editMixedCurrent->setMinimumHeight(20);
        ui->editTorqueCurrent->setMinimumHeight(20);
        ui->editPWMCurrent->setMinimumHeight(20);
        //ui->editCurrentCurrent->setMinimumHeight(20);
        ui->editVelocityCurrent->setMinimumHeight(20);

        ui->labelPositionCurrent->setMinimumHeight(20);
        ui->labelPositionCurrentUnits->setMinimumHeight(20);
        ui->labelPositionDirCurrent->setMinimumHeight(20);
        ui->labelPositionDirCurrentUnits->setMinimumHeight(20);
        ui->labelMixedCurrent->setMinimumHeight(20);
        ui->labelMixedCurrentUnits->setMinimumHeight(20);
        ui->labelTorqueCurrent->setMinimumHeight(20);
        ui->labelTorqueCurrentUnits->setMinimumHeight(20);
        ui->labelPWMCurrent->setMinimumHeight(20);
        ui->labelPWMCurrentUnits->setMinimumHeight(20);
        //ui->labelCurrentCurrent->setMinimumHeight(20);
        //ui->labelCurrentCurrentUnits->setMinimumHeight(20);
        ui->labelVelocityCurrent->setMinimumHeight(20);
        ui->labelVelocityCurrentUnits->setMinimumHeight(20);
        ui->labelIdleCurrent->setMinimumHeight(20);
        ui->labelIdleCurrentUnits->setMinimumHeight(20);
    }
}

void JointItem::setSpeedVisible(bool visible)
{
    joint_speedVisible = visible;
    ui->editIdleSpeed->setVisible(visible);
    ui->editPositionSpeed->setVisible(visible);
    ui->editPositionDirSpeed->setVisible(visible);
    ui->editMixedSpeed->setVisible(visible);
    ui->editTorqueSpeed->setVisible(visible);
    ui->editPWMSpeed->setVisible(visible);
    ui->editCurrentSpeed->setVisible(visible);
    ui->editVelocitySpeed->setVisible(visible);

    ui->labelIdleSpeed->setVisible(visible);
    ui->labelIdlevelUnits->setVisible(visible);
    ui->labelPositionSpeed->setVisible(visible);
    ui->labelPositionvelUnits->setVisible(visible);
    ui->labelPositionDirSpeed->setVisible(visible);
    ui->labelPositionDirvelUnits->setVisible(visible);
    ui->labelMixedSpeed->setVisible(visible);
    ui->labelMixedvelUnits->setVisible(visible);
    ui->labelTorqueSpeed->setVisible(visible);
    ui->labelTorquevelUnits->setVisible(visible);
    ui->labelPWMSpeed->setVisible(visible);
    ui->labelCurrentSpeed->setVisible(visible);
    ui->labelPWMvelUnits->setVisible(visible);
    ui->labelCurrentvelUnits->setVisible(visible);
    ui->labelVelocitySpeed->setVisible(visible);
    ui->labelVelocityvelUnits->setVisible(visible);


    if(!visible){
        ui->editIdleSpeed->setMinimumHeight(0);
        ui->editPositionSpeed->setMinimumHeight(0);
        ui->editPositionDirSpeed->setMinimumHeight(0);
        ui->editMixedSpeed->setMinimumHeight(0);
        ui->editTorqueSpeed->setMinimumHeight(0);
        ui->editPWMSpeed->setMinimumHeight(0);
        ui->editCurrentSpeed->setMinimumHeight(0);
        ui->editVelocitySpeed->setMinimumHeight(0);

        ui->labelPositionSpeed->setMinimumHeight(0);
        ui->labelPositionvelUnits->setMinimumHeight(0);
        ui->labelPositionDirSpeed->setMinimumHeight(0);
        ui->labelPositionDirvelUnits->setMinimumHeight(0);
        ui->labelMixedSpeed->setMinimumHeight(0);
        ui->labelMixedvelUnits->setMinimumHeight(0);
        ui->labelTorqueSpeed->setMinimumHeight(0);
        ui->labelTorquevelUnits->setMinimumHeight(0);
        ui->labelPWMSpeed->setMinimumHeight(0);
        ui->labelPWMvelUnits->setMinimumHeight(0);
        ui->labelCurrentSpeed->setMinimumHeight(0);
        ui->labelCurrentvelUnits->setMinimumHeight(0);
        ui->labelVelocitySpeed->setMinimumHeight(0);
        ui->labelVelocityvelUnits->setMinimumHeight(0);
        ui->labelIdleSpeed->setMinimumHeight(0);
        ui->labelIdlevelUnits->setMinimumHeight(0);
    }else{
        ui->editIdleSpeed->setMinimumHeight(20);
        ui->editPositionSpeed->setMinimumHeight(20);
        ui->editPositionDirSpeed->setMinimumHeight(20);
        ui->editMixedSpeed->setMinimumHeight(20);
        ui->editTorqueSpeed->setMinimumHeight(20);
        ui->editPWMSpeed->setMinimumHeight(20);
        ui->editCurrentSpeed->setMinimumHeight(20);
        ui->editVelocitySpeed->setMinimumHeight(20);

        ui->labelPositionSpeed->setMinimumHeight(20);
        ui->labelPositionvelUnits->setMinimumHeight(20);
        ui->labelPositionDirSpeed->setMinimumHeight(20);
        ui->labelPositionDirvelUnits->setMinimumHeight(20);
        ui->labelMixedSpeed->setMinimumHeight(20);
        ui->labelMixedvelUnits->setMinimumHeight(20);
        ui->labelTorqueSpeed->setMinimumHeight(20);
        ui->labelTorquevelUnits->setMinimumHeight(20);
        ui->labelPWMSpeed->setMinimumHeight(20);
        ui->labelPWMvelUnits->setMinimumHeight(20);
        ui->labelCurrentSpeed->setMinimumHeight(20);
        ui->labelCurrentvelUnits->setMinimumHeight(20);
        ui->labelVelocitySpeed->setMinimumHeight(20);
        ui->labelVelocityvelUnits->setMinimumHeight(20);
        ui->labelIdleSpeed->setMinimumHeight(20);
        ui->labelIdlevelUnits->setMinimumHeight(20);
    }
}

void JointItem::enablePositionSliderDoubleAuto()
{
    double positionSliderStep = 1 / (fabs(min_position - max_position) / 100.0);
    ui->sliderMixedPosition->setSliderStep(positionSliderStep);
    ui->sliderTrajectoryPosition->setSliderStep(positionSliderStep);
    ui->sliderDirectPosition->setSliderStep(positionSliderStep);
    ui->sliderMixedPosition->setIsDouble(true);
    ui->sliderTrajectoryPosition->setIsDouble(true);
    ui->sliderDirectPosition->setIsDouble(true);
    int sliderMin = min_position*positionSliderStep;
    int sliderMax = max_position*positionSliderStep;
    ui->sliderMixedPosition->setRange(sliderMin, sliderMax);
    ui->sliderTrajectoryPosition->setRange(sliderMin, sliderMax);
    ui->sliderDirectPosition->setRange(sliderMin, sliderMax);
    ui->sliderMixedPosition->resetTarget();
    ui->sliderTrajectoryPosition->resetTarget();
    ui->sliderDirectPosition->resetTarget();
}

void JointItem::enablePositionSliderDoubleValue(double value)
{
    double positionSliderStep = 1 / value;
    ui->sliderMixedPosition->setSliderStep(positionSliderStep);
    ui->sliderTrajectoryPosition->setSliderStep(positionSliderStep);
    ui->sliderDirectPosition->setSliderStep(positionSliderStep);
    ui->sliderMixedPosition->setIsDouble(true);
    ui->sliderTrajectoryPosition->setIsDouble(true);
    ui->sliderDirectPosition->setIsDouble(true);
    int sliderMin = min_position*positionSliderStep;
    int sliderMax = max_position*positionSliderStep;
    ui->sliderMixedPosition->setRange(sliderMin, sliderMax);
    ui->sliderTrajectoryPosition->setRange(sliderMin, sliderMax);
    ui->sliderDirectPosition->setRange(sliderMin, sliderMax);
    ui->sliderMixedPosition->resetTarget();
    ui->sliderTrajectoryPosition->resetTarget();
    ui->sliderDirectPosition->resetTarget();
}

void JointItem::disablePositionSliderDouble()
{
    if (fabs(max_position - min_position) < 1.0)
    {
        yError("Unable to set integer position slider");
        return;
    }
    double positionSliderStep = 1;
    ui->sliderMixedPosition->setSliderStep(positionSliderStep);
    ui->sliderTrajectoryPosition->setSliderStep(positionSliderStep);
    ui->sliderDirectPosition->setSliderStep(positionSliderStep);
    ui->sliderMixedPosition->setIsDouble(false);
    ui->sliderTrajectoryPosition->setIsDouble(false);
    ui->sliderDirectPosition->setIsDouble(false);
    int sliderMin = min_position;
    int sliderMax = max_position;
    ui->sliderMixedPosition->setRange(sliderMin, sliderMax);
    ui->sliderTrajectoryPosition->setRange(sliderMin, sliderMax);
    ui->sliderDirectPosition->setRange(sliderMin, sliderMax);
    ui->sliderMixedPosition->resetTarget();
    ui->sliderTrajectoryPosition->resetTarget();
    ui->sliderDirectPosition->resetTarget();
}

void JointItem::enableVelocitySliderDoubleAuto()
{
    double velocitySliderStep = 1 / (fabs(-max_velocity - max_velocity) / 100.0); //note that we are using -max_velocity
    ui->sliderVelocityVelocity->setSliderStep(velocitySliderStep);
    ui->sliderVelocityVelocity->setIsDouble(true);
    int sliderMin = -max_velocity*velocitySliderStep; //note that we are using -max_velocity
    int sliderMax = max_velocity*velocitySliderStep;
    ui->sliderVelocityVelocity->setRange(sliderMin, sliderMax);
    int v = ui->sliderVelocityVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefVelocitySpeed(ref_speed);
}

void JointItem::enableVelocitySliderDoubleValue(double value)
{
    double velocitySliderStep = 1 / value;
    ui->sliderVelocityVelocity->setSliderStep(velocitySliderStep);
    ui->sliderVelocityVelocity->setIsDouble(true);
    int sliderMin = -max_velocity*velocitySliderStep; //note that we are using -max_velocity
    int sliderMax = max_velocity*velocitySliderStep;
    ui->sliderVelocityVelocity->setRange(sliderMin, sliderMax);
    int v = ui->sliderVelocityVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefVelocitySpeed(ref_speed);
}

void JointItem::disableVelocitySliderDouble()
{
    if (fabs(max_velocity) < 1.0)
    {
        yError("Unable to set integer velocity slider");
        return;
    }
    double  velocitySliderStep = 1;
    ui->sliderVelocityVelocity->setSliderStep(velocitySliderStep);
    ui->sliderVelocityVelocity->setIsDouble(false);
    int sliderMin = -max_velocity; //note that we are using -max_velocity
    int sliderMax = max_velocity;
    ui->sliderVelocityVelocity->setRange(sliderMin, sliderMax);
    int v = ui->sliderVelocityVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefVelocitySpeed(ref_speed);
}

void JointItem::enableTorqueSliderDoubleAuto()
{
    double torqueSliderStep = 1 / (fabs(-max_torque - max_torque) / 100.0); //note that we are using -max_velocity
    ui->sliderTorqueTorque->setSliderStep(torqueSliderStep);
    ui->sliderTorqueTorque->setIsDouble(true);
    int sliderMin = -max_torque*torqueSliderStep; //note that we are using -max_torque
    int sliderMax = max_torque*torqueSliderStep;
    ui->sliderTorqueTorque->setRange(sliderMin, sliderMax);
    int v = ui->sliderTorqueTorque->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTorque(ref_torque);
}

void JointItem::enableTorqueSliderDoubleValue(double value)
{
    double torqueSliderStep = 1 / value;
    ui->sliderTorqueTorque->setSliderStep(torqueSliderStep);
    ui->sliderTorqueTorque->setIsDouble(true);
    int sliderMin = -max_torque*torqueSliderStep; //note that we are using -max_torque
    int sliderMax = max_torque*torqueSliderStep;
    ui->sliderTorqueTorque->setRange(sliderMin, sliderMax);
    int v = ui->sliderTorqueTorque->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTorque(ref_torque);
}

void JointItem::disableTorqueSliderDouble()
{
    if (fabs(max_torque) < 1.0)
    {
        yError("Unable to set integer torque slider");
        return;
    }
    double torqueSliderStep = 1;
    ui->sliderTorqueTorque->setSliderStep(torqueSliderStep);
    ui->sliderTorqueTorque->setIsDouble(false);
    int sliderMin = -max_torque; //note that we are using -max_torque
    int sliderMax = max_torque;
    ui->sliderTorqueTorque->setRange(sliderMin, sliderMax);
    int v = ui->sliderTorqueTorque->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTorque(ref_torque);
}

void JointItem::enableCurrentSliderDoubleAuto()
{
    double currentSliderStep = 1 / (fabs(-max_current - max_current) / 100.0); //note that we are using -max_velocity
    ui->sliderCurrentOutput->setSliderStep(currentSliderStep);
    ui->sliderCurrentOutput->setIsDouble(true);
    int sliderMin = -max_current*currentSliderStep; //note that we are using -max_current
    int sliderMax = max_current*currentSliderStep;
    ui->sliderCurrentOutput->setRange(sliderMin, sliderMax);
    int v = ui->sliderCurrentOutput->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefCurrent(ref_current);
}

void JointItem::enableCurrentSliderDoubleValue(double value)
{
    double currentSliderStep = 1 / value;
    ui->sliderCurrentOutput->setSliderStep(currentSliderStep);
    ui->sliderCurrentOutput->setIsDouble(true);
    int sliderMin = -max_current*currentSliderStep; //note that we are using -max_current
    int sliderMax = max_current*currentSliderStep;
    ui->sliderCurrentOutput->setRange(sliderMin, sliderMax);
    int v = ui->sliderCurrentOutput->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefCurrent(ref_current);
}

void JointItem::disableCurrentSliderDouble()
{
    if (fabs(max_velocity) < 1.0)
    {
        yError("Unable to set integer velocity slider");
        return;
    }
    double currentSliderStep = 1;
    ui->sliderCurrentOutput->setSliderStep(currentSliderStep);
    ui->sliderCurrentOutput->setIsDouble(false);
    int sliderMin = -max_current; //note that we are using -max_current
    int sliderMax = max_current;
    ui->sliderCurrentOutput->setRange(sliderMin, sliderMax);
    int v = ui->sliderCurrentOutput->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefCurrent(ref_current);
}

void JointItem::enableTrajectoryVelocitySliderDoubleAuto()
{
    double trajectoryVelocitySliderStep = 1 / (fabs(0 - max_trajectory_velocity) / 100.0); //note that we are using 0
    ui->sliderTrajectoryVelocity->setSliderStep(trajectoryVelocitySliderStep);
    ui->sliderTrajectoryVelocity->setIsDouble(true);
    int sliderMin = 0 * trajectoryVelocitySliderStep; //note that we are using 0
    int sliderMax = max_trajectory_velocity*trajectoryVelocitySliderStep;
    ui->sliderTrajectoryVelocity->setRange(sliderMin, sliderMax);
    int v = ui->sliderTrajectoryVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTrajectorySpeed(ref_trajectory_velocity);
}

void JointItem::enableTrajectoryVelocitySliderDoubleValue(double value)
{
    double trajectoryVelocitySliderStep = 1 / value;
    ui->sliderTrajectoryVelocity->setSliderStep(trajectoryVelocitySliderStep);
    ui->sliderTrajectoryVelocity->setIsDouble(true);
    int sliderMin = 0 * trajectoryVelocitySliderStep; //note that we are using 0
    int sliderMax = max_trajectory_velocity*trajectoryVelocitySliderStep;
    ui->sliderTrajectoryVelocity->setRange(sliderMin, sliderMax);
    int v = ui->sliderTrajectoryVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTrajectorySpeed(ref_trajectory_velocity);
}

void JointItem::disableTrajectoryVelocitySliderDouble()
{
    int sliderMin = 0; //note that we are using 0
    int sliderMax = max_trajectory_velocity;
    double trajectoryVelocitySliderStep = 1;
    ui->sliderTrajectoryVelocity->setSliderStep(trajectoryVelocitySliderStep);
    ui->sliderTrajectoryVelocity->setRange(sliderMin, sliderMax);
    ui->sliderTrajectoryVelocity->setIsDouble(false);
    int v = ui->sliderTrajectoryVelocity->value();
    if (v > sliderMax) {}
    if (v < sliderMin) {}
    setRefTrajectorySpeed(ref_trajectory_velocity);
}

void JointItem::setEnabledOptions(bool debug_param_enabled,
    bool speedview_param_enabled,
    bool enable_calib_all)
{
    Q_UNUSED(debug_param_enabled);
    Q_UNUSED(speedview_param_enabled);

    enableCalib = enable_calib_all;
    if (!enable_calib_all){
        ui->buttonCalib->setEnabled(false);
    }

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetChanged(int)));
}

JointItem::~JointItem()
{
    disconnect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));
    disconnect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));

    disconnect(ui->sliderTrajectoryPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderTrajectoryPositionPressed()));
    disconnect(ui->sliderTrajectoryPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderTrajectoryPositionReleased()));

    disconnect(ui->sliderTorqueTorque,SIGNAL(sliderPressed()),this,SLOT(onSliderTorquePressed()));
    disconnect(ui->sliderTorqueTorque,SIGNAL(sliderReleased()),this,SLOT(onSliderTorqueReleased()));

    disconnect(ui->sliderPWMOutput, SIGNAL(sliderPressed()), this, SLOT(onSliderPWMPressed()));
    disconnect(ui->sliderPWMOutput, SIGNAL(sliderReleased()), this, SLOT(onSliderPWMReleased()));

    disconnect(ui->sliderCurrentOutput, SIGNAL(sliderPressed()), this, SLOT(onSliderCurrentPressed()));
    disconnect(ui->sliderCurrentOutput, SIGNAL(sliderReleased()), this, SLOT(onSliderCurrentPressed()));

    disconnect(ui->sliderDirectPosition, SIGNAL(sliderPressed()), this, SLOT(onSliderDirectPositionPressed()));
    disconnect(ui->sliderDirectPosition, SIGNAL(sliderReleased()), this, SLOT(onSliderDirectPositionReleased()));

    disconnect(ui->sliderMixedPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderMixedPositionPressed()));
    disconnect(ui->sliderMixedPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderMixedPositionReleased()));

    disconnect(ui->sliderTrajectoryVelocity, SIGNAL(sliderPressed()), this, SLOT(onSliderTrajectoryVelocityPressed()));
    disconnect(ui->sliderMixedVelocity, SIGNAL(sliderReleased()), this, SLOT(onSliderMixedVelocityReleased()));

    disconnect(ui->buttonHome,SIGNAL(clicked()),this,SLOT(onHomeClicked()));
    disconnect(ui->buttonIdle,SIGNAL(clicked()),this,SLOT(onIdleClicked()));
    disconnect(ui->buttonRun,SIGNAL(clicked()),this,SLOT(onRunClicked()));
    disconnect(ui->buttonPid,SIGNAL(clicked()),this,SLOT(onPidClicked()));
    disconnect(ui->buttonCalib,SIGNAL(clicked()),this,SLOT(onCalibClicked()));
    delete ui;
}

int JointItem::getJointIndex()
{
    return jointIndex;
}

void JointItem::installFilter()
{
    auto* filter = new WheelEventFilter();
    ui->comboMode->installEventFilter(filter);
    ui->comboInteraction->installEventFilter(filter);

    ui->sliderMixedPosition->installEventFilter(filter);
    ui->sliderMixedVelocity->installEventFilter(filter);
    ui->sliderDirectPosition->installEventFilter(filter);
    ui->sliderTrajectoryPosition->installEventFilter(filter);
    ui->sliderTrajectoryVelocity->installEventFilter(filter);
    ui->sliderTorqueTorque->installEventFilter(filter);
}


void JointItem::onStackedWidgetChanged(int index)
{
    if(index == VELOCITY){
        if(velocityModeEnabled){
            velocityTimer.start();
        }
    }else{
        if(velocityModeEnabled)
        {
            velocityTimer.stop();
            lastVelocity = 0;
            updateSliderVelocity(0);
        }
    }
}

void JointItem::onModeChanged(int index)
{
    if (this->internalState == Velocity){
       velocityTimer.stop();
    }
    Q_UNUSED(index);
    int mode = ui->comboMode->currentData(Qt::UserRole).toInt();
    emit changeMode(mode,this);
}


void JointItem::onInteractionChanged(int index)
{
    emit changeInteraction(index,this);
}

void JointItem::setJointName(QString name)
{
    jointName = name;
}

QString JointItem::getJointName()
{
    return jointName;
}

void JointItem::onSliderVelocityPressed()
{
    sliderVelocityPressed = true;
}

void JointItem::onSliderVelocityReleased()
{
    lastVelocity = (double)ui->sliderVelocityVelocity->value() / ui->sliderVelocityVelocity->getSliderStep();
    sliderVelocityPressed = false;
}

void JointItem::onSliderMixedVelocityPressed()
{
    sliderMixedVelocityPressed = true;
}

void JointItem::onSliderMixedVelocityReleased()
{
    lastVelocity = ui->sliderMixedVelocity->value();

    if (ui->sliderMixedVelocity->getIsDouble())
    {
        double val = ui->sliderMixedVelocity->value();
        emit sliderMixedVelocityCommand(val / ui->sliderMixedVelocity->getSliderStep(), jointIndex);
    }
    else
    {
        emit sliderMixedVelocityCommand(ui->sliderMixedVelocity->value(), jointIndex);
    }

    sliderMixedVelocityPressed = false;
}


void JointItem::onVelocityTimer()
{
    if(velocityModeEnabled)
    {
        emit sliderVelocityCommand(lastVelocity,jointIndex);
    }
}

void JointItem::onSliderTrajectoryPositionPressed()
{
    sliderTrajectoryPositionPressed = true;
}

void JointItem::onSliderTrajectoryPositionReleased()
{
    if (ui->sliderTrajectoryPosition->getIsDouble())
    {
        double val = ui->sliderTrajectoryPosition->value();
        emit sliderTrajectoryPositionCommand(val / ui->sliderTrajectoryPosition->getSliderStep(), jointIndex);
        updateTrajectoryPositionTarget(val / ui->sliderTrajectoryPosition->getSliderStep());
    }
    else
    {
        double val = ui->sliderTrajectoryPosition->value();
        emit sliderTrajectoryPositionCommand(val, jointIndex);
        updateTrajectoryPositionTarget(val);
    }
    sliderTrajectoryPositionPressed = false;
    motionDone = false;
}

void JointItem::onSliderMixedPositionPressed()
{
    sliderMixedPositionPressed = true;
}

void JointItem::onSliderMixedPositionReleased()
{
    if (ui->sliderMixedPosition->getIsDouble())
    {
        double val = ui->sliderMixedPosition->value();
        emit sliderMixedPositionCommand(val / ui->sliderMixedPosition->getSliderStep(), jointIndex);
        updateMixedPositionTarget(val / ui->sliderMixedPosition->getSliderStep());
    }
    else
    {
        double val = ui->sliderMixedPosition->value();
        emit sliderMixedPositionCommand(val, jointIndex);
        updateMixedPositionTarget(val);
    }

    sliderMixedPositionPressed = false;
    motionDone = false;
}

void JointItem::onSliderDirectPositionPressed()
{
    sliderDirectPositionPressed = true;
}

void JointItem::onSliderDirectPositionReleased()
{
    double ref_direct_position = (double)ui->sliderDirectPosition->value() / ui->sliderDirectPosition->getSliderStep();
    emit sliderDirectPositionCommand(ref_direct_position, jointIndex);
    sliderDirectPositionPressed = false;
    motionDone = false;
}

void JointItem::onSliderTrajectoryVelocityPressed()
{
    sliderTrajectoryVelocityPressed = true;
}

void JointItem::onSliderTrajectoryVelocityReleased()
{
    ref_trajectory_velocity = (double)ui->sliderTrajectoryVelocity->value() / ui->sliderTrajectoryVelocity->getSliderStep();
    emit sliderTrajectoryVelocityCommand(ref_trajectory_velocity, jointIndex);
    sliderTrajectoryVelocityPressed = false;
}

void JointItem::onSliderPWMPressed()
{
    sliderPWMPressed = true;
}

void JointItem::onSliderPWMReleased()
{
    ref_pwm = (double)ui->sliderPWMOutput->value() / ui->sliderPWMOutput->getSliderStep();
    emit sliderPWMCommand(ref_pwm, jointIndex);
    sliderPWMPressed = false;
}

void JointItem::onSliderCurrentPressed()
{
    sliderCurrentPressed = true;
}

void JointItem::onSliderCurrentReleased()
{
    ref_current = (double)ui->sliderCurrentOutput->value() / ui->sliderCurrentOutput->getSliderStep();
    emit sliderCurrentCommand(ref_current, jointIndex);
    sliderCurrentPressed = false;
}

void JointItem::onSliderTorquePressed()
{
    sliderTorquePressed = true;
}

void JointItem::onSliderTorqueReleased()
{
    ref_torque = (double)ui->sliderTorqueTorque->value() / ui->sliderTorqueTorque->getSliderStep();
    emit sliderTorqueCommand(ref_torque, jointIndex);
    sliderTorquePressed = false;
}


double JointItem::getTrajectoryPositionValue()
{
    //this function is mainly used used by the sequencer
    double pos = (double)ui->sliderTrajectoryPosition->value() / ui->sliderTrajectoryPosition->getSliderStep();
    if (fabs(pos) < 1e-6) {
        pos = 0;
    }
    return pos;
}

double JointItem::getTrajectoryVelocityValue()
{
    //this function is mainly used used by the sequencer
    double vel = (double)ui->sliderTrajectoryVelocity->value() / ui->sliderTrajectoryVelocity->getSliderStep();
    if (fabs(vel) < 1e-6) {
        vel = 0;
    }
    return vel;
}

void JointItem::updateMotionDone(bool done)
{
    motionDone = done;
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        if(!done){
            ui->editPositionJointPos->setStyleSheet("background-color: rgb(255, 38, 41); color: rgb(35, 38, 41);");
        }else{
            ui->editPositionJointPos->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(35, 38, 41);");
        }
    } else if (index == POSITION_DIR) {
        if(!done){
            ui->editPositionDirJointPos->setStyleSheet("background-color: rgb(255, 38, 41); color: rgb(35, 38, 41);");
        }else{
            ui->editPositionDirJointPos->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(35, 38, 41);");
        }
    } else if (index == MIXED) {
        if(!done){
            ui->editMixedJointPos->setStyleSheet("background-color: rgb(255, 38, 41); color: rgb(35, 38, 41);");
        }else{
            ui->editMixedJointPos->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(35, 38, 41);");
        }
    }
}

void JointItem::updateSliderPosition(SliderWithTarget *slider, double val)
{
    if(sliderTrajectoryPositionPressed ||
        sliderMixedPositionPressed ||
        sliderDirectPositionPressed)
    {
        return;
    }

    /*
    //@@@checkme
    if (motionDone == false)
    {
        return;
    }*/
    slider->setValue(val);
}

void JointItem::updateSliderVelocity(double val)
{
    if (sliderVelocityPressed)
    {
        return;
    }
    ui->sliderVelocityVelocity->setValue(val);
}

void JointItem::updateSliderPWM(double val)
{
    if (sliderPWMPressed)  {
        return;
    }
    ui->sliderPWMOutput->setValue(val);
}

void JointItem::updateSliderCurrent(double val)
{
    if (sliderCurrentPressed)  {
        return;
    }
    ui->sliderCurrentOutput->setValue(val);
}

void JointItem::updateSliderTorque(double val)
{
    if(sliderTorquePressed){
        return;
    }
    ui->sliderTorqueTorque->setValue(val);
}

void JointItem::updateSliderTrajectoryVelocity(double val)
{
    if (sliderTrajectoryVelocityPressed){
        return;
    }
    ui->sliderTrajectoryVelocity->setValue(val);
}

void JointItem::setRefPWM(double pwmValue)
{
    if(sliderPWMPressed){
        return;
    }
    if(ui->stackedWidget->currentIndex() == PWM){
        updateSliderPWM(pwmValue);
        QString sVal;
        sVal = QString("%L1").arg(pwmValue, 0, 'f', 3);
        ui->editPWMDuty->setText(sVal);
    }
}

void JointItem::setRefCurrent(double currentValue)
{
    if (sliderCurrentPressed){
        return;
    }
    if (ui->stackedWidget->currentIndex() == CURRENT){
        updateSliderCurrent(currentValue);
        QString sVal;
        sVal = QString("%L1").arg(currentValue, 0, 'f', 3);
        ui->editCurrentCurrent->setText(sVal);
    }
}

void JointItem::setPosition(double val)
{
    QString sVal;
    if (ui->sliderTrajectoryPosition->getIsDouble())
    {
        double pos = val;
        sVal = QString("%L1").arg(pos, 0, 'f', 3);
    }
    else
    {
        double pos = floor(val * 10) / 10;
        sVal = QString("%1").arg(pos);
    }

    if(ui->stackedWidget->currentIndex() == IDLE){
        ui->editIdleJointPos->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionJointPos->setText(sVal);
        updateSliderPosition(ui->sliderTrajectoryPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == POSITION_DIR){
        ui->editPositionDirJointPos->setText(sVal);
        updateSliderPosition(ui->sliderDirectPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == MIXED){
        ui->editMixedJointPos->setText(sVal);
        updateSliderPosition(ui->sliderMixedPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == VELOCITY){
        ui->editVelocityJointPos->setText(sVal);

    }
    if(ui->stackedWidget->currentIndex() == TORQUE){
        ui->editTorqueJointPos->setText(sVal);
    }
    if(ui->stackedWidget->currentIndex() == PWM){
        ui->editPWMJointPos->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == CURRENT){
        ui->editCurrentJointPos->setText(sVal);
    }

}

void JointItem::setRefTorque(double val)
{
    if(ui->stackedWidget->currentIndex() == TORQUE){
        updateSliderTorque(val);
        ref_torque = val;
    }
}

void JointItem::setRefVelocitySpeed(double val)
{
    if (ui->stackedWidget->currentIndex() == VELOCITY){
        updateSliderVelocity(val);
        ref_speed = val;
    }
}

void JointItem::setRefTrajectorySpeed(double val)
{
    if (ui->stackedWidget->currentIndex() == POSITION){
        updateSliderTrajectoryVelocity(val);
        ref_trajectory_velocity = val;

        if (val < 0.001)
        {
            ui->groupBox_12->setStyleSheet("background-color:orange; color: rgb(35, 38, 41);");
            ui->groupBox_12->setTitle("Velocity is ZERO!");
        }
        else
        {
            ui->groupBox_12->setStyleSheet("background-color:transparent; color: rgb(35, 38, 41);");
            ui->groupBox_12->setTitle("Velocity");
        }
    }
}

void JointItem::setRefTrajectoryPosition(double val)
{
    if (ui->stackedWidget->currentIndex() == POSITION){
        updateTrajectoryPositionTarget(val);
    }
}

void JointItem::setTorque(double val)
{
    QString sVal = QString("%L1").arg(val,0,'f',3);

    if(ui->stackedWidget->currentIndex() == IDLE){
        ui->editIdleTorque->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionTorque->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION_DIR){
        ui->editPositionDirTorque->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == MIXED){
        ui->editMixedTorque->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == VELOCITY){
        ui->editVelocityTorque->setText(sVal);
    }
    if(ui->stackedWidget->currentIndex() == TORQUE){
        ui->editTorqueTorque->setText(sVal);
    }
    if(ui->stackedWidget->currentIndex() == PWM){
        ui->editPWMTorque->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == CURRENT){
        ui->editCurrentTorque->setText(sVal);
    }
}

void JointItem::setMotorPosition(double val)
{
    if (!joint_motorPositionVisible){
        return;
    }

    double mot = val;
    QString sVal = QString("%1").arg(mot, 0, 'f', 1);

    if (ui->stackedWidget->currentIndex() == IDLE){
        ui->editIdleMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == POSITION_DIR) {
        ui->editPositionDirMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == MIXED) {
        ui->editMixedMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == VELOCITY) {
        ui->editVelocityMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == TORQUE) {
        ui->editTorqueMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == PWM) {
        ui->editPWMMotorPos->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == CURRENT) {
        ui->editCurrentMotorPos->setText(sVal);
    }
}

void JointItem::setDutyCycles(double val)
{
    if (!joint_dutyVisible) {
        return;
    }

    double mot = val;
    QString sVal = QString("%1").arg(mot, 0, 'f', 1);

    //if (ui->stackedWidget->currentIndex() == IDLE) {
    //    ui->editIdleDuty->setText(sVal);
    //}

    //if (ui->stackedWidget->currentIndex() == POSITION) {
    //    ui->editPositionDuty->setText(sVal);
    //}

    //if (ui->stackedWidget->currentIndex() == POSITION_DIR) {
    //    ui->editPositionDirDuty->setText(sVal);
    //}

    //if (ui->stackedWidget->currentIndex() == MIXED) {
    //    ui->editMixedDuty->setText(sVal);
    //}

    //if (ui->stackedWidget->currentIndex() == VELOCITY) {
    //    ui->editVelocityDuty->setText(sVal);
    //}

    if (ui->stackedWidget->currentIndex() == TORQUE) {
        ui->editTorqueDuty->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == PWM) {
        ui->editPWMDuty->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == CURRENT) {
        ui->editCurrentDuty->setText(sVal);
    }
}

void JointItem::setSpeed(double meas)
{
    if (!joint_speedVisible){
        return;
    }
    //TODO

    double speed = meas;
    QString sVal = QString("%1").arg(speed,0,'f',1);

    if(ui->stackedWidget->currentIndex() == IDLE){
        ui->editIdleSpeed->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionSpeed->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION_DIR){
        ui->editPositionDirSpeed->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == MIXED){
        ui->editMixedSpeed->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == VELOCITY){
        if(!sliderVelocityPressed){
            ui->editVelocitySpeed->setText(sVal);
            //updateSlider(ui->sliderVelocityVelocity,ui->labelVelocityVelocity,val);
        }
    }
    if(ui->stackedWidget->currentIndex() == TORQUE){
        ui->editTorqueSpeed->setText(sVal);
        //updateSliderTorque(val);
    }
    if(ui->stackedWidget->currentIndex() == PWM){
        ui->editPWMSpeed->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == CURRENT){
        ui->editCurrentSpeed->setText(sVal);
    }

}

void JointItem::setCurrent(double meas)
{
    if (!joint_currentVisible) {
        return;
    }
    //TODO

    double current = meas;
    QString sVal = QString("%1").arg(current, 0, 'f', 3);

    if (ui->stackedWidget->currentIndex() == IDLE) {
        ui->editIdleCurrent->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == POSITION) {
        ui->editPositionCurrent->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == POSITION_DIR) {
        ui->editPositionDirCurrent->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == MIXED) {
        ui->editMixedCurrent->setText(sVal);
    }

    if (ui->stackedWidget->currentIndex() == VELOCITY) {
        ui->editVelocityCurrent->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == TORQUE) {
        ui->editTorqueCurrent->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == PWM) {
        ui->editPWMCurrent->setText(sVal);
    }
    if (ui->stackedWidget->currentIndex() == CURRENT) {
        ui->editCurrentCurrent->setText(sVal);
    }

}




void JointItem::setJointInternalInteraction(int interaction)
{
    disconnect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));
    ui->comboInteraction->setCurrentIndex(interaction);
    connect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));
}

void JointItem::setJointInternalState(int mode)
{
    disconnect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));
    ui->comboMode->setCurrentIndex(mode);
    connect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));

    ui->stackedWidget->setCurrentIndex(mode);
    if(ui->stackedWidget->widget(mode)){
        QVariant variant = ui->comboMode->itemData(mode,Qt::BackgroundRole);
        QColor c;
        switch(internalState){
            case Disconnected:{
                c = disconnectColor;
                break;
            }
            case HwFault:{
                c = hwFaultColor;
                break;
            }
            case Unknown:
            case NotConfigured:
            case Configured:
            case CalibDone:
            case Calibrating:
                c = calibratingColor;
                break;

            default:{
                enableAll();
                c = variant.value<QColor>();
                if(ui->groupBox->title() != QString("JOINT %1 (%2)").arg(jointIndex).arg(jointName))
                {
                    ui->groupBox->setTitle(QString("JOINT %1 (%2)").arg(jointIndex).arg(jointName));
                }
                ui->stackedWidget->setEnabled(true);
                ui->buttonsContainer->setEnabled(true);
                break;
            }
        }

        setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
    }
}

void JointItem::setJointInteraction(JointInteraction interaction)
{
    if(internalInteraction == interaction){
        return;
    }

    internalInteraction = interaction;

    switch (internalInteraction) {
    case Stiff:
        setJointInternalInteraction(STIFF);
        break;
    case COMPLIANT:
        setJointInternalInteraction(COMPLIANT);
        break;
    default:
        break;
    }
}


void JointItem::setJointState(JointState newState)
{
    if(internalState == newState){
        return;
    }

    internalState = newState;

    switch (internalState) {
    case Unknown:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  UNKNOWN").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(false);
        ui->buttonsContainer->setEnabled(false);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = calibratingColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }
    case Configured:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  CONFIGURED").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(true);
        ui->buttonsContainer->setEnabled(true);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = calibratingColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }

    case NotConfigured:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  NOT CONFIGURED").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(false);
        //activating only calib button
        if(enableCalib)
        {

            ui->buttonsContainer->setEnabled(true);
            ui->buttonIdle->setEnabled(false);
            ui->buttonPid->setEnabled(true);
            ui->buttonHome->setEnabled(false);
            ui->buttonCalib->setEnabled(true);
            ui->buttonRun->setEnabled(false);
            ui->comboMode->setEnabled(false);
            ui->comboInteraction->setEnabled(false);
        }
        //nothing must be activated
        else
        {
            ui->buttonsContainer->setEnabled(false);
        }

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = calibratingColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }

    case CalibDone:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  CALIBRATING DONE").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(true);
        ui->buttonsContainer->setEnabled(true);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = calibratingColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }
    case Calibrating:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  CALIBRATING").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(false);
        ui->buttonsContainer->setEnabled(false);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = calibratingColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }
    case HwFault:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  HARDWARE FAULT").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(false);
        //ui->buttonsContainer->setEnabled(false);
        ui->buttonHome->setEnabled(false);
        ui->buttonCalib->setEnabled(false);
        ui->buttonRun->setEnabled(false);
        ui->comboMode->setEnabled(false);
        ui->comboInteraction->setEnabled(false);
        ui->buttonIdle->setEnabled(true);
        ui->buttonPid->setEnabled(true);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = hwFaultColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }
    case Disconnected:{
        ui->groupBox->setTitle(QString("JOINT %1 (%2) -  DISCONNECTED").arg(jointIndex).arg(jointName));
        ui->stackedWidget->setEnabled(false);
        ui->buttonsContainer->setEnabled(false);

        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = disconnectColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3); color: rgb(35, 38, 41);").arg(c.red()).arg(c.green()).arg(c.blue()));
        }
        break;
    }
    case Idle:{
        setJointInternalState(IDLE);
        break;
    }
    case Position:{
        setJointInternalState(POSITION);
        break;
    }
    case PositionDirect:{
        setJointInternalState(POSITION_DIR);
        break;
    }
    case Mixed:{
        setJointInternalState(MIXED);
        break;
    }
    case Velocity:{
        setJointInternalState(VELOCITY);
        break;
    }
    case Torque:{
        setJointInternalState(TORQUE);
        break;
    }
    case Pwm:{
        setJointInternalState(PWM);
        break;
    }
    case Current:{
        setJointInternalState(CURRENT);
        break;
    }
    default:
        break;
    }

}

void JointItem::enableAll()
{
    ui->stackedWidget->setEnabled(true);
    ui->buttonsContainer->setEnabled(true);
    ui->buttonIdle->setEnabled(true);
    ui->buttonPid->setEnabled(true);
    ui->buttonHome->setEnabled(true);
    if(enableCalib){
        ui->buttonCalib->setEnabled(true);
    }
    ui->buttonRun->setEnabled(true);
    ui->comboMode->setEnabled(true);
    ui->comboInteraction->setEnabled(true);

}

void JointItem::setPWMRange(double min, double max)
{
    ui->sliderPWMOutput->setRange(min,max);
}

void JointItem::setCurrentRange(double min, double max)
{
    if (min < max)
    {
        min_current = min;
        max_current = max;
    }
    else
    {
        //Error
    }
    ui->sliderCurrentOutput->setRange(min, max);
}

void JointItem::setPositionRange(double min, double max)
{
    min_position = 1;
    max_position = 2;
    if(min < max)
    {
        min_position = min;
        max_position = max;
    }
    else
    {
        //Error
    }
}

void JointItem::setVelocityRange(double min, double max)
{
    min_velocity = -100;
    max_velocity = 100;
    if (min < max)
    {
        min_velocity = min;
        max_velocity = max;
    }
    else
    {
        //Error
    }
}

void JointItem::setTrajectoryVelocityRange( double max)
{
    max_trajectory_velocity = 100;
    if (max >= 0)
    {
        max_trajectory_velocity = max;
    }
    else
    {
        //Error
    }
}

void JointItem::setTorqueRange(double max)
{
    max_torque = 5;
    if (max >= 0)
    {
        max_torque = max;
    }
    else
    {
        //Error
    }
}

void JointItem::onCalibClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    emit calibClicked(this);
}

void JointItem::onHomeClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    emit homeClicked(this);
}

void JointItem::onIdleClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    emit idleClicked(this);
}

void JointItem::onRunClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    emit runClicked(this);
}

void JointItem::onPidClicked()
{
    emit pidClicked(this);
}

void JointItem::sequenceActivated()
{
    ui->sliderMixedPosition->setEnabled(false);
    ui->sliderMixedVelocity->setEnabled(false);
    ui->sliderPWMOutput->setEnabled(false);
    ui->sliderCurrentOutput->setEnabled(false);
    ui->sliderDirectPosition->setEnabled(false);
    ui->sliderTrajectoryPosition->setEnabled(false);
    ui->sliderTrajectoryVelocity->setEnabled(false);
    ui->sliderTorqueTorque->setEnabled(false);

}

void JointItem::sequenceStopped()
{
    ui->sliderMixedPosition->setEnabled(true);
    ui->sliderMixedVelocity->setEnabled(true);
    ui->sliderPWMOutput->setEnabled(true);
    ui->sliderCurrentOutput->setEnabled(true);
    ui->sliderDirectPosition->setEnabled(true);
    ui->sliderTrajectoryPosition->setEnabled(true);
    ui->sliderTrajectoryVelocity->setEnabled(true);
    ui->sliderTorqueTorque->setEnabled(true);
}


bool WheelEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
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
    int totValues = fabs(mmax - mmin);
    double cursor = fabs(val - mmin);
    double newX = cursor * (double)w / (double)totValues;
    ui->sliderTrajectoryPosition->updateSliderTarget(newX);
}

void JointItem::updateMixedPositionTarget(double val)
{
    int w = ui->sliderMixedPosition->width() - 30;
    double mmin = this->min_position;
    double mmax = this->max_position;
    int totValues = fabs(mmax - mmin);
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
    sliderOpenloopPressed = false;
    sliderVelocityPressed = false;
    enableCalib = true;
    speedVisible = false;
    lastVelocity = 0;
    velocityModeEnabled = false;
    motionDone = true;

    positionSliderStepIsAuto = true;
    velocitySliderStepIsAuto = false;
    trajectoryVelocitySliderStepIsAuto = false;

    max_position = 0;
    min_position = 0;
    max_velocity = 0;
    min_velocity = 0;
    max_trajectory_velocity = 0;
    max_torque = 0;
    min_torque = 0;
    ref_torque = 0;
    ref_openloop = 0;

    IDLE            = 0;
    POSITION        = 1;
    POSITION_DIR    = 2;
    MIXED           = 3;
    VELOCITY        = 4;
    TORQUE          = 5;
    OPENLOOP        = 6;



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

    ui->sliderOpenloopOutput->installEventFilter(this);
    connect(ui->sliderOpenloopOutput,SIGNAL(sliderPressed()),this,SLOT(onSliderOpenloopPressed()));
    connect(ui->sliderOpenloopOutput,SIGNAL(sliderReleased()),this,SLOT(onSliderOpenloopReleased()));
    ui->sliderOpenloopOutput->disableClickOutOfHandle = true;
    ui->sliderOpenloopOutput->enableViewTarget = false;

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




    movingSliderStyle = "QSlider::groove:horizontal:enabled {"
            "border: 1px solid #999999;"
            "height: 8px;"
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF2E2E, stop:1 #FDA6A6);"
            "margin: 2px 0;}"
       "QSlider::groove:horizontal:disabled {"
            "border: 1px solid #c8c8c8;"
            "height: 8px;"
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f0f0f0, stop:1 #dcdcdc);"
            "margin: 2px 0;}"
        "QSlider::handle:horizontal:enabled {"
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
            "border: 1px solid #5c5c5c;"
            "width: 30px;"
            "margin: -2px 0;"
            "border-radius: 3px;}"
        "QSlider::handle:horizontal:disabled {"
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #e6e6e6, stop:1 #c8c8c8);"
            "border: 1px solid #c8c8c8;"
            "width: 30px;"
            "margin: -2px 0;"
            "border-radius: 3px;}";



    comboStyle1 = "QComboBox {"
            "border: 1px solid gray;"
            "border-radius: 3px;"
            "padding: 1px 18px 1px 3px;"
            "min-width: 6em;}"
            "QComboBox:editable {"
            "background: white;}"
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

    ui->comboInteraction->setItemData(0,QColor(Qt::darkGray),Qt::BackgroundRole);
    ui->comboInteraction->setItemData(1,QColor(0,80,255),Qt::BackgroundRole);


//    idleColor           = QColor( 249,236,141);
//    positionColor       = QColor( 149,221,186);
//    positionDirectColor = QColor( 119,206,111);
//    mixedColor          = QColor( 150,(221+190)/2,(186+255)/2);
//    velocityColor       = QColor( 150,190,255);
//    torqueColor         = QColor( 219,166,171);
//    openLoopColor       = QColor( 250,250,250);
//    errorColor          = QColor(255,0,0);
//    disconnectColor     = QColor(190,190,190);
//    hwFaultColor        = QColor(255,0,0);
//    calibratingColor    = QColor(220,220,220);

    ui->comboMode->setItemData( IDLE,           idleColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( POSITION,       positionColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( POSITION_DIR,   positionDirectColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( MIXED,          mixedColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( VELOCITY,       velocityColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( TORQUE,         torqueColor, Qt::BackgroundRole );
    ui->comboMode->setItemData( OPENLOOP,       openLoopColor, Qt::BackgroundRole );

    ui->comboMode->setItemData( IDLE,           Idle, Qt::UserRole);
    ui->comboMode->setItemData( POSITION,       Position, Qt::UserRole );
    ui->comboMode->setItemData( POSITION_DIR,   PositionDirect, Qt::UserRole );
    ui->comboMode->setItemData( MIXED,          Mixed, Qt::UserRole );
    ui->comboMode->setItemData( VELOCITY,       Velocity, Qt::UserRole );
    ui->comboMode->setItemData( TORQUE,         Torque, Qt::UserRole );
    ui->comboMode->setItemData( OPENLOOP,       OpenLoop, Qt::UserRole );

    QString styleSheet = QString("%1 QComboBox:!editable, QComboBox::drop-down:editable {background-color: rgb(149,221,186);} %2").arg(comboStyle1).arg(comboStyle2);
    ui->comboMode->setStyleSheet(styleSheet);

    setJointInternalState(IDLE);

    QVariant variant = ui->comboInteraction->itemData(0,Qt::BackgroundRole);
    QColor c = variant.value<QColor>();

    styleSheet = QString("%1 QComboBox:!editable, QComboBox::drop-down:editable {background-color: rgb(%2,%3,%4);} %5").arg(comboStyle1).arg(c.red()).arg(c.green()).arg(c.blue()).arg(comboStyle2);
    ui->comboInteraction->setStyleSheet(styleSheet);


    ui->stackedWidget->widget(VELOCITY)->setEnabled(false);
    velocityTimer.setInterval(50);
    velocityTimer.setSingleShot(false);
    connect(&velocityTimer,SIGNAL(timeout()),this,SLOT(onVelocityTimer()));
}

bool JointItem::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();
        if(key == Qt::Key_Left || key == Qt::Key_Right  || key == Qt::Key_Up ||
           key == Qt::Key_Down || key == Qt::Key_PageUp || key == Qt::Key_PageDown){

            //SliderWithTarget *slider=0;
            QSlider* slider = 0;

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
            if(obj == ui->sliderOpenloopOutput){
                slider = ui->sliderOpenloopOutput;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderOpenloopPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderOpenloopReleased();
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

void JointItem::enableControlPositionDirect(bool control)
{
    ui->stackedWidget->widget(POSITION_DIR)->setEnabled(control);
}

void JointItem::enableControlOpenloop(bool control)
{
    ui->stackedWidget->widget(OPENLOOP)->setEnabled(control);
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
        ui->label_velUnits->setText(vel_metric_revolute);
        ui->label_velUnits_2->setText(vel_metric_revolute);
        ui->label_velUnits_3->setText(vel_metric_revolute);
        ui->label_velUnits_4->setText(vel_metric_revolute);
        ui->label_velUnits_5->setText(vel_metric_revolute);
        ui->label_velUnits_6->setText(vel_metric_revolute);
        ui->label_velUnits_7->setText(vel_metric_revolute);

        ui->label_trqTitle->setText(trq_metric_revolute_title);
        ui->label_trqTitle_2->setText(trq_metric_revolute_title);
        ui->label_trqTitle_3->setText(trq_metric_revolute_title);
        ui->label_trqTitle_4->setText(trq_metric_revolute_title);
        ui->label_trqTitle_5->setText(trq_metric_revolute_title);
        ui->label_trqTitle_6->setText(trq_metric_revolute_title);
        ui->label_trqTitle_7->setText(trq_metric_revolute_title);

        ui->label_posUnits->setText(pos_metric_revolute);
        ui->label_posUnits_2->setText(pos_metric_revolute);
        ui->label_posUnits_3->setText(pos_metric_revolute);
        ui->label_posUnits_4->setText(pos_metric_revolute);
        ui->label_posUnits_5->setText(pos_metric_revolute);
        ui->label_posUnits_6->setText(pos_metric_revolute);
        ui->label_posUnits_7->setText(pos_metric_revolute);

        ui->label_trqUnits->setText(trq_metric_revolute);
        ui->label_trqUnits_2->setText(trq_metric_revolute);
        ui->label_trqUnits_3->setText(trq_metric_revolute);
        ui->label_trqUnits_4->setText(trq_metric_revolute);
        ui->label_trqUnits_5->setText(trq_metric_revolute);
        ui->label_trqUnits_6->setText(trq_metric_revolute);
        ui->label_velUnits_7->setText(trq_metric_revolute);
    }
    else if (t == yarp::dev::VOCAB_JOINTTYPE_PRISMATIC)
    {
        ui->label_velUnits->setText(vel_metric_prism);
        ui->label_velUnits_2->setText(vel_metric_prism);
        ui->label_velUnits_3->setText(vel_metric_prism);
        ui->label_velUnits_4->setText(vel_metric_prism);
        ui->label_velUnits_5->setText(vel_metric_prism);
        ui->label_velUnits_6->setText(vel_metric_prism);
        ui->label_velUnits_7->setText(vel_metric_prism);

        ui->label_trqTitle->setText(trq_metric_prism_title);
        ui->label_trqTitle_2->setText(trq_metric_prism_title);
        ui->label_trqTitle_3->setText(trq_metric_prism_title);
        ui->label_trqTitle_4->setText(trq_metric_prism_title);
        ui->label_trqTitle_5->setText(trq_metric_prism_title);
        ui->label_trqTitle_6->setText(trq_metric_prism_title);
        ui->label_trqTitle_7->setText(trq_metric_prism_title);

        ui->label_posUnits->setText(pos_metric_prism);
        ui->label_posUnits_2->setText(pos_metric_prism);
        ui->label_posUnits_3->setText(pos_metric_prism);
        ui->label_posUnits_4->setText(pos_metric_prism);
        ui->label_posUnits_5->setText(pos_metric_prism);
        ui->label_posUnits_6->setText(pos_metric_prism);
        ui->label_posUnits_7->setText(pos_metric_prism);

        ui->label_trqUnits->setText(trq_metric_prism);
        ui->label_trqUnits_2->setText(trq_metric_prism);
        ui->label_trqUnits_3->setText(trq_metric_prism);
        ui->label_trqUnits_4->setText(trq_metric_prism);
        ui->label_trqUnits_5->setText(trq_metric_prism);
        ui->label_trqUnits_6->setText(trq_metric_prism);
        ui->label_velUnits_7->setText(trq_metric_prism);
    }
    else
    {
        yFatal("Unspecified joint type");
    }
}

void JointItem::setSpeedVisible(bool visible)
{
    speedVisible = visible;
    ui->editIdleSpeed->setVisible(visible);
    ui->editPositionSpeed->setVisible(visible);
    ui->editPositionDirSpeed->setVisible(visible);
    ui->editMixedSpeed->setVisible(visible);
    ui->editTorqueSpeed->setVisible(visible);
    ui->editOpenLoopSpeed->setVisible(visible);
    ui->editVelocitySpeed->setVisible(visible);

    ui->labelSpeed->setVisible(visible);
    ui->label_velUnits->setVisible(visible);
    ui->labelSpeed2->setVisible(visible);
    ui->label_velUnits_2->setVisible(visible);
    ui->labelSpeed4->setVisible(visible);
    ui->label_velUnits_3->setVisible(visible);
    ui->labelSpeed6->setVisible(visible);
    ui->label_velUnits_4->setVisible(visible);
    ui->labelSpeed8->setVisible(visible);
    ui->label_velUnits_5->setVisible(visible);
    ui->labelSpeed10->setVisible(visible);
    ui->label_velUnits_6->setVisible(visible);
    ui->labelSpeed10_2->setVisible(visible);
    ui->label_velUnits_7->setVisible(visible);


    if(!visible){
        ui->editIdleSpeed->setMinimumHeight(0);
        ui->editPositionSpeed->setMinimumHeight(0);
        ui->editPositionDirSpeed->setMinimumHeight(0);
        ui->editMixedSpeed->setMinimumHeight(0);
        ui->editTorqueSpeed->setMinimumHeight(0);
        ui->editOpenLoopSpeed->setMinimumHeight(0);
        ui->editVelocitySpeed->setMinimumHeight(0);

        ui->labelSpeed->setMinimumHeight(0);
        ui->label_velUnits->setMinimumHeight(0);
        ui->labelSpeed2->setMinimumHeight(0);
        ui->label_velUnits_2->setMinimumHeight(0);
        ui->labelSpeed4->setMinimumHeight(0);
        ui->label_velUnits_3->setMinimumHeight(0);
        ui->labelSpeed6->setMinimumHeight(0);
        ui->label_velUnits_4->setMinimumHeight(0);
        ui->labelSpeed8->setMinimumHeight(0);
        ui->label_velUnits_5->setMinimumHeight(0);
        ui->labelSpeed10->setMinimumHeight(0);
        ui->label_velUnits_6->setMinimumHeight(0);
        ui->labelSpeed10_2->setMinimumHeight(0);
        ui->label_velUnits_7->setMinimumHeight(0);

    }else{
        ui->editIdleSpeed->setMinimumHeight(20);
        ui->editPositionSpeed->setMinimumHeight(20);
        ui->editPositionDirSpeed->setMinimumHeight(20);
        ui->editMixedSpeed->setMinimumHeight(20);
        ui->editTorqueSpeed->setMinimumHeight(20);
        ui->editOpenLoopSpeed->setMinimumHeight(20);
        ui->editVelocitySpeed->setMinimumHeight(20);

        ui->labelSpeed->setMinimumHeight(20);
        ui->label_velUnits->setMinimumHeight(20);
        ui->labelSpeed2->setMinimumHeight(20);
        ui->label_velUnits_2->setMinimumHeight(20);
        ui->labelSpeed4->setMinimumHeight(20);
        ui->label_velUnits_3->setMinimumHeight(20);
        ui->labelSpeed6->setMinimumHeight(20);
        ui->label_velUnits_4->setMinimumHeight(20);
        ui->labelSpeed8->setMinimumHeight(20);
        ui->label_velUnits_5->setMinimumHeight(20);
        ui->labelSpeed10->setMinimumHeight(20);
        ui->label_velUnits_6->setMinimumHeight(20);
        ui->labelSpeed10_2->setMinimumHeight(20);
        ui->label_velUnits_7->setMinimumHeight(20);
    }
}

void JointItem::enablePositionSliderDoubleAuto()
{
    positionSliderStepIsAuto = true;
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
    velocitySliderStepIsAuto = true;
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
    torqueSliderStepIsAuto = true;
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
    if (fabs(max_velocity) < 1.0)
    {
        yError("Unable to set integer velocity slider");
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

void JointItem::enableTrajectoryVelocitySliderDoubleAuto()
{
    trajectoryVelocitySliderStepIsAuto = true;
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

    disconnect(ui->sliderOpenloopOutput,SIGNAL(sliderPressed()),this,SLOT(onSliderOpenloopPressed()));
    disconnect(ui->sliderOpenloopOutput,SIGNAL(sliderReleased()),this,SLOT(onSliderOpenloopReleased()));

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
    WheelEventFilter *filter = new WheelEventFilter();
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
    changeMode(mode,this);
}


void JointItem::onInteractionChanged(int index)
{
    changeInteraction(index,this);
}

void JointItem::setJointName(QString name)
{
    jointName = name;
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
        sliderMixedVelocityCommand(val / ui->sliderMixedVelocity->getSliderStep(), jointIndex);
    }
    else
    {
        sliderMixedVelocityCommand(ui->sliderMixedVelocity->value(), jointIndex);
    }

    sliderMixedVelocityPressed = false;
}


void JointItem::onVelocityTimer()
{
    if(velocityModeEnabled)
    {
        sliderVelocityCommand(lastVelocity,jointIndex);
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
        sliderTrajectoryPositionCommand(val / ui->sliderTrajectoryPosition->getSliderStep(), jointIndex);
        updateTrajectoryPositionTarget(val / ui->sliderTrajectoryPosition->getSliderStep());
    }
    else
    {
        double val = ui->sliderTrajectoryPosition->value();
        sliderTrajectoryPositionCommand(val, jointIndex);
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
        sliderMixedPositionCommand(val / ui->sliderMixedPosition->getSliderStep(), jointIndex);
        updateMixedPositionTarget(val / ui->sliderMixedPosition->getSliderStep());
    }
    else
    {
        double val = ui->sliderMixedPosition->value();
        sliderMixedPositionCommand(val, jointIndex);
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
    sliderDirectPositionCommand(ref_direct_position, jointIndex);
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
    sliderTrajectoryVelocityCommand(ref_trajectory_velocity, jointIndex);
    sliderTrajectoryVelocityPressed = false;
}

void JointItem::onSliderOpenloopPressed()
{
    sliderOpenloopPressed = true;
}

void JointItem::onSliderOpenloopReleased()
{
    ref_openloop = (double)ui->sliderOpenloopOutput->value() / ui->sliderOpenloopOutput->getSliderStep();
    sliderOpenloopCommand(ref_openloop, jointIndex);
    sliderOpenloopPressed = false;
}

void JointItem::onSliderTorquePressed()
{
    sliderTorquePressed = true;
}

void JointItem::onSliderTorqueReleased()
{
    ref_torque = (double)ui->sliderTorqueTorque->value() / ui->sliderTorqueTorque->getSliderStep();
    sliderTorqueCommand(ref_torque, jointIndex);
    sliderTorquePressed = false;
}


double JointItem::getTrajectoryPositionValue()
{
    //this function is mainly used used by the sequencer
    double pos = (double)ui->sliderTrajectoryPosition->value() / ui->sliderTrajectoryPosition->getSliderStep();
    if (fabs(pos) < 1e-6) pos = 0;
    return pos;
}

double JointItem::getTrajectoryVelocityValue()
{
    //this function is mainly used used by the sequencer
    double vel = (double)ui->sliderTrajectoryVelocity->value() / ui->sliderTrajectoryVelocity->getSliderStep();
    if (fabs(vel) < 1e-6) vel = 0;
    return vel;
}

void JointItem::updateMotionDone(bool done)
{
    motionDone = done;
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        if(!done){
            ui->editPositionCurrentPos->setStyleSheet("background-color: rgb(255, 38, 41);");
        }else{
            ui->editPositionCurrentPos->setStyleSheet("background-color: rgb(255, 255, 255);");
        }
    } else if (index == POSITION_DIR) {
        if(!done){
            ui->editPositionDirCurrentPos->setStyleSheet("background-color: rgb(255, 38, 41);");
        }else{
            ui->editPositionDirCurrentPos->setStyleSheet("background-color: rgb(255, 255, 255);");
        }
    } else if (index == MIXED) {
        if(!done){
            ui->editMixedCurrentPos->setStyleSheet("background-color: rgb(255, 38, 41);");
        }else{
            ui->editMixedCurrentPos->setStyleSheet("background-color: rgb(255, 255, 255);");
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

void JointItem::updateSliderOpenloop(double val)
{
    if (sliderOpenloopPressed)  {
        return;
    }
    ui->sliderOpenloopOutput->setValue(val);
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

void JointItem::setOpenLoop(double openLoopValue)
{
    if(sliderOpenloopPressed){
        return;
    }
    if(ui->stackedWidget->currentIndex() == OPENLOOP){
        updateSliderOpenloop(openLoopValue);
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
        ui->editIdleCurrentPos->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionCurrentPos->setText(sVal);
        double dt = ui->sliderTrajectoryPosition->value();
        updateSliderPosition(ui->sliderTrajectoryPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == POSITION_DIR){
        ui->editPositionDirCurrentPos->setText(sVal);
        updateSliderPosition(ui->sliderDirectPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == MIXED){
        ui->editMixedCurrentPos->setText(sVal);
        updateSliderPosition(ui->sliderMixedPosition, val);
    }

    if(ui->stackedWidget->currentIndex() == VELOCITY){
        ui->editVelocityCurrentPos->setText(sVal);

    }
    if(ui->stackedWidget->currentIndex() == TORQUE){
        ui->editTorqueCurrentPos->setText(sVal);
    }
    if(ui->stackedWidget->currentIndex() == OPENLOOP){
        ui->editOpenLoopCurrentPos->setText(sVal);
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
    if(ui->stackedWidget->currentIndex() == OPENLOOP){
        ui->editOpenLoopTorque->setText(sVal);
    }
}

void JointItem::setSpeed(double val)
{
    if(!speedVisible){
        return;
    }
    //TODO

    double speed = val;
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
    if(ui->stackedWidget->currentIndex() == OPENLOOP){
        ui->editOpenLoopSpeed->setText(sVal);
    }

}





void JointItem::setJointInternalInteraction(int interaction)
{
    disconnect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));
    ui->comboInteraction->setCurrentIndex(interaction);
    connect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));

    if(ui->stackedWidget->widget(interaction)){
        QVariant variant = ui->comboInteraction->itemData(interaction,Qt::BackgroundRole);
        QColor c = variant.value<QColor>();

        QString styleSheet = QString("%1 QComboBox:!editable, QComboBox::drop-down:editable {background-color: rgb(%2,%3,%4);} %5").arg(comboStyle1).arg(c.red()).arg(c.green()).arg(c.blue()).arg(comboStyle2);
        ui->comboInteraction->setStyleSheet(styleSheet);
    }
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


        //ui->stackedWidget->widget(mode)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
        setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));

        QString styleSheet = QString("%1 QComboBox:!editable, QComboBox::drop-down:editable {background-color: rgb(%2,%3,%4);} %5").arg(comboStyle1).arg(c.red()).arg(c.green()).arg(c.blue()).arg(comboStyle2);
        ui->comboMode->setStyleSheet(styleSheet);
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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


        int index = ui->stackedWidget->currentIndex();
        if(ui->stackedWidget->widget(index)){
            QColor c = hwFaultColor;
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
            setStyleSheet(QString("font: 8pt; background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
            //ui->stackedWidget->widget(index)->setStyleSheet(QString("background-color: rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()));
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
    case OpenLoop:{
        setJointInternalState(OPENLOOP);
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

void JointItem::setOpenLoopRange(double min, double max)
{
    ui->sliderOpenloopOutput->setRange(min,max);
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
    calibClicked(this);
}

void JointItem::onHomeClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    homeClicked(this);
}

void JointItem::onIdleClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    idleClicked(this);
}

void JointItem::onRunClicked()
{
    if (this->internalState == Velocity)
    {
       velocityTimer.stop();
    }
    runClicked(this);
}

void JointItem::onPidClicked()
{
    pidClicked(this);
}

void JointItem::sequenceActivated()
{
    ui->sliderMixedPosition->setEnabled(false);
    ui->sliderMixedVelocity->setEnabled(false);
    ui->sliderOpenloopOutput->setEnabled(false);
    ui->sliderDirectPosition->setEnabled(false);
    ui->sliderTrajectoryPosition->setEnabled(false);
    ui->sliderTrajectoryVelocity->setEnabled(false);
    ui->sliderTorqueTorque->setEnabled(false);

}

void JointItem::sequenceStopped()
{
    ui->sliderMixedPosition->setEnabled(true);
    ui->sliderMixedVelocity->setEnabled(true);
    ui->sliderOpenloopOutput->setEnabled(true);
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

/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */

#include "jointitem.h"
#include "ui_jointitem.h"

#include "log.h"
#include "yarpmotorgui.h"

#include <cmath>

#include <QDebug>
#include <QKeyEvent>

JointItem::JointItem(int index,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JointItem)
{
    ui->setupUi(this);
    internalState = StateStarting;
    internalInteraction = InteractionStarting;
    jointIndex = index;
    sliderPositionPressed = false;
    sliderTorquePressed = false;
    sliderOpenloopPressed = false;
    sliderVelocityPressed = false;
    speed = 10;
    enableCalib = true;
    speedVisible = false;
    lastVelocity = 0;
    velocityModeEnabled = false;

    IDLE            = 0;
    POSITION        = 1;
    POSITION_DIR    = 2;
    MIXED           = 3;
    VELOCITY        = 4;
    TORQUE          = 5;
    OPENLOOP        = 6;



    connect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));
    connect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));

    ui->sliderPositionPosition->installEventFilter(this);
    connect(ui->sliderPositionPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    connect(ui->sliderPositionPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));
    connect(ui->sliderPositionPosition,SIGNAL(actionTriggered(int)),this,SLOT(onSliderPositionActionTriggered(int)));

    ui->sliderTorqueTorque->installEventFilter(this);
    connect(ui->sliderTorqueTorque,SIGNAL(sliderPressed()),this,SLOT(onSliderTorquePressed()));
    connect(ui->sliderTorqueTorque,SIGNAL(sliderReleased()),this,SLOT(onSliderTorqueReleased()));

    ui->sliderOpenloopOutput->installEventFilter(this);
    connect(ui->sliderOpenloopOutput,SIGNAL(sliderPressed()),this,SLOT(onSliderOpenloopPressed()));
    connect(ui->sliderOpenloopOutput,SIGNAL(sliderReleased()),this,SLOT(onSliderOpenloopReleased()));

    ui->sliderPositionDirect->installEventFilter(this);
    connect(ui->sliderPositionDirect,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    connect(ui->sliderPositionDirect,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));

    ui->sliderMixedPosition->installEventFilter(this);
    connect(ui->sliderMixedPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    connect(ui->sliderMixedPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));

    ui->sliderVelocityVelocity->installEventFilter(this);
    connect(ui->sliderVelocityVelocity,SIGNAL(sliderPressed()),this,SLOT(onSliderVelocityPressed()));
    connect(ui->sliderVelocityVelocity,SIGNAL(sliderReleased()),this,SLOT(onSliderVelocityReleased()));

    ui->sliderPositionVelocity->installEventFilter(this);
    connect(ui->sliderPositionVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onSliderVelocityMoved(int)));

    ui->sliderMixedVelocity->installEventFilter(this);
    connect(ui->sliderMixedVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onSliderVelocityMoved(int)));


    connect(ui->buttonHome,SIGNAL(clicked()),this,SLOT(onHomeClicked()));
    connect(ui->buttonIdle,SIGNAL(clicked()),this,SLOT(onIdleClicked()));
    connect(ui->buttonRun,SIGNAL(clicked()),this,SLOT(onRunClicked()));
    connect(ui->buttonPid,SIGNAL(clicked()),this,SLOT(onPidClicked()));
    connect(ui->buttonCalib,SIGNAL(clicked()),this,SLOT(onCalibClicked()));

    ui->groupBox->setTitle(QString("JOINT %1").arg(index));
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

            QSlider *slider;
            QLabel *label;

            if(obj == ui->sliderPositionPosition){
                motionDone = false;
                slider = ui->sliderPositionPosition;
                label = ui->labelPositionPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderPositionReleased();
                }
            }
            if(obj == ui->sliderPositionVelocity){
                motionDone = false;
                slider = ui->sliderPositionVelocity;
                label = ui->labelPositionVelocity;
            }
            if(obj == ui->sliderPositionDirect){
                motionDone = false;
                slider = ui->sliderPositionDirect;
                label = ui->labelPositionDirectPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderPositionReleased();
                }
            }
            if(obj == ui->sliderMixedPosition){
                motionDone = false;
                slider = ui->sliderMixedPosition;
                label = ui->labelMixedPosition;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderPositionPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderPositionReleased();
                }
            }
            if(obj == ui->sliderMixedVelocity){
                slider = ui->sliderMixedVelocity;
                label = ui->labelMixedVelocity;
            }
            if(obj == ui->sliderTorqueTorque){
                slider = ui->sliderTorqueTorque;
                label = ui->labelTorqueTorque;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderTorquePressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderTorqueReleased();
                }
            }
            if(obj == ui->sliderOpenloopOutput){
                slider = ui->sliderOpenloopOutput;
                label = ui->labelOpenLoopOutput;
                if(keyEvent->type() == QEvent::KeyPress){
                    onSliderOpenloopPressed();
                }
                if(keyEvent->type() == QEvent::KeyRelease){
                    onSliderOpenloopReleased();
                }
            }
            if(obj == ui->sliderVelocityVelocity){
                slider = ui->sliderVelocityVelocity;
                label = ui->labelVelocityVelocity;
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
                if(obj == ui->sliderTorqueTorque){
                    updateSliderTorqueLabel(slider,label,slider->value());
                }else{
                    updateSliderLabel(slider,label,slider->value());
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

void JointItem::controlVelocity(bool control)
{
    velocityModeEnabled = control;
    ui->stackedWidget->widget(VELOCITY)->setEnabled(velocityModeEnabled);
    if(ui->stackedWidget->currentIndex() == VELOCITY && velocityModeEnabled){
        velocityTimer.start();
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
    ui->labelSpeed1->setVisible(visible);
    ui->labelSpeed2->setVisible(visible);
    ui->labelSpeed3->setVisible(visible);
    ui->labelSpeed4->setVisible(visible);
    ui->labelSpeed5->setVisible(visible);
    ui->labelSpeed6->setVisible(visible);
    ui->labelSpeed7->setVisible(visible);
    ui->labelSpeed8->setVisible(visible);
    ui->labelSpeed9->setVisible(visible);
    ui->labelSpeed10->setVisible(visible);
    ui->labelSpeed11->setVisible(visible);
    ui->labelSpeed10_2->setVisible(visible);
    ui->labelSpeed11_2->setVisible(visible);


    if(!visible){
        ui->editIdleSpeed->setMinimumHeight(0);
        ui->editPositionSpeed->setMinimumHeight(0);
        ui->editPositionDirSpeed->setMinimumHeight(0);
        ui->editMixedSpeed->setMinimumHeight(0);
        ui->editTorqueSpeed->setMinimumHeight(0);
        ui->editOpenLoopSpeed->setMinimumHeight(0);
        ui->editVelocitySpeed->setMinimumHeight(0);

        ui->labelSpeed->setMinimumHeight(0);
        ui->labelSpeed1->setMinimumHeight(0);
        ui->labelSpeed2->setMinimumHeight(0);
        ui->labelSpeed3->setMinimumHeight(0);
        ui->labelSpeed4->setMinimumHeight(0);
        ui->labelSpeed5->setMinimumHeight(0);
        ui->labelSpeed6->setMinimumHeight(0);
        ui->labelSpeed7->setMinimumHeight(0);
        ui->labelSpeed8->setMinimumHeight(0);
        ui->labelSpeed9->setMinimumHeight(0);
        ui->labelSpeed10->setMinimumHeight(0);
        ui->labelSpeed11->setMinimumHeight(0);
        ui->labelSpeed10_2->setMinimumHeight(0);
        ui->labelSpeed11_2->setMinimumHeight(0);

    }else{
        ui->editIdleSpeed->setMinimumHeight(20);
        ui->editPositionSpeed->setMinimumHeight(20);
        ui->editPositionDirSpeed->setMinimumHeight(20);
        ui->editMixedSpeed->setMinimumHeight(20);
        ui->editTorqueSpeed->setMinimumHeight(20);
        ui->editOpenLoopSpeed->setMinimumHeight(20);
        ui->editVelocitySpeed->setMinimumHeight(20);

        ui->labelSpeed->setMinimumHeight(20);
        ui->labelSpeed1->setMinimumHeight(20);
        ui->labelSpeed2->setMinimumHeight(20);
        ui->labelSpeed3->setMinimumHeight(20);
        ui->labelSpeed4->setMinimumHeight(20);
        ui->labelSpeed5->setMinimumHeight(20);
        ui->labelSpeed6->setMinimumHeight(20);
        ui->labelSpeed7->setMinimumHeight(20);
        ui->labelSpeed8->setMinimumHeight(20);
        ui->labelSpeed9->setMinimumHeight(20);
        ui->labelSpeed10->setMinimumHeight(20);
        ui->labelSpeed11->setMinimumHeight(20);
        ui->labelSpeed10_2->setMinimumHeight(20);
        ui->labelSpeed11_2->setMinimumHeight(20);
    }
}

void JointItem::setEnabledOptions(bool debug_param_enabled,
                                  bool speedview_param_enabled,
                                  bool enable_calib_all,
                                  bool position_direct_enabled,
                                  bool openloop_enabled)
{
    Q_UNUSED(debug_param_enabled);
    Q_UNUSED(speedview_param_enabled);

    enableCalib = enable_calib_all;
    if(!enable_calib_all){
        ui->buttonCalib->setEnabled(false);
    }

    if(!position_direct_enabled){
        ui->stackedWidget->widget(POSITION_DIR)->setEnabled(false);
//        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(POSITION_DIR));
//        ui->comboMode->removeItem(POSITION_DIR);
//        POSITION_DIR = -1;
//        MIXED--;
//        VELOCITY--;
//        TORQUE--;
//        OPENLOOP--;
    }

    if(!openloop_enabled){
        //ui->stackedWidget->removeWidget(ui->stackedWidget->widget(OPENLOOP));
        //ui->comboMode->removeItem(OPENLOOP);
        ui->stackedWidget->widget(OPENLOOP)->setEnabled(false);
        //OPENLOOP = -1;
    }


    connect(ui->stackedWidget,SIGNAL(currentChanged(int)),this,SLOT(onStackedWidgetChanged(int)));



}

JointItem::~JointItem()
{
    disconnect(ui->comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeChanged(int)));
    disconnect(ui->comboInteraction,SIGNAL(currentIndexChanged(int)),this,SLOT(onInteractionChanged(int)));

    disconnect(ui->sliderPositionPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    disconnect(ui->sliderPositionPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));

    disconnect(ui->sliderTorqueTorque,SIGNAL(sliderPressed()),this,SLOT(onSliderTorquePressed()));
    disconnect(ui->sliderTorqueTorque,SIGNAL(sliderReleased()),this,SLOT(onSliderTorqueReleased()));

    disconnect(ui->sliderOpenloopOutput,SIGNAL(sliderPressed()),this,SLOT(onSliderOpenloopPressed()));
    disconnect(ui->sliderOpenloopOutput,SIGNAL(sliderReleased()),this,SLOT(onSliderOpenloopReleased()));

    disconnect(ui->sliderPositionDirect,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    disconnect(ui->sliderPositionDirect,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));

    disconnect(ui->sliderMixedPosition,SIGNAL(sliderPressed()),this,SLOT(onSliderPositionPressed()));
    disconnect(ui->sliderMixedPosition,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionReleased()));

    disconnect(ui->sliderPositionVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onSliderVelocityMoved(int)));
    disconnect(ui->sliderMixedVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onSliderVelocityMoved(int)));

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
    ui->sliderPositionDirect->installEventFilter(filter);
    ui->sliderPositionPosition->installEventFilter(filter);
    ui->sliderPositionVelocity->installEventFilter(filter);
    ui->sliderTorqueTorque->installEventFilter(filter);
}


void JointItem::onStackedWidgetChanged(int index)
{
    if(index == VELOCITY){
        if(velocityModeEnabled){
            velocityTimer.start();
        }
    }else{
        if(velocityModeEnabled){
            velocityTimer.stop();
            lastVelocity = 0;
            updateSlider(ui->sliderVelocityVelocity,ui->labelVelocityVelocity,0);
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
    motionDone = true;
}


void JointItem::onInteractionChanged(int index)
{
    changeInteraction(index,this);
}


void JointItem::setTorque(double max,double min,double val)
{
    max_torque = max;
    min_torque = min;
    torque = val;
}

void JointItem::onVelocitySliderMoved(int val)
{
    int index = ui->stackedWidget->currentIndex();
    if (index == VELOCITY) {
        updateSliderLabel(ui->sliderVelocityVelocity,ui->labelVelocityVelocity,val);
    }
}

void JointItem::onSliderVelocityMoved(int val)
{
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        updateSliderLabel(ui->sliderPositionVelocity,ui->labelPositionVelocity,val);
        speed = val;
    } else if (index == MIXED) {
        updateSliderLabel(ui->sliderMixedVelocity,ui->labelMixedVelocity,val);
        speed = val;
    }

}

void JointItem::onSliderMoved(int val)
{
    motionDone = false;
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        updateSliderLabel(ui->sliderPositionPosition,ui->labelPositionPosition,val);
    } else if (index == POSITION_DIR) {
        updateSliderLabel(ui->sliderPositionDirect,ui->labelPositionDirectPosition,val);
    } else if (index == MIXED) {
        updateSliderLabel(ui->sliderMixedPosition,ui->labelMixedPosition,val);
    }
}

void JointItem::onSliderOpenloopMoved(int val)
{
    int index = ui->stackedWidget->currentIndex();
    if (index == OPENLOOP) {
        updateSliderLabel(ui->sliderOpenloopOutput,ui->labelOpenLoopOutput,(double)val);
    }
}

void JointItem::onSliderTorqueMoved(int val)
{
    int index = ui->stackedWidget->currentIndex();
    if (index == TORQUE) {
        updateSliderTorqueLabel(ui->sliderTorqueTorque,ui->labelTorqueTorque,(double)val);
    }
}

void JointItem::onSliderVelocityPressed()
{
    sliderVelocityPressed = true;
    int index = ui->stackedWidget->currentIndex();
    if (index == VELOCITY) {
        connect(ui->sliderVelocityVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onVelocitySliderMoved(int)));
    }
}

void JointItem::onSliderVelocityReleased()
{
    int index = ui->stackedWidget->currentIndex();
    if (index == VELOCITY) {
        disconnect(ui->sliderVelocityVelocity,SIGNAL(sliderMoved(int)),this,SLOT(onVelocitySliderMoved(int)));
        lastVelocity = ui->sliderVelocityVelocity->value();
        //sliderVelocityMoved(ui->sliderVelocityVelocity->value(),jointIndex);
    }


    sliderVelocityPressed = false;
}


void JointItem::onVelocityTimer()
{
    if(velocityModeEnabled){
        sliderVelocityMoved(lastVelocity,jointIndex);
        if(!sliderVelocityPressed){
            updateSliderLabel(ui->sliderVelocityVelocity,ui->labelVelocityVelocity,lastVelocity);
        }
    }
}

void JointItem::onSliderPositionPressed()
{
    sliderPositionPressed = true;
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        connect(ui->sliderPositionPosition,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
    } else if (index == POSITION_DIR) {
        connect(ui->sliderPositionDirect,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
    } else if (index == MIXED) {
        connect(ui->sliderMixedPosition,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
    }
}

void JointItem::onSliderPositionReleased()
{
    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        disconnect(ui->sliderPositionPosition,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
//        updateSliderLabel(ui->sliderPositionDirect,ui->labelPositionDirectPosition,ui->sliderPositionPosition->value());
//        updateSliderLabel(ui->sliderMixedPosition,ui->labelMixedPosition,ui->sliderPositionPosition->value());
        sliderPositionMoved(ui->sliderPositionPosition->value(),ui->sliderPositionVelocity->value(),jointIndex);
    } else if (index == POSITION_DIR) {
        disconnect(ui->sliderPositionDirect,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
//        updateSliderLabel(ui->sliderPositionPosition,ui->labelPositionPosition,ui->sliderPositionDirect->value());
//        updateSliderLabel(ui->sliderMixedPosition,ui->labelMixedPosition,ui->sliderPositionDirect->value());
        sliderPositionMoved(ui->sliderPositionDirect->value(),-1,jointIndex);
    } else if (index == MIXED) {
        disconnect(ui->sliderMixedPosition,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
//        updateSliderLabel(ui->sliderPositionPosition,ui->labelPositionPosition,ui->sliderMixedPosition->value());
//        updateSliderLabel(ui->sliderPositionDirect,ui->labelPositionDirectPosition,ui->sliderMixedPosition->value());
        sliderPositionMoved(ui->sliderMixedPosition->value(),ui->sliderMixedVelocity->value(),jointIndex);
    }

    sliderPositionPressed = false;
}

void JointItem::onSliderPositionActionTriggered(int val)
{
    if( !sliderPositionPressed){
        motionDone = false;
        int index = ui->stackedWidget->currentIndex();
        if (index == POSITION) {
            connect(ui->sliderPositionPosition,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)),Qt::UniqueConnection);
        } else if (index == POSITION_DIR) {
            connect(ui->sliderPositionDirect,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)),Qt::UniqueConnection);
        } else if (index == MIXED) {
            connect(ui->sliderMixedPosition,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)),Qt::UniqueConnection);
        }


        //onSliderPositionReleased();
    }
}
void JointItem::onSliderPositionValueChanged(int val)
{

    int index = ui->stackedWidget->currentIndex();
    if (index == POSITION) {
        disconnect(ui->sliderPositionPosition,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)));
    } else if (index == POSITION_DIR) {
        disconnect(ui->sliderPositionDirect,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)));
    } else if (index == MIXED) {
        disconnect(ui->sliderMixedPosition,SIGNAL(valueChanged(int)),this,SLOT(onSliderPositionValueChanged(int)));
    }
    onSliderMoved(val);
    onSliderPositionReleased();
}

void JointItem::onSliderOpenloopPressed()
{
    sliderOpenloopPressed = true;

    int index = ui->stackedWidget->currentIndex();
    if (index == OPENLOOP) {
        connect(ui->sliderOpenloopOutput,SIGNAL(sliderMoved(int)),this,SLOT(onSliderOpenloopMoved(int)));
    }
}

void JointItem::onSliderOpenloopReleased()
{
    int index = ui->stackedWidget->currentIndex();
    if (index == OPENLOOP) {
        disconnect(ui->sliderOpenloopOutput,SIGNAL(sliderMoved(int)),this,SLOT(onSliderOpenloopMoved(int)));
        sliderOpenloopMoved((double)ui->sliderOpenloopOutput->value(),jointIndex);
    }
    sliderOpenloopPressed = false;
}

void JointItem::onSliderTorquePressed()
{
    sliderTorquePressed = true;
    int index = ui->stackedWidget->currentIndex();
    if (index == TORQUE) {
        connect(ui->sliderTorqueTorque,SIGNAL(sliderMoved(int)),this,SLOT(onSliderTorqueMoved(int)));
    }
}

void JointItem::onSliderTorqueReleased()
{

    int index = ui->stackedWidget->currentIndex();
    if (index == TORQUE) {
        disconnect(ui->sliderTorqueTorque,SIGNAL(sliderMoved(int)),this,SLOT(onSliderTorqueMoved(int)));
        sliderTorqueMoved((double)ui->sliderTorqueTorque->value()/10,jointIndex);
    }
    sliderTorquePressed = false;
}


double JointItem::getPositionValue()
{
    return position;
}

double JointItem::getPositionSpeed()
{
    return speed;
}

double JointItem::getPositionSliderSpeed()
{
    return ui->sliderPositionVelocity->value();
}


void JointItem::setMotionDone(bool done)
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

void JointItem::updateSliderLabel(QSlider *slider,QLabel *label,double val)
{
    int w = slider->width() - 30;
    int totValues = slider->maximum() + abs(slider->minimum());
    double newX = ((double)w/(double)totValues) * ((double)val + abs(slider->minimum()));
    label->setGeometry(newX,0,30,20);
    label->setText(QString("%1").arg((int)val));
}

void JointItem::updateSliderTorqueLabel(QSlider *slider,QLabel *label,double val)
{
    int w = slider->width() - 30;
    int totValues = slider->maximum() + abs(slider->minimum());
    double newX = ((double)w/(double)totValues) * ((double)val + abs(slider->minimum()));
    label->setGeometry(newX,0,30,20);
    label->setText(QString("%L1").arg((double)val/10,0,'f',1));
}

void JointItem::updateSlider(QSlider *slider, QLabel *label, double val)
{
    if(sliderPositionPressed || !motionDone){
        return;
    }
    slider->setValue(val);
    updateSliderLabel(slider,label,val);
}

void JointItem::updateSliderTorque(double val)
{
    if(sliderTorquePressed){
        return;
    }
    ui->sliderTorqueTorque->setValue(val*10);
    updateSliderTorqueLabel(ui->sliderTorqueTorque,ui->labelTorqueTorque,val*10);
}


void JointItem::setOpenLoop(double openLoopValue)
{
    if(sliderOpenloopPressed){
        return;
    }
    if(ui->stackedWidget->currentIndex() == OPENLOOP){
        updateSlider(ui->sliderOpenloopOutput,ui->labelOpenLoopOutput,openLoopValue);
    }
}

void JointItem::setPosition(double val)
{
    position = val;


    position = floor(val * 10)/10;
    QString sVal = QString("%1").arg(position);

    if(ui->stackedWidget->currentIndex() == IDLE){
        ui->editIdleCurrentPos->setText(sVal);
    }

    if(ui->stackedWidget->currentIndex() == POSITION){
        ui->editPositionCurrentPos->setText(sVal);
        updateSlider(ui->sliderPositionPosition,ui->labelPositionPosition,val);
    }

    if(ui->stackedWidget->currentIndex() == POSITION_DIR){
        ui->editPositionDirCurrentPos->setText(sVal);
        updateSlider(ui->sliderPositionDirect,ui->labelPositionDirectPosition,val);
    }

    if(ui->stackedWidget->currentIndex() == MIXED){
        ui->editMixedCurrentPos->setText(sVal);
        updateSlider(ui->sliderMixedPosition,ui->labelMixedPosition,val);
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
    }
}

void JointItem::setTorque(double val)
{
    torque = val;
    QString sVal = QString("%L1").arg(torque,0,'f',3);

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

    speed = val;
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
                if(ui->groupBox->title() != QString("JOINT %1").arg(jointIndex)){
                    ui->groupBox->setTitle(QString("JOINT %1").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  UNKNOWN").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  CONFIGURED").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  NOT CONFIGURED").arg(jointIndex));
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

    case CalibDone:{
        ui->groupBox->setTitle(QString("JOINT %1  -  CALIBRATING DONE").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  CALIBRATING").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  HARDWARE FAULT").arg(jointIndex));
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
        ui->groupBox->setTitle(QString("JOINT %1  -  DISCONNECTED").arg(jointIndex));
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
    int sliderMin = 1;
    int sliderMax = 2;

    if(min < max){
        sliderMin = min;
        sliderMax = max;
    }
    ui->sliderMixedPosition->setRange(sliderMin,sliderMax);
    ui->sliderPositionPosition->setRange(sliderMin,sliderMax);
    ui->sliderPositionDirect->setRange(sliderMin,sliderMax);
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
    ui->sliderPositionDirect->setEnabled(false);
    ui->sliderPositionPosition->setEnabled(false);
    ui->sliderPositionVelocity->setEnabled(false);
    ui->sliderTorqueTorque->setEnabled(false);

}

void JointItem::sequenceStopped()
{
    ui->sliderMixedPosition->setEnabled(true);
    ui->sliderMixedVelocity->setEnabled(true);
    ui->sliderOpenloopOutput->setEnabled(true);
    ui->sliderPositionDirect->setEnabled(true);
    ui->sliderPositionPosition->setEnabled(true);
    ui->sliderPositionVelocity->setEnabled(true);
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

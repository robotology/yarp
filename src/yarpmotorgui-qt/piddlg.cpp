/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#include "piddlg.h"
#include "ui_piddlg.h"
#include <QDebug>
#include <QPushButton>

#define     TAB_POSITION    0
#define     TAB_TORQUE      1
#define     TAB_STIFF       2
#define     TAB_OPENLOOP    3

#define     POSITION_KP         0
#define     POSITION_KD         1
#define     POSITION_KI         2
#define     POSITION_SCALE      3
#define     POSITION_MAXOUTPUT  4
#define     POSITION_MAXINT     5
#define     POSITION_OFFSET     6
#define     POSITION_STICTIONUP 7
#define     POSITION_STICTIONDW 8

#define     TORQUE_KP           0
#define     TORQUE_KD           1
#define     TORQUE_KI           2
#define     TORQUE_SCALE        3
#define     TORQUE_MAXOUTPUT    4
#define     TORQUE_MAXINT       5
#define     TORQUE_OFFSET       6
#define     TORQUE_STITCTIONUP  7
#define     TORQUE_STICTIONDW   8
#define     TORQUE_KFF          9
#define     TORQUE_BEMFGAIN     10

PidDlg::PidDlg(QString partname, int jointIndex,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PidDlg)
{
    ui->setupUi(this);

    this->jointIndex = jointIndex;

    QString title = QString("Pid Control %1 JNT:%2").arg(partname).arg(jointIndex);
    setWindowTitle(title);

    connect(ui->btnSend,SIGNAL(clicked()),this,SLOT(onSend()));
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(onCancel()));

    ui->tablePosition->setItemDelegate(new TableDoubleDelegate);
    ui->tableTorque->setItemDelegate(new TableDoubleDelegate);
    ui->tableStiffness->setItemDelegate(new TableDoubleDelegate);
    ui->tableOpenloop->setItemDelegate(new TableDoubleDelegate);

}

PidDlg::~PidDlg()
{
    delete ui;
}

void PidDlg::initPosition(Pid myPid)
{
    ui->tablePosition->item(POSITION_KP,0)->setText(QString("%1").arg((int)myPid.kp));
    ui->tablePosition->item(POSITION_KP,1)->setText(QString("%1").arg((int)myPid.kp));

    ui->tablePosition->item(POSITION_KD,0)->setText(QString("%1").arg((int)myPid.kd));
    ui->tablePosition->item(POSITION_KD,1)->setText(QString("%1").arg((int)myPid.kd));

    ui->tablePosition->item(POSITION_KI,0)->setText(QString("%1").arg((int)myPid.ki));
    ui->tablePosition->item(POSITION_KI,1)->setText(QString("%1").arg((int)myPid.ki));

    ui->tablePosition->item(POSITION_SCALE,0)->setText(QString("%1").arg((int)myPid.scale));
    ui->tablePosition->item(POSITION_SCALE,1)->setText(QString("%1").arg((int)myPid.scale));

    ui->tablePosition->item(POSITION_OFFSET,0)->setText(QString("%1").arg((int)myPid.offset));
    ui->tablePosition->item(POSITION_OFFSET,1)->setText(QString("%1").arg((int)myPid.offset));

    ui->tablePosition->item(POSITION_STICTIONUP,0)->setText(QString("%1").arg((int)myPid.stiction_up_val));
    ui->tablePosition->item(POSITION_STICTIONUP,1)->setText(QString("%1").arg((int)myPid.stiction_up_val));

    ui->tablePosition->item(POSITION_MAXOUTPUT,0)->setText(QString("%1").arg((int)myPid.max_output));
    ui->tablePosition->item(POSITION_MAXOUTPUT,1)->setText(QString("%1").arg((int)myPid.max_output));

    ui->tablePosition->item(POSITION_STICTIONDW,0)->setText(QString("%1").arg((int)myPid.stiction_down_val));
    ui->tablePosition->item(POSITION_STICTIONDW,1)->setText(QString("%1").arg((int)myPid.stiction_down_val));

    ui->tablePosition->item(POSITION_MAXINT,0)->setText(QString("%1").arg((int)myPid.max_int));
    ui->tablePosition->item(POSITION_MAXINT,1)->setText(QString("%1").arg((int)myPid.max_int));
}

void PidDlg::initTorque(Pid myPid, MotorTorqueParameters TrqParam)
{
    ui->tableTorque->item(TORQUE_KP,0)->setText(QString("%1").arg((double)myPid.kp));
    ui->tableTorque->item(TORQUE_KP,1)->setText(QString("%1").arg((double)myPid.kp));

    ui->tableTorque->item(TORQUE_KFF,0)->setText(QString("%1").arg((double)myPid.kff));
    ui->tableTorque->item(TORQUE_KFF,1)->setText(QString("%1").arg((double)myPid.kff));

    ui->tableTorque->item(TORQUE_KD,0)->setText(QString("%1").arg((double)myPid.kd));
    ui->tableTorque->item(TORQUE_KD,1)->setText(QString("%1").arg((double)myPid.kd));

    ui->tableTorque->item(TORQUE_BEMFGAIN,0)->setText(QString("%1").arg((double)TrqParam.bemf));
    ui->tableTorque->item(TORQUE_BEMFGAIN,1)->setText(QString("%1").arg((double)TrqParam.bemf));

    ui->tableTorque->item(TORQUE_KI,0)->setText(QString("%1").arg((double)myPid.ki));
    ui->tableTorque->item(TORQUE_KI,1)->setText(QString("%1").arg((double)myPid.ki));

    ui->tableTorque->item(TORQUE_SCALE,0)->setText(QString("%1").arg((int)myPid.scale));
    ui->tableTorque->item(TORQUE_SCALE,1)->setText(QString("%1").arg((int)myPid.scale));

    ui->tableTorque->item(TORQUE_OFFSET,0)->setText(QString("%1").arg((int)myPid.offset));
    ui->tableTorque->item(TORQUE_OFFSET,1)->setText(QString("%1").arg((int)myPid.offset));

    ui->tableTorque->item(TORQUE_STITCTIONUP,0)->setText(QString("%1").arg((int)myPid.stiction_up_val));
    ui->tableTorque->item(TORQUE_STITCTIONUP,1)->setText(QString("%1").arg((int)myPid.stiction_up_val));

    ui->tableTorque->item(TORQUE_MAXOUTPUT,0)->setText(QString("%1").arg((int)myPid.max_output));
    ui->tableTorque->item(TORQUE_MAXOUTPUT,1)->setText(QString("%1").arg((int)myPid.max_output));

    ui->tableTorque->item(TORQUE_STICTIONDW,0)->setText(QString("%1").arg((int)myPid.stiction_down_val));
    ui->tableTorque->item(TORQUE_STICTIONDW,1)->setText(QString("%1").arg((int)myPid.stiction_down_val));

    ui->tableTorque->item(TORQUE_MAXINT,0)->setText(QString("%1").arg((int)myPid.max_int));
    ui->tableTorque->item(TORQUE_MAXINT,1)->setText(QString("%1").arg((int)myPid.max_int));
}

void PidDlg::initStiffness(double curStiffVal, double minStiff, double maxStiff,
                           double curDampVal, double minDamp, double maxDamp,
                           double curForceVal, double minForce, double maxForce)
{
    ui->tableStiffness->item(0,0)->setText(QString("%L1").arg(curStiffVal,0,'f',3));
    ui->tableStiffness->item(0,1)->setText(QString("%L1").arg(minStiff,0,'f',3));
    ui->tableStiffness->item(0,2)->setText(QString("%L1").arg(maxStiff,0,'f',3));
    ui->tableStiffness->item(0,3)->setText(QString("%L1").arg(curStiffVal,0,'f',3));

    ui->tableStiffness->item(1,0)->setText(QString("%L1").arg(curDampVal,0,'f',3));
    ui->tableStiffness->item(1,1)->setText(QString("%L1").arg(minDamp,0,'f',3));
    ui->tableStiffness->item(1,2)->setText(QString("%L1").arg(maxDamp,0,'f',3));
    ui->tableStiffness->item(1,3)->setText(QString("%L1").arg(curDampVal,0,'f',3));

    ui->tableStiffness->item(2,0)->setText(QString("%L1").arg(curForceVal,0,'f',3));
    ui->tableStiffness->item(2,1)->setText(QString("%L1").arg(minForce,0,'f',3));
    ui->tableStiffness->item(2,2)->setText(QString("%L1").arg(maxForce,0,'f',3));
    ui->tableStiffness->item(2,3)->setText(QString("%L1").arg(curForceVal,0,'f',3));
}


void PidDlg::initOpenLoop(double openLoopVal, double pwm)
{
    ui->tableOpenloop->item(0,0)->setText(QString("%1").arg((int)openLoopVal));
    ui->tableOpenloop->item(0,1)->setText(QString("%1").arg((int)openLoopVal));

    ui->tableOpenloop->item(1,0)->setText(QString("%1").arg(pwm));
}

void PidDlg::onSend()
{
    Pid newPid;
    MotorTorqueParameters newMotorTorqueParams;

    switch (ui->tabWidget->currentIndex()) {
    case TAB_POSITION:
        newPid.kp = ui->tablePosition->item(POSITION_KP,1)->text().toDouble();
        newPid.kd = ui->tablePosition->item(POSITION_KD,1)->text().toDouble();
        newPid.ki = ui->tablePosition->item(POSITION_KI,1)->text().toDouble();
        newPid.scale = ui->tablePosition->item(POSITION_SCALE,1)->text().toDouble();
        newPid.offset = ui->tablePosition->item(POSITION_OFFSET,1)->text().toDouble();
        newPid.stiction_up_val = ui->tablePosition->item(POSITION_STICTIONUP,1)->text().toDouble();
        newPid.max_output = ui->tablePosition->item(POSITION_MAXOUTPUT,1)->text().toDouble();
        newPid.stiction_down_val = ui->tablePosition->item(POSITION_STICTIONDW,1)->text().toDouble();
        newPid.max_int = ui->tablePosition->item(POSITION_MAXINT,1)->text().toDouble();
        sendPid(jointIndex,newPid);
        break;
    case TAB_TORQUE:
        newPid.kp = ui->tableTorque->item(TORQUE_KP,1)->text().toDouble();
        newPid.kff = ui->tableTorque->item(TORQUE_KFF,1)->text().toDouble();
        newPid.kd = ui->tableTorque->item(TORQUE_KD,1)->text().toDouble();
        newMotorTorqueParams.bemf = ui->tableTorque->item(TORQUE_BEMFGAIN,1)->text().toDouble();
        newMotorTorqueParams.bemf_scale = 0;
        newMotorTorqueParams.ktau = 0;
        newMotorTorqueParams.ktau_scale = 0;
        newPid.ki = ui->tableTorque->item(TORQUE_KI,1)->text().toDouble();
        newPid.scale = ui->tableTorque->item(TORQUE_SCALE,1)->text().toDouble();
        newPid.offset = ui->tableTorque->item(TORQUE_OFFSET,1)->text().toDouble();
        newPid.stiction_up_val = ui->tableTorque->item(TORQUE_STITCTIONUP,1)->text().toDouble();
        newPid.max_output = ui->tableTorque->item(TORQUE_MAXOUTPUT,1)->text().toDouble();
        newPid.stiction_down_val = ui->tableTorque->item(TORQUE_STICTIONDW,1)->text().toDouble();
        newPid.max_int = ui->tableTorque->item(TORQUE_MAXINT,1)->text().toDouble();
        sendTorque(jointIndex,newPid,newMotorTorqueParams);
        break;
    case TAB_STIFF:{
        double desiredStiff = ui->tableStiffness->item(0,3)->text().toDouble();
        double desiredDamp = ui->tableStiffness->item(1,3)->text().toDouble();
        double desiredForce = ui->tableStiffness->item(2,3)->text().toDouble();
        sendStiffness(jointIndex,desiredStiff,desiredDamp,desiredForce);
        break;
    }
    case TAB_OPENLOOP:{
        int desiredOpenLoop = ui->tableOpenloop->item(0,1)->text().toDouble();
        sendOpenLoop(jointIndex,desiredOpenLoop);

    }
    default:
        break;
    }


}

void PidDlg::onCancel()
{
    reject();
}






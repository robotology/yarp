/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "piddlg.h"
#include "ui_piddlg.h"
#include <QDebug>
#include <QPushButton>
#include <yarp/os/Bottle.h>
#include <yarp/os/Value.h>

#define     TAB_POSITION    0
#define     TAB_VELOCITY    1
#define     TAB_TORQUE      2
#define     TAB_CURRENT     3
#define     TAB_MOTORPARAMS 4
#define     TAB_STIFF       5
#define     TAB_PWM         6
#define     TAB_VARIABLES   7

#define     POSITION_KP         0
#define     POSITION_KD         1
#define     POSITION_KI         2
#define     POSITION_SCALE      3
#define     POSITION_MAXOUTPUT  4
#define     POSITION_MAXINT     5
#define     POSITION_OFFSET     6
#define     POSITION_STICTIONUP 7
#define     POSITION_STICTIONDW 8
#define     POSITION_KFF        9

#define     VELOCITY_KP         0
#define     VELOCITY_KD         1
#define     VELOCITY_KI         2
#define     VELOCITY_SCALE      3
#define     VELOCITY_MAXOUTPUT  4
#define     VELOCITY_MAXINT     5
#define     VELOCITY_OFFSET     6
#define     VELOCITY_STICTIONUP 7
#define     VELOCITY_STICTIONDW 8
#define     VELOCITY_KFF        9

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

#define     MOTOR_BEMFGAIN     0
#define     MOTOR_BEMFSCALE    1
#define     MOTOR_KTAUGAIN     2
#define     MOTOR_KTAUSCALE    3
#define     MOTOR_VISCOUSPOS   4
#define     MOTOR_VISCOUSNEG   5
#define     MOTOR_COULOMBPOS   6
#define     MOTOR_COULOMBNEG   7
#define     MOTOR_VELOCITY_THRESHOLD  8

#define     CURRENT_KP         0
#define     CURRENT_KD         1
#define     CURRENT_KI         2
#define     CURRENT_SCALE      3
#define     CURRENT_MAXOUTPUT  4
#define     CURRENT_MAXINT     5
#define     CURRENT_OFFSET     6
#define     CURRENT_KFF        7

PidDlg::PidDlg(QString partname, int jointIndex, QString jointName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PidDlg)
{
    ui->setupUi(this);

    this->jointIndex = jointIndex;

    QString title = QString("Pid Control %1 JNT:%2 (%3)").arg(partname).arg(jointIndex).arg(jointName);
    setWindowTitle(title);

    connect(ui->btnRefresh, SIGNAL(clicked()), this, SLOT(onRefresh()));
    connect(ui->btnSend,SIGNAL(clicked()),this,SLOT(onSend()));
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(onCancel()));
    connect(ui->btnDump, SIGNAL(clicked()), this, SLOT(onDumpRemoteVariables()));
    connect(ui->selectPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChangedPos(int)));
    connect(ui->selectVelocity, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChangedVel(int)));
    connect(ui->selectTorque  , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChangedTrq(int)));
    connect(ui->selectCurrent,  SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChangedCur(int)));

    connect(ui->tableStiffness, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item)
    {
        if (item == ui->tableStiffness->item(2, 3))
        {
            onForceOffsetChanged(item->text());
        }
    });

    ui->tablePosition->setItemDelegate(new TableDoubleDelegate);
    ui->tableVelocity->setItemDelegate(new TableDoubleDelegate);
    ui->tableTorque->setItemDelegate(new TableDoubleDelegate);
    ui->tableStiffness->setItemDelegate(new TableDoubleDelegate);
    ui->tablePWM->setItemDelegate(new TableDoubleDelegate);
    ui->tableCurrent->setItemDelegate(new TableDoubleDelegate);
    ui->tableCurrent->setItemDelegate(new TableGenericDelegate);
}

void PidDlg::onForceOffsetChanged(QString forceOffset)
{
    forceOffsetChanged = true;
}

void PidDlg::onDumpRemoteVariables()
{
    emit dumpRemoteVariables();
}

PidDlg::~PidDlg()
{
    for (auto& button : buttons)
    {
        delete button;
        button = nullptr;
    }
    buttons.clear();
    delete ui;
}

std::string prepareDescription(const Pid& myPid)
{
    return "Description: " + myPid.info.pid_description + "\n" +
           "Input:       " + myPid.info.input_data_description + "\n" +
           "Output:      " + myPid.info.output_data_description;
}

void PidDlg::initPositionPID(const std::vector<Pid>& myPids)
{
    size_t pidindex = ui->selectPosition->currentIndex();
    std::string desc = prepareDescription(myPids[pidindex]);
    ui->descPosition->setText(QString("%1").arg(desc.c_str()));

    ui->tablePosition->item(POSITION_KP,0)->setText(QString("%1").arg((double)myPids[pidindex].kp));
    ui->tablePosition->item(POSITION_KP,1)->setText(QString("%1").arg((double)myPids[pidindex].kp));

    ui->tablePosition->item(POSITION_KD,0)->setText(QString("%1").arg((double)myPids[pidindex].kd));
    ui->tablePosition->item(POSITION_KD,1)->setText(QString("%1").arg((double)myPids[pidindex].kd));

    ui->tablePosition->item(POSITION_KI,0)->setText(QString("%1").arg((double)myPids[pidindex].ki));
    ui->tablePosition->item(POSITION_KI,1)->setText(QString("%1").arg((double)myPids[pidindex].ki));

    ui->tablePosition->item(POSITION_SCALE,0)->setText(QString("%1").arg((int)myPids[pidindex].scale));
    ui->tablePosition->item(POSITION_SCALE,1)->setText(QString("%1").arg((int)myPids[pidindex].scale));

    ui->tablePosition->item(POSITION_OFFSET,0)->setText(QString("%1").arg((int)myPids[pidindex].offset));
    ui->tablePosition->item(POSITION_OFFSET,1)->setText(QString("%1").arg((int)myPids[pidindex].offset));

    ui->tablePosition->item(POSITION_STICTIONUP,0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));
    ui->tablePosition->item(POSITION_STICTIONUP,1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));

    ui->tablePosition->item(POSITION_MAXOUTPUT,0)->setText(QString("%1").arg((int)myPids[pidindex].max_output));
    ui->tablePosition->item(POSITION_MAXOUTPUT,1)->setText(QString("%1").arg((int)myPids[pidindex].max_output));

    ui->tablePosition->item(POSITION_STICTIONDW,0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));
    ui->tablePosition->item(POSITION_STICTIONDW,1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));

    ui->tablePosition->item(POSITION_MAXINT,0)->setText(QString("%1").arg((int)myPids[pidindex].max_int));
    ui->tablePosition->item(POSITION_MAXINT,1)->setText(QString("%1").arg((int)myPids[pidindex].max_int));

    ui->tablePosition->item(POSITION_KFF,0)->setText(QString("%1").arg((double)myPids[pidindex].kff));
    ui->tablePosition->item(POSITION_KFF,1)->setText(QString("%1").arg((double)myPids[pidindex].kff));
}

void PidDlg::initVelocityPID(const std::vector<Pid>& myPids)
{
    size_t pidindex = ui->selectVelocity->currentIndex();
    std::string desc = prepareDescription(myPids[pidindex]);
    ui->descVelocity->setText(QString("%1").arg(desc.c_str()));

    ui->tableVelocity->item(VELOCITY_KP, 0)->setText(QString("%1").arg((double)myPids[pidindex].kp));
    ui->tableVelocity->item(VELOCITY_KP, 1)->setText(QString("%1").arg((double)myPids[pidindex].kp));

    ui->tableVelocity->item(VELOCITY_KD, 0)->setText(QString("%1").arg((double)myPids[pidindex].kd));
    ui->tableVelocity->item(VELOCITY_KD, 1)->setText(QString("%1").arg((double)myPids[pidindex].kd));

    ui->tableVelocity->item(VELOCITY_KI, 0)->setText(QString("%1").arg((double)myPids[pidindex].ki));
    ui->tableVelocity->item(VELOCITY_KI, 1)->setText(QString("%1").arg((double)myPids[pidindex].ki));

    ui->tableVelocity->item(VELOCITY_SCALE, 0)->setText(QString("%1").arg((int)myPids[pidindex].scale));
    ui->tableVelocity->item(VELOCITY_SCALE, 1)->setText(QString("%1").arg((int)myPids[pidindex].scale));

    ui->tableVelocity->item(VELOCITY_OFFSET, 0)->setText(QString("%1").arg((int)myPids[pidindex].offset));
    ui->tableVelocity->item(VELOCITY_OFFSET, 1)->setText(QString("%1").arg((int)myPids[pidindex].offset));

    ui->tableVelocity->item(VELOCITY_STICTIONUP, 0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));
    ui->tableVelocity->item(VELOCITY_STICTIONUP, 1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));

    ui->tableVelocity->item(VELOCITY_MAXOUTPUT, 0)->setText(QString("%1").arg((int)myPids[pidindex].max_output));
    ui->tableVelocity->item(VELOCITY_MAXOUTPUT, 1)->setText(QString("%1").arg((int)myPids[pidindex].max_output));

    ui->tableVelocity->item(VELOCITY_STICTIONDW, 0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));
    ui->tableVelocity->item(VELOCITY_STICTIONDW, 1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));

    ui->tableVelocity->item(VELOCITY_MAXINT, 0)->setText(QString("%1").arg((int)myPids[pidindex].max_int));
    ui->tableVelocity->item(VELOCITY_MAXINT, 1)->setText(QString("%1").arg((int)myPids[pidindex].max_int));

    ui->tableVelocity->item(VELOCITY_KFF,0)->setText(QString("%1").arg((double)myPids[pidindex].kff));
    ui->tableVelocity->item(VELOCITY_KFF,1)->setText(QString("%1").arg((double)myPids[pidindex].kff));
}

void PidDlg::initTorquePID(const std::vector<Pid>& myPids)
{
    size_t pidindex = ui->selectTorque->currentIndex();
    std::string desc = prepareDescription(myPids[pidindex]);
    ui->descTorque->setText(QString("%1").arg(desc.c_str()));

    ui->tableTorque->item(TORQUE_KP,0)->setText(QString("%1").arg((double)myPids[pidindex].kp));
    ui->tableTorque->item(TORQUE_KP,1)->setText(QString("%1").arg((double)myPids[pidindex].kp));

    ui->tableTorque->item(TORQUE_KD,0)->setText(QString("%1").arg((double)myPids[pidindex].kd));
    ui->tableTorque->item(TORQUE_KD,1)->setText(QString("%1").arg((double)myPids[pidindex].kd));

    ui->tableTorque->item(TORQUE_KI,0)->setText(QString("%1").arg((double)myPids[pidindex].ki));
    ui->tableTorque->item(TORQUE_KI,1)->setText(QString("%1").arg((double)myPids[pidindex].ki));

    ui->tableTorque->item(TORQUE_SCALE,0)->setText(QString("%1").arg((int)myPids[pidindex].scale));
    ui->tableTorque->item(TORQUE_SCALE,1)->setText(QString("%1").arg((int)myPids[pidindex].scale));

    ui->tableTorque->item(TORQUE_OFFSET,0)->setText(QString("%1").arg((int)myPids[pidindex].offset));
    ui->tableTorque->item(TORQUE_OFFSET,1)->setText(QString("%1").arg((int)myPids[pidindex].offset));

    ui->tableTorque->item(TORQUE_STITCTIONUP,0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));
    ui->tableTorque->item(TORQUE_STITCTIONUP,1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_up_val));

    ui->tableTorque->item(TORQUE_MAXOUTPUT,0)->setText(QString("%1").arg((int)myPids[pidindex].max_output));
    ui->tableTorque->item(TORQUE_MAXOUTPUT,1)->setText(QString("%1").arg((int)myPids[pidindex].max_output));

    ui->tableTorque->item(TORQUE_STICTIONDW,0)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));
    ui->tableTorque->item(TORQUE_STICTIONDW,1)->setText(QString("%1").arg((double)myPids[pidindex].stiction_down_val));

    ui->tableTorque->item(TORQUE_MAXINT,0)->setText(QString("%1").arg((int)myPids[pidindex].max_int));
    ui->tableTorque->item(TORQUE_MAXINT,1)->setText(QString("%1").arg((int)myPids[pidindex].max_int));

    ui->tableTorque->item(TORQUE_KFF,0)->setText(QString("%1").arg((double)myPids[pidindex].kff));
    ui->tableTorque->item(TORQUE_KFF,1)->setText(QString("%1").arg((double)myPids[pidindex].kff));
}

void PidDlg::initMotorParams(MotorTorqueParameters MotParam)
{
    ui->tableMotorParams->item(MOTOR_BEMFGAIN,0)->setText(QString("%1").arg((double)MotParam.bemf));
    ui->tableMotorParams->item(MOTOR_BEMFGAIN,1)->setText(QString("%1").arg((double)MotParam.bemf));

    ui->tableMotorParams->item(MOTOR_BEMFSCALE,0)->setText(QString("%1").arg((int)MotParam.bemf_scale));
    ui->tableMotorParams->item(MOTOR_BEMFSCALE,1)->setText(QString("%1").arg((int)MotParam.bemf_scale));

    ui->tableMotorParams->item(MOTOR_KTAUGAIN,0)->setText(QString("%1").arg((double)MotParam.ktau));
    ui->tableMotorParams->item(MOTOR_KTAUGAIN,1)->setText(QString("%1").arg((double)MotParam.ktau));

    ui->tableMotorParams->item(MOTOR_KTAUSCALE,0)->setText(QString("%1").arg((int)MotParam.ktau_scale));
    ui->tableMotorParams->item(MOTOR_KTAUSCALE,1)->setText(QString("%1").arg((int)MotParam.ktau_scale));

    ui->tableMotorParams->item(MOTOR_VISCOUSPOS, 0)->setText(QString("%1").arg((double)MotParam.viscousPos));
    ui->tableMotorParams->item(MOTOR_VISCOUSPOS, 1)->setText(QString("%1").arg((double)MotParam.viscousPos));
    ui->tableMotorParams->item(MOTOR_VISCOUSNEG, 0)->setText(QString("%1").arg((double)MotParam.viscousNeg));
    ui->tableMotorParams->item(MOTOR_VISCOUSNEG, 1)->setText(QString("%1").arg((double)MotParam.viscousNeg));

    ui->tableMotorParams->item(MOTOR_COULOMBPOS, 0)->setText(QString("%1").arg((double)MotParam.coulombPos));
    ui->tableMotorParams->item(MOTOR_COULOMBPOS, 1)->setText(QString("%1").arg((double)MotParam.coulombPos));
    ui->tableMotorParams->item(MOTOR_COULOMBNEG, 0)->setText(QString("%1").arg((double)MotParam.coulombNeg));
    ui->tableMotorParams->item(MOTOR_COULOMBNEG, 1)->setText(QString("%1").arg((double)MotParam.coulombNeg));

    ui->tableMotorParams->item(MOTOR_VELOCITY_THRESHOLD, 0)->setText(QString("%1").arg((double)MotParam.velocityThres));
    ui->tableMotorParams->item(MOTOR_VELOCITY_THRESHOLD, 1)->setText(QString("%1").arg((double)MotParam.velocityThres));
}

void PidDlg::onSendRemoteVariable()
{
    int i = -1;
    for (size_t elem = 0; elem < buttons.size(); elem++)
    {
        if (sender() == buttons[elem])
        {
            i = elem;
            break;
        }
    }
    if (i == -1) {
        return;
    }

    std::string key = ui->tableVariables->item(i, 0)->text().toStdString();
    std::string val = ui->tableVariables->item(i, 1)->text().toStdString();
    yarp::os::Bottle valb(val);
    emit sendSingleRemoteVariable(key, valb);
    emit updateAllRemoteVariables();
}

void PidDlg::initRemoteVariables(IRemoteVariables* iVar)
{
    // Remote Variables
    if (iVar)
    {
        ui->tableVariables->clear();
        ui->tableVariables->clearContents();
        ui->tableVariables->setRowCount(0);
        ui->tableVariables->setColumnCount(0);
        ui->tableVariables->insertColumn(0);
        ui->tableVariables->insertColumn(0);
        ui->tableVariables->insertColumn(0);
        ui->tableVariables->setMinimumWidth(500);
        ui->tableVariables->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Key")));
        ui->tableVariables->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Values")));
        ui->tableVariables->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("")));

        yarp::os::Bottle keys;
        if (iVar->getRemoteVariablesList(&keys))
        {
            std::string s = keys.toString();
            int keys_size = keys.size();
            for (auto& button : buttons)
            {
                delete button;
                button = nullptr;
            }
            buttons.clear();
            buttons.resize(keys_size);

            for (int i = 0; i < keys_size; i++)
            {
                buttons[i] = new QPushButton("Send");
                connect(buttons[i], SIGNAL(clicked()), this, SLOT(onSendRemoteVariable()));

                std::string v;
                if (keys.get(i).isString())
                {
                    yarp::os::Bottle val;
                    v = keys.get(i).asString();
                    iVar->getRemoteVariable(v, val);
                    ui->tableVariables->insertRow(i);
                    ui->tableVariables->setItem(i, 0, new QTableWidgetItem(QString(v.c_str())));
                    ui->tableVariables->item(i, 0)->setFlags(Qt::NoItemFlags);
                    ui->tableVariables->setItem(i, 1, new QTableWidgetItem(QString(val.toString().c_str())));
                    ui->tableVariables->item(i, 1)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled );
                    ui->tableVariables->setColumnWidth(1, 500);
                    ui->tableVariables->setCellWidget(i, 2, (QWidget*)buttons[i]);

                    /*yarp::os::Bottle val;
                    v = keys.get(i).asString();
                    iVar->getRemoteVariable(v, val);
                    int valsize = val.get(0).size();
                    ui->tableVariables->insertRow(i);
                    ui->tableVariables->setItem(i, 0, new QTableWidgetItem(QString(v.c_str())));
                    ui->tableVariables->item(i, 0)->setFlags(Qt::NoItemFlags);
                    for (int j = 0; j < valsize; j++)
                    {
                        ui->tableVariables->setItem(i, j+1, new QTableWidgetItem(QString(val.get(j).toString().c_str())));
                    }*/
                }
            }
        }
    }
}

void PidDlg::initStiffness(double curStiffVal, double minStiff, double maxStiff,
                           double curDampVal, double minDamp, double maxDamp)
{
    ui->tableStiffness->item(0,0)->setText(QString("%L1").arg(curStiffVal,0,'f',3));
    ui->tableStiffness->item(0,1)->setText(QString("%L1").arg(minStiff,0,'f',3));
    ui->tableStiffness->item(0,2)->setText(QString("%L1").arg(maxStiff,0,'f',3));
    ui->tableStiffness->item(0,3)->setText(QString("%L1").arg(curStiffVal,0,'f',3));

    ui->tableStiffness->item(1,0)->setText(QString("%L1").arg(curDampVal,0,'f',3));
    ui->tableStiffness->item(1,1)->setText(QString("%L1").arg(minDamp,0,'f',3));
    ui->tableStiffness->item(1,2)->setText(QString("%L1").arg(maxDamp,0,'f',3));
    ui->tableStiffness->item(1,3)->setText(QString("%L1").arg(curDampVal,0,'f',3));
}

void PidDlg::initTorqueOffset(double curForceVal, double minForce, double maxForce)
{
    ui->tableStiffness->item(2,0)->setText(QString("%L1").arg(curForceVal,0,'f',3));
    ui->tableStiffness->item(2,1)->setText(QString("%L1").arg(minForce,0,'f',3));
    ui->tableStiffness->item(2,2)->setText(QString("%L1").arg(maxForce,0,'f',3));
    ui->tableStiffness->item(2,3)->setText(QString("%L1").arg(curForceVal,0,'f',3));
    forceOffsetChanged=false;
}


void PidDlg::initPWM(double PWMVal, double pwm)
{
    ui->tablePWM->item(0, 0)->setText(QString("%1").arg((double)PWMVal));
    ui->tablePWM->item(0, 1)->setText(QString("%1").arg((double)PWMVal));

    ui->tablePWM->item(1,0)->setText(QString("%1").arg(pwm));
}

void PidDlg::initCurrentPID(const std::vector<Pid>& myPids)
{
    size_t pidindex = ui->selectCurrent->currentIndex();
    std::string desc = prepareDescription(myPids[pidindex]);
    ui->descCurrent->setText(QString("%1").arg(desc.c_str()));

    ui->tableCurrent->item(CURRENT_KP, 0)->setText(QString("%1").arg((double)myPids[pidindex].kp));
    ui->tableCurrent->item(CURRENT_KP, 1)->setText(QString("%1").arg((double)myPids[pidindex].kp));

    ui->tableCurrent->item(CURRENT_KD, 0)->setText(QString("%1").arg((double)myPids[pidindex].kd));
    ui->tableCurrent->item(CURRENT_KD, 1)->setText(QString("%1").arg((double)myPids[pidindex].kd));

    ui->tableCurrent->item(CURRENT_KI, 0)->setText(QString("%1").arg((double)myPids[pidindex].ki));
    ui->tableCurrent->item(CURRENT_KI, 1)->setText(QString("%1").arg((double)myPids[pidindex].ki));

    ui->tableCurrent->item(CURRENT_SCALE, 0)->setText(QString("%1").arg((int)myPids[pidindex].scale));
    ui->tableCurrent->item(CURRENT_SCALE, 1)->setText(QString("%1").arg((int)myPids[pidindex].scale));

    ui->tableCurrent->item(CURRENT_MAXOUTPUT, 0)->setText(QString("%1").arg((int)myPids[pidindex].max_output));
    ui->tableCurrent->item(CURRENT_MAXOUTPUT, 1)->setText(QString("%1").arg((int)myPids[pidindex].max_output));

    ui->tableCurrent->item(CURRENT_MAXINT, 0)->setText(QString("%1").arg((int)myPids[pidindex].max_int));
    ui->tableCurrent->item(CURRENT_MAXINT, 1)->setText(QString("%1").arg((int)myPids[pidindex].max_int));

    ui->tableCurrent->item(CURRENT_OFFSET, 0)->setText(QString("%1").arg((int)myPids[pidindex].offset));
    ui->tableCurrent->item(CURRENT_OFFSET, 1)->setText(QString("%1").arg((int)myPids[pidindex].offset));

    ui->tableCurrent->item(CURRENT_KFF, 0)->setText(QString("%1").arg((double)myPids[pidindex].kff));
    ui->tableCurrent->item(CURRENT_KFF, 1)->setText(QString("%1").arg((double)myPids[pidindex].kff));
}

void PidDlg::onRefresh()
{
    emit refreshPids(jointIndex);
}

void PidDlg::onComboBoxIndexChangedPos(int selection)
{
    emit refreshPids(jointIndex);
}
void PidDlg::onComboBoxIndexChangedVel(int selection)
{
    emit refreshPids(jointIndex);
}
void PidDlg::onComboBoxIndexChangedTrq(int selection)
{
    emit refreshPids(jointIndex);
}
void PidDlg::onComboBoxIndexChangedCur(int selection)
{
    emit refreshPids(jointIndex);
}


PidControlTypeEnum choosePIDType(const PidControlTypeEnum& base, int index)
{
    std::string pidtyped = yarp::os::Vocab32::decode((yarp::conf::vocab32_t)base);
    pidtyped[3] = index +'0';
    PidControlTypeEnum val = (PidControlTypeEnum)(yarp::os::Vocab32::encode(pidtyped));
    return val;
}

void PidDlg::onSend()
{
    Pid newPid;
    MotorTorqueParameters newMotorTorqueParams;
    PidControlTypeEnum pidtype;
    int combo_selection=0;

    switch (ui->tabMain->currentIndex()) {
    case TAB_POSITION:
        combo_selection = ui->selectPosition->currentIndex();
        pidtype = choosePIDType(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION, combo_selection+1);
        newPid.kp = ui->tablePosition->item(POSITION_KP,1)->text().toDouble();
        newPid.kd = ui->tablePosition->item(POSITION_KD,1)->text().toDouble();
        newPid.ki = ui->tablePosition->item(POSITION_KI,1)->text().toDouble();
        newPid.scale = ui->tablePosition->item(POSITION_SCALE,1)->text().toDouble();
        newPid.offset = ui->tablePosition->item(POSITION_OFFSET,1)->text().toDouble();
        newPid.stiction_up_val = ui->tablePosition->item(POSITION_STICTIONUP,1)->text().toDouble();
        newPid.max_output = ui->tablePosition->item(POSITION_MAXOUTPUT,1)->text().toDouble();
        newPid.stiction_down_val = ui->tablePosition->item(POSITION_STICTIONDW,1)->text().toDouble();
        newPid.max_int = ui->tablePosition->item(POSITION_MAXINT,1)->text().toDouble();
        emit sendPid(pidtype, jointIndex,newPid);
        break;
    case TAB_VELOCITY:
        combo_selection = ui->selectVelocity->currentIndex();
        pidtype = choosePIDType(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY, combo_selection+1);
        newPid.kp = ui->tableVelocity->item(VELOCITY_KP, 1)->text().toDouble();
        newPid.kd = ui->tableVelocity->item(VELOCITY_KD, 1)->text().toDouble();
        newPid.ki = ui->tableVelocity->item(VELOCITY_KI, 1)->text().toDouble();
        newPid.scale = ui->tableVelocity->item(VELOCITY_SCALE, 1)->text().toDouble();
        newPid.offset = ui->tableVelocity->item(VELOCITY_OFFSET, 1)->text().toDouble();
        newPid.stiction_up_val = ui->tableVelocity->item(VELOCITY_STICTIONUP, 1)->text().toDouble();
        newPid.max_output = ui->tableVelocity->item(VELOCITY_MAXOUTPUT, 1)->text().toDouble();
        newPid.stiction_down_val = ui->tableVelocity->item(VELOCITY_STICTIONDW, 1)->text().toDouble();
        newPid.max_int = ui->tableVelocity->item(VELOCITY_MAXINT, 1)->text().toDouble();
        emit sendPid(pidtype, jointIndex, newPid);
        break;
    case TAB_TORQUE:
        combo_selection = ui->selectTorque->currentIndex();
        pidtype = choosePIDType(PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE, combo_selection+1);
        newPid.kp = ui->tableTorque->item(TORQUE_KP,1)->text().toDouble();
        newPid.kff = ui->tableTorque->item(TORQUE_KFF,1)->text().toDouble();
        newPid.kd = ui->tableTorque->item(TORQUE_KD,1)->text().toDouble();
        newPid.ki = ui->tableTorque->item(TORQUE_KI,1)->text().toDouble();
        newPid.scale = ui->tableTorque->item(TORQUE_SCALE,1)->text().toDouble();
        newPid.offset = ui->tableTorque->item(TORQUE_OFFSET,1)->text().toDouble();
        newPid.stiction_up_val = ui->tableTorque->item(TORQUE_STITCTIONUP,1)->text().toDouble();
        newPid.max_output = ui->tableTorque->item(TORQUE_MAXOUTPUT,1)->text().toDouble();
        newPid.stiction_down_val = ui->tableTorque->item(TORQUE_STICTIONDW,1)->text().toDouble();
        newPid.max_int = ui->tableTorque->item(TORQUE_MAXINT,1)->text().toDouble();
        emit sendPid(pidtype, jointIndex,newPid);
        break;
    case TAB_CURRENT:{
        combo_selection = ui->selectCurrent->currentIndex();
        pidtype = choosePIDType(PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT, combo_selection+1);
        newPid.kp = ui->tableCurrent->item(CURRENT_KP, 1)->text().toDouble();
        newPid.kd = ui->tableCurrent->item(CURRENT_KD, 1)->text().toDouble();
        newPid.ki = ui->tableCurrent->item(CURRENT_KI, 1)->text().toDouble();
        newPid.scale = ui->tableCurrent->item(CURRENT_SCALE, 1)->text().toDouble();
        newPid.offset = ui->tableCurrent->item(CURRENT_OFFSET, 1)->text().toDouble();
        newPid.max_output = ui->tableCurrent->item(CURRENT_MAXOUTPUT, 1)->text().toDouble();
        newPid.max_int = ui->tableCurrent->item(CURRENT_MAXINT, 1)->text().toDouble();
        emit sendPid(pidtype, jointIndex, newPid);
        break;
    }
    case TAB_MOTORPARAMS:
        newMotorTorqueParams.bemf = ui->tableMotorParams->item(MOTOR_BEMFGAIN,1)->text().toDouble();
        newMotorTorqueParams.bemf_scale = ui->tableMotorParams->item(MOTOR_BEMFSCALE,1)->text().toDouble();
        newMotorTorqueParams.ktau = ui->tableMotorParams->item(MOTOR_KTAUGAIN,1)->text().toDouble();
        newMotorTorqueParams.ktau_scale = ui->tableMotorParams->item(MOTOR_KTAUSCALE,1)->text().toDouble();
        newMotorTorqueParams.viscousPos = ui->tableMotorParams->item(MOTOR_VISCOUSPOS,1)->text().toDouble();
        newMotorTorqueParams.viscousNeg = ui->tableMotorParams->item(MOTOR_VISCOUSNEG,1)->text().toDouble();
        newMotorTorqueParams.coulombPos = ui->tableMotorParams->item(MOTOR_COULOMBPOS,1)->text().toDouble();
        newMotorTorqueParams.coulombNeg = ui->tableMotorParams->item(MOTOR_COULOMBNEG,1)->text().toDouble();
        newMotorTorqueParams.velocityThres =  ui->tableMotorParams->item(MOTOR_VELOCITY_THRESHOLD,1)->text().toDouble();
        emit sendMotorParameters(jointIndex,newMotorTorqueParams);
        break;
    case TAB_STIFF:{
        double desiredStiff = ui->tableStiffness->item(0,3)->text().toDouble();
        double desiredDamp = ui->tableStiffness->item(1,3)->text().toDouble();
        double desiredForce = ui->tableStiffness->item(2,3)->text().toDouble();
        emit sendStiffness(jointIndex,desiredStiff,desiredDamp);
        if (forceOffsetChanged)
        {
            emit sendForceOffset(jointIndex, desiredForce);
            forceOffsetChanged = false;
        }
        break;
    }
    case TAB_PWM:{
        int desiredDuty = ui->tablePWM->item(0,1)->text().toDouble();
        emit sendPWM(jointIndex,desiredDuty);
        break;
    }
    case TAB_VARIABLES:{
        // Remote Variables
        int rows = ui->tableVariables->rowCount();
        for (int i = 0; i < rows; i++)
        {
            std::string key = ui->tableVariables->item(i, 0)->text().toStdString();
            std::string val = ui->tableVariables->item(i, 1)->text().toStdString();
            yarp::os::Bottle valb(val);
            emit sendSingleRemoteVariable(key, valb);
        }
        emit updateAllRemoteVariables();
        break;
    }
    default:
        break;
    }


}

void PidDlg::onCancel()
{
    reject();
}

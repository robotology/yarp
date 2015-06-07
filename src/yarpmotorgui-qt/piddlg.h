/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef PIDDLG_H
#define PIDDLG_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QModelIndex>
#include <QLineEdit>

#include <yarp/dev/ControlBoardInterfaces.h>

class TableDelegate;
class TableDoubleDelegate;

namespace Ui {
class PidDlg;
}
using namespace yarp::dev;

class PidDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PidDlg(QString partname, int jointIndex,QWidget *parent = 0);
    ~PidDlg();

    void initPosition(Pid myPid);
    void initVelocity(Pid myPid);
    void initTorque(Pid myPid, MotorTorqueParameters TorqueParam);
    void initCurrent(Pid myPid);
    void initStiffness(double curStiffVal, double minStiff, double maxStiff,
                       double curDampVal, double minDamp, double maxDamp,
                       double curForceVal, double minForce, double maxForce);
    void initOpenLoop(double openLoopVal, double pwm);
    void initRemoteVariables(IRemoteVariables* iVar);


signals:
    void sendStiffness(int,double,double,double);
    void sendPositionPid(int jointIndex, Pid pid);
    void sendVelocityPid(int jointIndex, Pid pid);
    void sendTorquePid(int jointIndex,Pid,MotorTorqueParameters newTorqueParam);
    void sendOpenLoop(int jointIndex,int openLoopVal);
    void sendCurrentPid(int jointIndex, Pid pid);
    void sendSingleRemoteVariable(std::string key, yarp::os::Bottle val);
    void updateAllRemoteVariables();

private:
    Ui::PidDlg *ui;
    int jointIndex;

private slots:
    void onSend();
    void onCancel();

};

class TableIntDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                      const QModelIndex & index) const
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        QLineEdit *lineEdit = new QLineEdit(parent);
        // Set validator
        QIntValidator *validator = new QIntValidator(-100000, 100000, lineEdit);
        lineEdit->setValidator(validator);
        return lineEdit;
    }
};

class TableDoubleDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                      const QModelIndex & index) const
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        QLineEdit *lineEdit = new QLineEdit(parent);
        // Set validator
        QDoubleValidator *validator = new QDoubleValidator(-100000, 100000, 4,lineEdit);
        lineEdit->setValidator(validator);
        return lineEdit;
    }
};

class TableGenericDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
        const QModelIndex & index) const
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        QLineEdit *lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
};

#endif // PIDDLG_H

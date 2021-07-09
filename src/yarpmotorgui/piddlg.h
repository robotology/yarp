/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PIDDLG_H
#define PIDDLG_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QModelIndex>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <vector>

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
    explicit PidDlg(QString partname, int jointIndex, QString jointName, QWidget *parent = 0);
    ~PidDlg();

    void initPosition(Pid myPid);
    void initVelocity(Pid myPid);
    void initTorque(Pid myPid, MotorTorqueParameters TorqueParam);
    void initCurrent(Pid myPid);
    void initStiffness(double curStiffVal, double minStiff, double maxStiff,
                       double curDampVal, double minDamp, double maxDamp,
                       double curForceVal, double minForce, double maxForce);
    void initPWM(double pwmVal, double pwm);
    void initRemoteVariables(IRemoteVariables* iVar);


signals:
    void sendStiffness(int,double,double,double);
    void sendPositionPid(int jointIndex, Pid pid);
    void sendVelocityPid(int jointIndex, Pid pid);
    void sendTorquePid(int jointIndex,Pid,MotorTorqueParameters newTorqueParam);
    void sendPWM(int jointIndex,double dutyVal);
    void sendCurrent(int jointIndex,int currentVal);
    void sendCurrentPid(int jointIndex, Pid pid);
    void sendSingleRemoteVariable(std::string key, yarp::os::Bottle val);
    void refreshPids(int jointIndex);
    void updateAllRemoteVariables();
    void dumpRemoteVariables();

private:
    Ui::PidDlg *ui;
    int jointIndex;
    std::vector <QPushButton*> buttons;

private slots:
    void onRefresh();
    void onSend();
    void onCancel();
    void onSendRemoteVariable();
    void onDumpRemoteVariables();
};

class TableIntDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                      const QModelIndex & index) const override
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
                      const QModelIndex & index) const override
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
        const QModelIndex & index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        QLineEdit *lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
};

#endif // PIDDLG_H

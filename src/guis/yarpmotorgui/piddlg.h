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

    void initPositionPID(const std::vector<Pid>& myPids);
    void initVelocityPID(const std::vector<Pid>& myPids);
    void initTorquePID(const std::vector<Pid>& myPids);
    void initMotorParams(MotorTorqueParameters TorqueParam);
    void initCurrentPID(const std::vector<Pid>& myPids);
    void initStiffness(double curStiffVal, double minStiff, double maxStiff,
                       double curDampVal, double minDamp, double maxDamp);
    void initTorqueOffset(double curForceVal, double minForce, double maxForce);
    void initPWM(double pwmVal, double pwm);
    void initRemoteVariables(IRemoteVariables* iVar);


signals:
    void sendStiffness(int,double,double,double);
    void sendForceOffset(int,double);
    void sendPid(PidControlTypeEnum pidtype, int jointIndex, Pid newPid);
    void sendMotorParameters(int jointIndex,MotorTorqueParameters newTorqueParam);
    void sendPWM(int jointIndex,double dutyVal);
    void sendCurrent(int jointIndex,int currentVal);
    void sendSingleRemoteVariable(std::string key, yarp::os::Bottle val);
    void refreshPids(int jointIndex);
    void updateAllRemoteVariables();
    void dumpRemoteVariables();

private:
    Ui::PidDlg *ui;
    int jointIndex;
    std::vector <QPushButton*> buttons;
    bool forceOffsetChanged = false;

private slots:
    void onRefresh();
    void onSend();
    void onCancel();
    void onSendRemoteVariable();
    void onDumpRemoteVariables();
    void onForceOffsetChanged(QString forceOffset);
    void onComboBoxIndexChangedPos(int);
    void onComboBoxIndexChangedVel(int);
    void onComboBoxIndexChangedTrq(int);
    void onComboBoxIndexChangedCur(int);
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

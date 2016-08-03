/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef STARTDLG_H
#define STARTDLG_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class StartDlg;
}

class StartDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StartDlg(QWidget *parent = 0);
    ~StartDlg();

    void init(QString robotName, QStringList partsName, QList<int> ENA);
    QList <int> getEnabledParts();
    QString getRobotName();

private:
    Ui::StartDlg *ui;

    QList <QCheckBox*> checkList;
};

#endif // STARTDLG_H

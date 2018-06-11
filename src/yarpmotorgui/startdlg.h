/*
 * Copyright (C) 2010 RobotCub Consortium
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see LICENSE
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

    void init(QStringList partsName);
    std::vector<bool> getEnabledParts();

private:
    Ui::StartDlg *ui;

    QList <QCheckBox*> checkList;
};

#endif // STARTDLG_H

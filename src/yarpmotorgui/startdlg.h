/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

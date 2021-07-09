/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "startdlg.h"
#include "ui_startdlg.h"
#include "flowlayout.h"


StartDlg::StartDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDlg)
{
    ui->setupUi(this);

    setWindowTitle("Qt Robot Motor GUI V2.0 - Select Parts");
}

StartDlg::~StartDlg()
{
    delete ui;
}


void StartDlg::init(QStringList partsName)
{

    auto* layout = new FlowLayout(ui->groupBox);

    for(int i=0;i<partsName.count();i++)
    {
        auto* check = new QCheckBox(partsName.at(i),ui->groupBox);
        check->setChecked(true);
        check->setMinimumSize(QSize(100, check->height()));
        layout->addWidget(check);
        checkList.append(check);
    }
    adjustSize();

    ui->groupBox->setLayout(layout);
}


std::vector<bool> StartDlg::getEnabledParts()
{
    std::vector<bool> enabled;

    for(int i = 0; i < checkList.count(); i++)
    {
        enabled.push_back(checkList.at(i)->isChecked());
    }

    return enabled;
}

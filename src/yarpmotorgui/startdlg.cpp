/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
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

    FlowLayout *layout = new FlowLayout(ui->groupBox);

    for(int i=0;i<partsName.count();i++)
    {
        QCheckBox *check = new QCheckBox(partsName.at(i),ui->groupBox);
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

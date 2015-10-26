/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
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

    setWindowTitle("Qt Robot Motor GUI V1.0 - Select Parts");
}

StartDlg::~StartDlg()
{
    delete ui;
}


void StartDlg::init(QString robotName,QStringList partsName,QList <int> ENA)
{

    ui->lineEdit->setText(robotName);
    FlowLayout *layout = new FlowLayout(ui->groupBox);

    for(int i=0;i<partsName.count();i++){
        QCheckBox *check = new QCheckBox(partsName.at(i),ui->groupBox);
        check->setChecked(ENA.at(i));
        check->setMinimumSize(QSize(100,check->height()));
        layout->addWidget(check);
        checkList.append(check);
    }
    adjustSize();

    ui->groupBox->setLayout(layout);
}


QList <int> StartDlg::getEnabledParts()
{
    QList <int> enabled;
    for(int i=0; i<checkList.count();i++){
        if(checkList.at(i)->isChecked()){
            enabled.append(1);
        }else{
            enabled.append(0);
        }
    }
    return enabled;
}

QString StartDlg::getRobotName()
{
    return ui->lineEdit->text();
}

/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "clusterWidget.h"
#include "ui_clusterWidget.h"
#include "iostream"
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>

using namespace std;

ClusterWidget::ClusterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterWidget)
{
    ui->setupUi(this);

    ui->gridLayout->addWidget(new QLabel("Name"), 0, 0);
    ui->gridLayout->addWidget(new QLabel("Display"), 0, 1);
    ui->gridLayout->addWidget(new QLabel("User"), 0, 2);
    ui->gridLayout->addWidget(new QLabel("On/off"), 0, 3);
    ui->gridLayout->addWidget(new QLabel("Log"), 0, 4);
    ui->gridLayout->addWidget(new QLabel("Select"), 0, 5);

    //TODO just for debug
    for(int i=0; i<1; i++)
        addRow("ciccio","none","giovanni",true);
}

void ClusterWidget::addRow(string name, string display, string user,
                           bool onOff, bool log, bool select)
{
    int rowCount = ui->gridLayout->rowCount();
    ui->gridLayout->addWidget(new QLineEdit(name.c_str()), rowCount, 0);
    ui->gridLayout->addWidget(new QLineEdit(display.c_str()), rowCount, 1);
    ui->gridLayout->addWidget(new QLineEdit(user.c_str()), rowCount, 2);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 3);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 4);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 5);

    //initialize checkboxes
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setChecked(onOff);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,4)->widget())->setChecked(log);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,5)->widget())->setChecked(select);

    //read only
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,0)->widget())->setReadOnly(true);
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,1)->widget())->setReadOnly(true);
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,2)->widget())->setReadOnly(true);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setAttribute(Qt::WA_TransparentForMouseEvents);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setFocusPolicy(Qt::NoFocus);

}

std::string ClusterWidget::getSSHCmd(std::string user, std::string host, std::string ssh_options)
{
    return "";
}

ClusterWidget::~ClusterWidget()
{
    delete ui;
}

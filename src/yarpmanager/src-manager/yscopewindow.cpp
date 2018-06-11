/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#include "yscopewindow.h"
#include "ui_yscopewindow.h"

YscopeWindow::YscopeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::YscopeWindow)
{
    ui->setupUi(this);
    index = 0;
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));
    connect(ui->inspectBtn,SIGNAL(clicked()),this,SLOT(onInspect()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(onIndexChanged(int)));
}

YscopeWindow::~YscopeWindow()
{
    delete ui;
}

void YscopeWindow::onCancel()
{
    reject();
}

void YscopeWindow::onInspect()
{
    accept();
}

void YscopeWindow::onIndexChanged(int val)
{
    index = val;
}

int YscopeWindow::getIndex()
{
    return index;
}

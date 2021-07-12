/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

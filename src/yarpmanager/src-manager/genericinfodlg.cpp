/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#include "genericinfodlg.h"
#include "ui_genericinfodlg.h"

GenericInfoDlg::GenericInfoDlg(QString title, QString description, QString text,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenericInfoDlg)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->description->setText(description);
    ui->textBrowser->setHtml(text);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(accept()));
}

GenericInfoDlg::~GenericInfoDlg()
{
    delete ui;
}

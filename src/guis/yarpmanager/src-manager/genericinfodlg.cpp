/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

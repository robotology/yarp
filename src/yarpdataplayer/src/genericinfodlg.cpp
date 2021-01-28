/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "include/genericinfodlg.h"
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

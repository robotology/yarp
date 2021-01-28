/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
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

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


void StartDlg::init(std::vector<std::string> partsName)
{
    auto* layout = new FlowLayout(ui->groupBox);

    for(int i=0;i<partsName.size();i++)
    {
        auto* check = new QCheckBox(QString(partsName[i].c_str()), ui->groupBox);
        check->setChecked(true);
        check->setMinimumSize(QSize(100, check->height()));
        layout->addWidget(check);
        checkList.append(check);
    }

    auto* button = new QPushButton("Deselect All",ui->groupBox);
    connect(button,SIGNAL(clicked()),this,SLOT(onSelDesel()));

    button->setFixedSize(QSize(100, button->height()));
    layout->addWidget(button);
    adjustSize();

    ui->groupBox->setLayout(layout);

    auto bb = this->findChildren<QDialogButtonBox *>();
    QPushButton* okBtn = bb[0]->button(QDialogButtonBox::Ok);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    QPushButton* cancelBtn = bb[0]->button(QDialogButtonBox::Cancel);
    cancelBtn->setAutoDefault(false);
    cancelBtn->setDefault(false);

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

void StartDlg::onSelDesel()
{
    auto checkboxes = this->findChildren<QCheckBox *>();
    auto btn = this->findChildren<QPushButton *>();
    bool sel;

    if(btn[0]->text() == "Deselect All"){
        btn[0]->setText("Select All");
        sel = false;
    }
    else{
        btn[0]->setText("Deselect All");
        sel = true;
    }

    for (auto child : checkboxes) {
        child->setChecked(sel);
    }
}

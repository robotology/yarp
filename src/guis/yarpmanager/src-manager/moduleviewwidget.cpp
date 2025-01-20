/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "moduleviewwidget.h"
#include "ui_moduleviewwidget.h"


ModuleViewWidget::ModuleViewWidget(yarp::manager::Module *mod, QWidget *parent) :
    GenericViewWidget(parent),
    ui(new Ui::ModuleViewWidget)
{
    ui->setupUi(this);
    module = mod;
    type = yarp::manager::MODULE;

    QTreeWidgetItem *nameItem = ui->treeWidget->topLevelItem(0);
    QTreeWidgetItem *versionItem = ui->treeWidget->topLevelItem(1);
    QTreeWidgetItem *descriptionItem = ui->treeWidget->topLevelItem(2);
    QTreeWidgetItem *parametersItem = ui->treeWidget->topLevelItem(3);
    QTreeWidgetItem *authorsItem = ui->treeWidget->topLevelItem(4);
    QTreeWidgetItem *inputsItem = ui->treeWidget->topLevelItem(5);
    QTreeWidgetItem *outputsItem = ui->treeWidget->topLevelItem(6);
    //Q_UNUSED(outputsItem);

    nameItem->setText(1,mod->getName());
    versionItem->setText(1,mod->getVersion());
    descriptionItem->setText(1,mod->getDescription());

    for(int i=0;i<mod->argumentCount();i++){
        yarp::manager::Argument a = mod->getArgumentAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(parametersItem,QStringList() << a.getParam() << a.getDescription());
        Q_UNUSED(it);
    }

    for(int i=0;i<mod->authorCount();i++){
        yarp::manager::Author a = mod->getAuthorAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(authorsItem,QStringList() << a.getName() << a.getEmail());
        Q_UNUSED(it);
    }

    for(int i=0;i<mod->inputCount();i++){
        yarp::manager::InputData a = mod->getInputAt(i);

        QTreeWidgetItem *type = new QTreeWidgetItem(inputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        QTreeWidgetItem *req = new QTreeWidgetItem(type,QStringList() << "Required" << (a.isRequired() ? "yes" : "no"));
        Q_UNUSED(port);
        Q_UNUSED(desc);
        Q_UNUSED(req);
    }

    for(int i=0;i<mod->outputCount();i++){
        yarp::manager::OutputData a = mod->getOutputAt(i); //TODO controllare

        QTreeWidgetItem *type = new QTreeWidgetItem(outputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        Q_UNUSED(port);
        Q_UNUSED(desc);
    }



}

ModuleViewWidget::~ModuleViewWidget()
{
    delete ui;
}

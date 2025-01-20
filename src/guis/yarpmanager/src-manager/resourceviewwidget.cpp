/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "resourceviewwidget.h"
#include "ui_resourceviewwidget.h"
#include <QTreeWidgetItem>

ResourceViewWidget::ResourceViewWidget(yarp::manager::Computer *res, QWidget *parent) :
    GenericViewWidget(parent),
    ui(new Ui::ResourceViewWidget)
{
    this->res = res;
    ui->setupUi(this);

    type = yarp::manager::RESOURCE;

    init();

}

ResourceViewWidget::~ResourceViewWidget()
{
    delete ui;
}

void ResourceViewWidget::refresh()
{
    QTreeWidgetItem *peripheralItem = ui->treeWidget->topLevelItem(9);
    while(peripheralItem->childCount() > 0){
        peripheralItem->removeChild(peripheralItem->child(0));
    }

    QTreeWidgetItem *processesItem = ui->treeWidget->topLevelItem(10);
    while(processesItem->childCount() > 0){
        processesItem->removeChild(processesItem->child(0));
    }

    init();
}


void ResourceViewWidget::init()
{
    QTreeWidgetItem *nameItem = ui->treeWidget->topLevelItem(0);
    QTreeWidgetItem *descriptionItem = ui->treeWidget->topLevelItem(1);
    QTreeWidgetItem *disableItem = ui->treeWidget->topLevelItem(2);
    QTreeWidgetItem *availabilityItem = ui->treeWidget->topLevelItem(3);
    QTreeWidgetItem *platformItem = ui->treeWidget->topLevelItem(4);
    QTreeWidgetItem *processorItem = ui->treeWidget->topLevelItem(5);
    QTreeWidgetItem *cpuLoadItem = ui->treeWidget->topLevelItem(6);
    QTreeWidgetItem *memoryItem = ui->treeWidget->topLevelItem(7);
    QTreeWidgetItem *storageItem = ui->treeWidget->topLevelItem(8);
    QTreeWidgetItem *peripheralItem = ui->treeWidget->topLevelItem(9);
    QTreeWidgetItem *processesItem = ui->treeWidget->topLevelItem(10);

    QTreeWidgetItem *platformNameItem = platformItem->child(0);
    QTreeWidgetItem *platformDistributionItem = platformItem->child(1);
    QTreeWidgetItem *platformReleaseItem = platformItem->child(2);

    QTreeWidgetItem *processorModelItem = processorItem->child(0);
    QTreeWidgetItem *processorArchitectureItem = processorItem->child(1);
    QTreeWidgetItem *processorCoresItem = processorItem->child(2);
    QTreeWidgetItem *processorSibilingsItem = processorItem->child(3);
    QTreeWidgetItem *processorFrequencyItem = processorItem->child(4);

    QTreeWidgetItem *cpuLoadInstantItem = cpuLoadItem->child(0);
    QTreeWidgetItem *cpuLoadAvg1Item = cpuLoadItem->child(1);
    QTreeWidgetItem *cpuLoadAvg5Item = cpuLoadItem->child(2);
    QTreeWidgetItem *cpuLoadAvg15Item = cpuLoadItem->child(3);

    QTreeWidgetItem *memoryTotalItem = memoryItem->child(0);
    QTreeWidgetItem *memoryFreeItem = memoryItem->child(1);

    QTreeWidgetItem *storageTotalItem = storageItem->child(0);
    QTreeWidgetItem *storageFreeItem = storageItem->child(1);

    nameItem->setText(1,res->getName());
    descriptionItem->setText(1,res->getDescription());
    disableItem->setText(1,QString("%1").arg((res->getDisable() ? "Yes" : "No")));
    availabilityItem->setText(1,QString("%1").arg((res->getAvailability() ? "Yes" : "No")));

    yarp::manager::Platform plat = res->getPlatform();
    platformNameItem->setText(1,plat.getName());
    platformDistributionItem->setText(1,plat.getDistribution());
    platformReleaseItem->setText(1,plat.getRelease());

    yarp::manager::Processor proc = res->getProcessor();
    processorModelItem->setText(1,proc.getModel());
    processorArchitectureItem->setText(1,proc.getArchitecture());
    processorCoresItem->setText(1,QString("%1").arg(proc.getCores()));
    processorSibilingsItem->setText(1,QString("%1").arg(proc.getSiblings()));
    processorFrequencyItem->setText(1,QString("%1").arg(proc.getFrequency()));

    yarp::manager::LoadAvg avg = proc.getCPULoad();
    cpuLoadInstantItem->setText(1,QString("%1%").arg((avg.loadAverageInstant >=0 ? avg.loadAverageInstant : 0)));
    cpuLoadAvg1Item->setText(1,QString("%1%").arg((avg.loadAverage1 >=0 ? avg.loadAverage1 : 0)));
    cpuLoadAvg5Item->setText(1,QString("%1%").arg((avg.loadAverage5 >=0 ? avg.loadAverage5 : 0)));
    cpuLoadAvg15Item->setText(1,QString("%1%").arg((avg.loadAverage15 >=0 ? avg.loadAverage15 : 0)));

    yarp::manager::Storage storage = res->getStorage();
    storageTotalItem->setText(1,QString("%1 KB").arg(storage.getTotalSpace()));
    storageFreeItem->setText(1,QString("%1 KB").arg(storage.getFreeSpace()));

    yarp::manager::Memory mem = res->getMemory();
    memoryTotalItem->setText(1,QString("%1 KB").arg(mem.getTotalSpace()));
    memoryFreeItem->setText(1,QString("%1 KB").arg(mem.getFreeSpace()));

    for(int i=0; i<res->peripheralCount();i++){
        QTreeWidgetItem *it = new QTreeWidgetItem(peripheralItem,QStringList() << res->getPeripheralAt(i).getName());
        Q_UNUSED(it);
    }

    yarp::manager::ProcessContainer procs = res->getProcesses();

    for(auto p : procs){
        QTreeWidgetItem *it = new QTreeWidgetItem(processesItem,QStringList() << QString("%1").arg(p.pid));
        Q_UNUSED(it);
    }
}

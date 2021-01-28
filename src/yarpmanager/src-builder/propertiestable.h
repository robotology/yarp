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

#ifndef PROPERTIESTABLE_H
#define PROPERTIESTABLE_H

#include <QWidget>
#include <QTreeWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <yarp/manager/application.h>
#include <yarp/manager/module.h>
#include <yarp/manager/manager.h>
#include "moduleitem.h"

using namespace yarp::manager;

#include <QComboBox>
class PropertiesTable : public QWidget
{
    Q_OBJECT
public:
    explicit PropertiesTable(Manager *manager, QWidget *parent = 0);
    void showApplicationTab(Application *application);
    void showModuleTab(ModuleItem *it);
    void addModules(ModuleItem *);


private:
    Application *currentApplication;
    ModuleItem *currentModule;
    QTabWidget *propertiesTab;
    QTreeWidget *appProperties;
    QTreeWidget *moduleProperties;
    QTreeWidget *moduleDescription;

    QTreeWidgetItem *appName;
    QTreeWidgetItem *appDescr;
    QTreeWidgetItem *appVersion;
    QTreeWidgetItem *appAuthors;
    QTreeWidgetItem *appPrefix;

    QTreeWidgetItem *modName;
    QTreeWidgetItem *modNode;
    QTreeWidgetItem *modStdio;
    QTreeWidgetItem *modWorkDir;
    QTreeWidgetItem *modPrefix;
    QTreeWidgetItem *modDeployer;
    QTreeWidgetItem *modParams;

    Manager *manager;

//    QTreeWidgetItem *nameItem;
//    QTreeWidgetItem *versionItem;
//    QTreeWidgetItem *descriptionItem;
//    QTreeWidgetItem *parametersItem;
//    QTreeWidgetItem *authorsItem;
//    QTreeWidgetItem *inputsItem;
//    QTreeWidgetItem *outputsItem;

    QSignalMapper *paramsSignalMapper;
    QComboBox *nodeCombo;
    QComboBox *deployerCombo;

    QString lastPrefix;

    QList <ModuleItem *> modules;


signals:
    void modified();


private slots:
    void onComboChanged(QWidget *);
    void onAppItemDoubleClicked(QTreeWidgetItem*,int);
    void onAppItemChanged(QTreeWidgetItem*,int);
    void onModItemDoubleClicked(QTreeWidgetItem*,int);
    void onModItemChanged(QTreeWidgetItem*,int);
};

#endif // PROPERTIESTABLE_H

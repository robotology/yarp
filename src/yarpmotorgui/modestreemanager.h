/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODESTREEMANAGER_H
#define MODESTREEMANAGER_H

#include <QObject>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QIcon>
#include <QTabWidget>
#include <string>
#include <unordered_map>
#include "partitem.h"
#include "customgroupbox.h"

class ModesListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ModesListWidget(QWidget * parent = 0);
};

class ModesTreeManager : public QObject
{
    Q_OBJECT

public:
    ModesTreeManager(QHBoxLayout* layout, QWidget* parent = nullptr);

    void addRobot(const std::string& robotName);

    void addRobotPart(const std::string& robotName, const std::string &partName, int partIndex, PartItem* part);

    void updateRobotPart(PartItem* part);

private slots:

    void tabChanged(int index);

private:

    void addRobotInList(const std::string& robotName);

    void addRobotInWidget(const std::string& robotName);

    void addRobotPartInList(const std::string& robotName, const std::string &partName, PartItem* part);

    void addRobotPartInWidget(const std::string& robotName, const std::string &partName, int partIndex, PartItem* part);


    QTabWidget* m_tabs;
    ModesListWidget* m_list;
    QVBoxLayout* m_widgetLayout;
    std::unordered_map<std::string, QTreeWidgetItem*> m_robotMapList;
    std::unordered_map<std::string, CustomGroupBox*> m_robotMapWidget;
    QIcon m_okIcon;
    QIcon m_warningIcon;
};

#endif // MODESTREEMANAGER_H

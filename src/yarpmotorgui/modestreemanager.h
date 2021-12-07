/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODESTREEMANAGER_H
#define MODESTREEMANAGER_H

#include <QObject>
#include <QTreeWidget>
#include <QIcon>
#include <string>
#include <unordered_map>
#include "partitem.h"

class ModesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ModesTreeWidget(QWidget * parent = 0);
};

class ModesTreeManager : public QObject
{
    Q_OBJECT

public:
    ModesTreeManager(ModesTreeWidget* treeToManage, QObject* parent = nullptr);

    void addRobot(const std::string& robotName);

    void addRobotPart(const std::string& robotName, PartItem* part);

    void updateRobotPart(PartItem* part);

private:

    ModesTreeWidget* m_tree;
    std::unordered_map<std::string, QTreeWidgetItem*> m_robotMap;
    QIcon m_okIcon;
    QIcon m_warningIcon;
};

#endif // MODESTREEMANAGER_H

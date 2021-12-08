/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODESTREEMANAGER_H
#define MODESTREEMANAGER_H

#include <QObject>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QTabWidget>
#include <string>
#include <unordered_map>
#include "partitem.h"

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

    void addRobotPart(const std::string& robotName, const std::string &partName, PartItem* part);

    void updateRobotPart(PartItem* part);

private slots:

    void tabChanged(int index);

private:

    QTabWidget* m_tabs;
    ModesListWidget* m_list;
    std::unordered_map<std::string, QTreeWidgetItem*> m_robotMap;
    QIcon m_okIcon;
    QIcon m_warningIcon;
};

#endif // MODESTREEMANAGER_H

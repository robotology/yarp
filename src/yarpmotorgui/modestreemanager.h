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
#include <QTimer>
#include <string>
#include <unordered_map>
#include "partitem.h"
#include "robotWidgetTree.h"

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

    void updateRobotPart(int index);

signals:

    void sig_jointClicked(int partIndex, int jointIndex);

    void sig_partDoubleClicked(int index);

private slots:

    void onTabChanged(int index);

    void onJointClicked(int partIndex, int jointIndex);

    void onJointHomeFromTree(int partIndex, int jointIndex);

    void onJointRunFromTree(int partIndex, int jointIndex);

    void onJointIdleFromTree(int partIndex, int jointIndex);

    void onJointPIDFromTree(int partIndex, int jointIndex);

    void onPartDoubleClicked(int partIndex);

private:

    void addRobotInList(const std::string& robotName);

    void addRobotInWidget(const std::string& robotName);

    void addRobotPartInList(const std::string& robotName, const std::string &partName, int partIndex, PartItem* part);

    void addRobotPartInWidget(const std::string& robotName, const std::string &partName, int partIndex, PartItem* part);

    void updateRobotPartInList(int index, const QVector<JointItem::JointState>& modes);

    void updateRobotPartInWidget(int index, const QVector<JointItem::JointState>& modes);

    struct PartPointers
    {
        PartItem* partItem;
        QTreeWidgetItem* listWidget;
        RobotWidgetTree* robotWidget;
    };

    QTabWidget* m_tabs;
    ModesListWidget* m_list;
    QVBoxLayout* m_widgetLayout;
    std::unordered_map<std::string, QTreeWidgetItem*> m_robotMapList;
    std::unordered_map<std::string, RobotWidgetTree*> m_robotMapWidget;
    std::unordered_map<int, PartPointers> m_indexToPartMap;
    QIcon m_okIcon;
    QIcon m_warningIcon;
};

#endif // MODESTREEMANAGER_H

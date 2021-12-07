/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "modestreemanager.h"
#define TREEMODE_OK     1
#define TREEMODE_WARN   2

ModesTreeWidget::ModesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}

ModesTreeManager::ModesTreeManager(ModesTreeWidget *treeToManage, QObject *parent)
    : QObject(parent),
      m_tree(treeToManage),
      m_okIcon(":/apply.svg"),
      m_warningIcon(":/warning.svg")
{

}

void ModesTreeManager::addRobot(const std::string &robotName)
{
    auto* robot_top = new QTreeWidgetItem();
    robot_top->setText(0, robotName.c_str());
    m_tree->addTopLevelItem(robot_top);
    robot_top->setExpanded(true);
    m_robotMap[robotName] = robot_top;
}

void ModesTreeManager::addRobotPart(const std::string &robotName, PartItem* part)
{
    auto* partItem = new QTreeWidgetItem();
    partItem->setText(0, part->getPartName());
    QTreeWidgetItem *tp = m_robotMap[robotName];
    tp->addChild(partItem);
    partItem->setExpanded(false);
    part->setTreeWidgetModeNode(partItem);

    for (int i = 0; i < part->getNumberOfJoints(); ++i)
    {
        auto* jointNode = new QTreeWidgetItem(partItem);
        jointNode->setText(0,QString("Joint %1 (%2)").arg(i).arg(part->getJointName(i)));
        jointNode->setText(1,JointItem::GetModeString(JointItem::Idle));
        QColor c = JointItem::GetModeColor(JointItem::Idle);
        jointNode->setBackground(0,c);
        jointNode->setBackground(1,c);
        jointNode->setForeground(0,QColor(Qt::black));
        jointNode->setForeground(1,QColor(Qt::black));
    }
}

void ModesTreeManager::updateRobotPart(PartItem *part)
{
    QTreeWidgetItem *parentNode = part->getTreeWidgetModeNode();

    QList <JointItem::JointState> modes = part->getPartMode();

    if (modes.size() != parentNode->childCount())
    {
        return;
    }

    bool foundFaultPart = false;
    for(int i = 0; i < parentNode->childCount(); i++){
        QTreeWidgetItem *item = parentNode->child(i);
        QString mode;
        QColor c = JointItem::GetModeColor(modes.at(i));
        mode = JointItem::GetModeString(modes.at(i));

        if(modes.at(i) == JointItem::HwFault){
            foundFaultPart = true;
            if(item->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
                item->setIcon(0,m_warningIcon);
                item->setData(0,Qt::UserRole,TREEMODE_WARN);
            }
        }else{
            item->setIcon(0,QIcon());
            item->setData(0,Qt::UserRole,TREEMODE_OK);
        }

        if(parentNode->isExpanded()){
            if(item->text(1) != mode){
                item->setText(1,mode);
            }
            if(item->background(0) != c){
                item->setBackground(0, c);
                item->setBackground(1, c);
                item->setForeground(0, Qt::black);
                item->setForeground(1, Qt::black);
            }
        }
    }

    if(!foundFaultPart){
        if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_OK){
            parentNode->setBackground(0,Qt::white);
            parentNode->setIcon(0, m_okIcon);
            parentNode->setData(0,Qt::UserRole,TREEMODE_OK);
        }
    }else{
        if(parentNode->data(0,Qt::UserRole).toInt() != TREEMODE_WARN){
            parentNode->setBackground(0,hwFaultColor);
            parentNode->setIcon(0, m_warningIcon);
            parentNode->setData(0,Qt::UserRole,TREEMODE_WARN);
        }

    }
}

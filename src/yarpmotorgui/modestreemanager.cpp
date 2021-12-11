/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "modestreemanager.h"

#include <QHeaderView>
#include <QScrollArea>
#include <QGroupBox>
#define TREEMODE_OK     1
#define TREEMODE_WARN   2

ModesListWidget::ModesListWidget(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderLabels({"Parts", "Mode"});
    setMaximumWidth(300);
    setFrameShape(Shape::NoFrame);
    setAnimated(true);
    header()->setDefaultSectionSize(150);
}

ModesTreeManager::ModesTreeManager(QHBoxLayout *layout, QWidget *parent)
    : QObject(parent),
      m_okIcon(":/apply.svg"),
      m_warningIcon(":/warning.svg")
{
    m_tabs = new QTabWidget();
    m_list = new ModesListWidget();
    m_tabs->addTab(m_list, "List");
    m_tabs->setMaximumWidth(m_list->maximumWidth());

    auto* widgetContainer = new QWidget();
    m_widgetLayout = new QVBoxLayout();
    m_widgetLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    widgetContainer->setLayout(m_widgetLayout);
    widgetContainer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setWidget(widgetContainer);

    m_tabs->addTab(scroll, "Widgets");

    layout->addWidget(m_tabs);

    connect(m_tabs,SIGNAL(currentChanged(int)),this, SLOT(onTabChanged(int)));
}

void ModesTreeManager::addRobot(const std::string &robotName)
{
    addRobotInList(robotName);
    addRobotInWidget(robotName);
}

void ModesTreeManager::addRobotPart(const std::string &robotName, const std::string& partName, int partIndex, PartItem* part)
{
    m_indexToPartMap[partIndex].partItem = part;
    addRobotPartInList(robotName, partName, partIndex, part);
    addRobotPartInWidget(robotName, partName, partIndex, part);
}

void ModesTreeManager::updateRobotPart(int index)
{
    const QVector<JointItem::JointState>& modes = m_indexToPartMap[index].partItem->getPartModes();
    updateRobotPartInList(index, modes);
    updateRobotPartInWidget(index, modes);
}

void ModesTreeManager::onTabChanged(int index)
{
    m_tabs->setMaximumWidth(m_tabs->widget(index)->maximumWidth());
}

void ModesTreeManager::onJointClicked(int partIndex, int jointIndex)
{
    emit sig_jointClicked(partIndex, jointIndex);
}

void ModesTreeManager::onJointHomeFromTree(int partIndex, int jointIndex)
{
    m_indexToPartMap[partIndex].partItem->getJointWidget(jointIndex)->home();
}

void ModesTreeManager::onJointRunFromTree(int partIndex, int jointIndex)
{
    m_indexToPartMap[partIndex].partItem->getJointWidget(jointIndex)->run();
}

void ModesTreeManager::onJointIdleFromTree(int partIndex, int jointIndex)
{
    m_indexToPartMap[partIndex].partItem->getJointWidget(jointIndex)->idle();
}

void ModesTreeManager::onJointPIDFromTree(int partIndex, int jointIndex)
{
    m_indexToPartMap[partIndex].partItem->getJointWidget(jointIndex)->showPID();
}

void ModesTreeManager::addRobotInList(const std::string &robotName)
{
    auto* robot_top = new QTreeWidgetItem();
    robot_top->setText(0, robotName.c_str());
    m_list->addTopLevelItem(robot_top);
    robot_top->setExpanded(true);
    m_robotMapList[robotName] = robot_top;
}

void ModesTreeManager::addRobotInWidget(const std::string &robotName)
{
    CustomGroupBox* newGroup = new CustomGroupBox;
    newGroup->setTitle(robotName.c_str());
    m_widgetLayout->addWidget(newGroup);

    newGroup->enableCollapseAllContextMenu(true);

    m_robotMapWidget[robotName] = newGroup;
}

void ModesTreeManager::addRobotPartInList(const std::string &robotName, const std::string &partName, int partIndex, PartItem *part)
{
    auto* partItem = new QTreeWidgetItem();
    partItem->setText(0, partName.c_str());
    QTreeWidgetItem *tp = m_robotMapList[robotName];
    tp->addChild(partItem);
    partItem->setExpanded(false);
    m_indexToPartMap[partIndex].listWidget = partItem;

    for (int i = 0; i < part->getNumberOfJoints(); ++i)
    {
        auto* jointNode = new QTreeWidgetItem(partItem);
        jointNode->setText(0,QString("Joint %1 (%2)").arg(i).arg(part->getJointName(i)));
        jointNode->setText(1,JointItem::GetModeString(JointItem::Idle));
        QColor c = JointItem::GetModeColor(JointItem::Idle);
        jointNode->setBackground(0,c);
        jointNode->setBackground(1,c);
        jointNode->setForeground(0,Qt::black);
        jointNode->setForeground(1,Qt::black);
    }
}

void ModesTreeManager::addRobotPartInWidget(const std::string &robotName, const std::string &partName, int partIndex, PartItem *part)
{
    PartItemTree* partWidget = new PartItemTree(partIndex);

    for (int i = 0; i < part->getNumberOfJoints(); ++i)
    {
        auto* jointWidget = partWidget->addJoint();
        jointWidget->setJointName(QString("%1 - %2").arg(i).arg(part->getJointName(i)));
    }

    CustomGroupBox* newPart = new CustomGroupBox;
    newPart->setTitle(partName.c_str());
    newPart->addWidget(partWidget);

    m_robotMapWidget[robotName]->addWidget(newPart);
    m_indexToPartMap[partIndex].partWidget = partWidget;
    m_indexToPartMap[partIndex].partWidgetParent = newPart;

    connect(partWidget, SIGNAL(sig_jointClicked(int,int)), this, SLOT(onJointClicked(int,int)));
    connect(partWidget, SIGNAL(sig_homeClicked(int,int)), this, SLOT(onJointHomeFromTree(int,int)));
    connect(partWidget, SIGNAL(sig_runClicked(int,int)), this, SLOT(onJointRunFromTree(int,int)));
    connect(partWidget, SIGNAL(sig_idleClicked(int,int)), this, SLOT(onJointIdleFromTree(int,int)));
    connect(partWidget, SIGNAL(sig_PIDClicked(int,int)), this, SLOT(onJointPIDFromTree(int,int)));
}

void ModesTreeManager::updateRobotPartInList(int index, const QVector<JointItem::JointState> &modes)
{
    QTreeWidgetItem *parentNode = m_indexToPartMap[index].listWidget;

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

void ModesTreeManager::updateRobotPartInWidget(int index, const QVector<JointItem::JointState> &modes)
{
    PartPointers& partPointers = m_indexToPartMap[index];
    PartItemTree* partWidget = partPointers.partWidget;
    CustomGroupBox* partWidgetParent = partPointers.partWidgetParent;


    bool foundFaultPart = false;
    for (int i = 0; i < partWidget->numberOfJoints(); i++){
        auto* jointNode = partWidget->getJoint(i);
        jointNode->setJointMode(modes.at(i));

        if (modes.at(i) == JointItem::HwFault){
            foundFaultPart = true;
        }
    }

    if (!foundFaultPart){
        partWidgetParent->removeTitleBackground();
        partWidgetParent->setTitleIcon(m_okIcon);
    } else {
        partWidgetParent->setTitleBackgroundColor(hwFaultColor);
        partWidgetParent->setTitleIcon(m_warningIcon);
    }
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "robotWidgetTree.h"
#include "yarpmotorgui.h"


void RobotWidgetTree::setIcons(const QIcon &okIcon, const QIcon &warningIcon)
{
    m_okIcon = okIcon;
    m_warningIcon = warningIcon;
}

void RobotWidgetTree::addPart(const std::string &partName, int partIndex, PartItemTree *partTreeWidget)
{
    CustomGroupBox* newPart = new CustomGroupBox;
    newPart->setTitle(partName.c_str());
    newPart->addWidget(partTreeWidget);

    this->addWidget(newPart);
    m_indexToPartMap[partIndex].partWidget = partTreeWidget;
    m_indexToPartMap[partIndex].partWidgetParent = newPart;
}

void RobotWidgetTree::updateRobotPart(int index, const QVector<JointItem::JointState> &modes)
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

void RobotWidgetTree::resizeEvent(QResizeEvent *event)
{
    QSize maxSize;
    for (auto& part : m_indexToPartMap)
    {
        maxSize = maxSize.expandedTo(part.second.partWidget->getMaxElementSize());
    }

    for (auto& part : m_indexToPartMap)
    {
        part.second.partWidget->setDesiredElementSize(maxSize);
    }

    QWidget::resizeEvent(event);

}

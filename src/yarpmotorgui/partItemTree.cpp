/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "partItemTree.h"
#include <iostream>

PartItemTree::PartItemTree(QWidget *parent) : QWidget(parent)
{
    m_layout = new FlowLayout(1, 1, 1);
    setLayout(m_layout);
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

int PartItemTree::numberOfJoints() const
{
    return m_layout->count();
}

JointItemTree *PartItemTree::addJoint()
{
    JointItemTree* added = new JointItemTree(this);
    m_layout->addWidget(added);
    return added;
}

JointItemTree *PartItemTree::getJoint(int index)
{
    if (index < 0 || index >= numberOfJoints())
    {
        return nullptr;
    }

    return (JointItemTree*)m_layout->itemAt(index)->widget();
}

QSize PartItemTree::sizeHint() const
{
    QSize initialHint = QWidget::sizeHint();
    return QSize(initialHint.width(), m_layout->heightForWidth(width()));
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "partItemTree.h"
#include <iostream>

PartItemTree::PartItemTree(QWidget *parent) : QWidget(parent)
{
    m_layout = new FlowLayout(5, 5, 5);
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

void PartItemTree::uniformLayout()
{
    if (m_layout->count() == 0)
    {
        return;
    }

    m_layout->activate(); //To make sure that the widgets have already the appropriate size

    int maxWidth = m_layout->itemAt(0)->widget()->width();
    int maxHeight = m_layout->itemAt(0)->widget()->height();

    for (int i = 1; i < m_layout->count(); ++i)
    {
        int width = m_layout->itemAt(i)->widget()->width();
        int height = m_layout->itemAt(i)->widget()->height();

        if (width > maxWidth)
        {
            maxWidth = width;
        }

        if (height > maxHeight)
        {
            maxHeight = height;
        }
    }

    for (int i = 0; i < m_layout->count(); ++i)
    {
        JointItemTree* item = (JointItemTree*)(m_layout->itemAt(i)->widget());
        item->setDesiredSize(maxWidth, maxHeight);
    }

    m_layout->activate(); //Update the layout
}

void PartItemTree::resizeEvent(QResizeEvent *event)
{
    uniformLayout();
}

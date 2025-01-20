/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "partItemTree.h"

PartItemTree::PartItemTree(int index, QWidget *parent) : QWidget(parent), m_index(index)
{
    const int flowLayoutSpacing = 5;
    m_layout = new FlowLayout(flowLayoutSpacing, flowLayoutSpacing, flowLayoutSpacing);
    setLayout(m_layout);
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

int PartItemTree::numberOfJoints() const
{
    return m_layout->count();
}

JointItemTree *PartItemTree::addJoint()
{
    JointItemTree* added = new JointItemTree(m_layout->count(), this);
    connect(added, SIGNAL(sig_jointClicked(int)), this, SLOT(onJointClicked(int)));
    connect(added, SIGNAL(sig_homeClicked(int)), this, SLOT(onHomeClicked(int)));
    connect(added, SIGNAL(sig_runClicked(int)), this, SLOT(onRunClicked(int)));
    connect(added, SIGNAL(sig_idleClicked(int)), this, SLOT(onIdleClicked(int)));
    connect(added, SIGNAL(sig_PIDClicked(int)), this, SLOT(onPIDClicked(int)));

    m_layout->addWidget(added);
    return added;
}

JointItemTree *PartItemTree::getJoint(int index)
{
    if (index < 0 || index >= numberOfJoints())
    {
        return nullptr;
    }

    return static_cast<JointItemTree*>(m_layout->itemAt(index)->widget());
}

void PartItemTree::uniformLayout()
{
    if (m_layout->count() == 0)
    {
        return;
    }

    QSize desiredSize = m_desiredElementSize.expandedTo(getMaxElementSize());

    for (int i = 0; i < m_layout->count(); ++i)
    {
        JointItemTree* item = getJoint(i);
        item->setDesiredSize(desiredSize.width(), desiredSize.height());
    }

    m_layout->activate(); //Update the layout
}

void PartItemTree::resizeEvent(QResizeEvent *event)
{
    uniformLayout();
    QWidget::resizeEvent(event);
}

QSize PartItemTree::getMaxElementSize()
{
    QSize output;
    m_layout->activate(); //To make sure that the widgets have already the appropriate size

    for (int i = 0; i < m_layout->count(); ++i)
    {
        output = output.expandedTo(m_layout->itemAt(i)->widget()->size());
    }
    return output;
}

void PartItemTree::setDesiredElementSize(const QSize& maxSize)
{
    m_desiredElementSize = maxSize;
}

void PartItemTree::onJointClicked(int index)
{
    emit sig_jointClicked(m_index, index);
}

void PartItemTree::onHomeClicked(int index)
{
    emit sig_homeClicked(m_index, index);
}

void PartItemTree::onRunClicked(int index)
{
    emit sig_runClicked(m_index, index);
}

void PartItemTree::onIdleClicked(int index)
{
    emit sig_idleClicked(m_index, index);
}

void PartItemTree::onPIDClicked(int index)
{
    emit sig_PIDClicked(m_index, index);
}

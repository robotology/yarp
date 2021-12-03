/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "jointItemTree.h"
#include "ui_jointItemTree.h"

JointItemTree::JointItemTree(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::jointItemTree)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
}

JointItemTree::~JointItemTree()
{
    delete m_ui;
}

QLabel *JointItemTree::jointLabel()
{
    return m_ui->jointName;
}

QLabel *JointItemTree::modeLabel()
{
    return m_ui->jointMode;
}

void JointItemTree::setColor(const QColor &color, const QColor &background)
{
    setAutoFillBackground(true);
    setStyleSheet(QString("color: rbg(%1, %2, %3); background-color:  rbg(%4, %5, %6);").arg(color.red())
                  .arg(color.green())
                  .arg(color.blue())
                  .arg(background.red())
                  .arg(background.green())
                  .arg(background.blue()));
}

QSize JointItemTree::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    if (m_desiredWidth > 0)
    {
        size.setWidth(m_desiredWidth);
    }
    if (m_desiredHeight > 0)
    {
        size.setHeight(m_desiredHeight);
    }

    return size;
}

void JointItemTree::setDesiredSize(int w, int h)
{
    m_desiredWidth = w;
    m_desiredHeight = h;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "jointItemTree.h"
#include "ui_jointItemTree.h"

JointItemTree::JointItemTree(int index, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::jointItemTree),
    m_index(index)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onShowContextMenu(QPoint)));

    m_rightClickMenuTitle = new QAction(this);
    m_rightClickMenuTitle->setEnabled(false);
    m_rightClickMenu.addAction(m_rightClickMenuTitle);
    m_rightClickMenu.addSeparator();

    QAction* homeAction = new QAction("Home joint", this);
    connect(homeAction, SIGNAL(triggered()), this, SLOT(onHomeClicked()));
    m_rightClickMenu.addAction(homeAction);

    QAction* idleAction = new QAction("Idle joint", this);
    connect(idleAction, SIGNAL(triggered()), this, SLOT(onIdleClicked()));
    m_rightClickMenu.addAction(idleAction);

    QAction* runAction = new QAction("Run joint", this);
    connect(runAction, SIGNAL(triggered()), this, SLOT(onRunClicked()));
    m_rightClickMenu.addAction(runAction);

    QAction* pidAction = new QAction("Show joint PID", this);
    connect(pidAction, SIGNAL(triggered()), this, SLOT(onPIDClicked()));
    m_rightClickMenu.addAction(pidAction);
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
    QString stileSheet = QString("color: rgb(%1, %2, %3); background-color: rgb(%4, %5, %6)")
            .arg(color.red()).arg(color.green()).arg(color.blue())
            .arg(background.red()).arg(background.green()).arg(background.blue());
    setStyleSheet(stileSheet);
}

void JointItemTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sig_jointClicked(m_index);
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

void JointItemTree::onShowContextMenu(QPoint pos)
{
    m_rightClickMenuTitle->setText(m_ui->jointName->text() + " menu");
    m_rightClickMenu.exec(mapToGlobal(pos));
}

void JointItemTree::onHomeClicked()
{
    emit sig_homeClicked(m_index);
}

void JointItemTree::onRunClicked()
{
    emit sig_runClicked(m_index);
}

void JointItemTree::onIdleClicked()
{
    emit sig_idleClicked(m_index);
}

void JointItemTree::onPIDClicked()
{
    emit sig_PIDClicked(m_index);
}

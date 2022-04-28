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

    m_homeAction = new QAction("Home joint", this);
    connect(m_homeAction, SIGNAL(triggered()), this, SLOT(onHomeClicked()));
    m_rightClickMenu.addAction(m_homeAction);

    m_idleAction = new QAction("Idle joint", this);
    connect(m_idleAction, SIGNAL(triggered()), this, SLOT(onIdleClicked()));
    m_rightClickMenu.addAction(m_idleAction);

    m_runAction = new QAction("Run joint", this);
    connect(m_runAction, SIGNAL(triggered()), this, SLOT(onRunClicked()));
    m_rightClickMenu.addAction(m_runAction);

    m_pidAction = new QAction("Show joint PID", this);
    connect(m_pidAction, SIGNAL(triggered()), this, SLOT(onPIDClicked()));
    m_rightClickMenu.addAction(m_pidAction);
}

JointItemTree::~JointItemTree()
{
    delete m_ui;
}

void JointItemTree::setJointName(const QString &name)
{
    m_ui->jointName->setText(name);
}

QString JointItemTree::jointName() const
{
    return m_ui->jointName->text();
}

void JointItemTree::setJointMode(const JointItem::JointState &mode)
{

    if (!m_modeSet || mode != m_mode)
    {
        const int text_r = 35;
        const int text_g = 38;
        const int text_b = 41;
        setColor(QColor(text_r, text_g, text_b), JointItem::GetModeColor(mode));

        switch (mode) {
        case JointItem::Position:
        case JointItem::PositionDirect:
        {
            m_homeAction->setEnabled(true);
            m_runAction->setEnabled(true);
            m_idleAction->setEnabled(true);
            m_pidAction->setEnabled(true);
            break;
        }
        case JointItem::Idle:
        case JointItem::Mixed:
        case JointItem::Velocity:
        case JointItem::Torque:
        case JointItem::Pwm:
        case JointItem::Current:
        {
            m_homeAction->setEnabled(false);
            m_runAction->setEnabled(true);
            m_idleAction->setEnabled(true);
            m_pidAction->setEnabled(true);
            break;
        }

        case JointItem::HwFault:{
            m_homeAction->setEnabled(false);
            m_runAction->setEnabled(false);
            m_idleAction->setEnabled(true);
            m_pidAction->setEnabled(true);
            break;
        }
        case JointItem::Disconnected:
        case JointItem::Calibrating:
        case JointItem::NotConfigured:
        case JointItem::Configured:
        default:
        {
            m_homeAction->setEnabled(false);
            m_runAction->setEnabled(false);
            m_idleAction->setEnabled(false);
            m_pidAction->setEnabled(false);
            break;
        }
        }

        m_ui->jointMode->setText(JointItem::GetModeString(mode));
        m_modeSet = true;
        m_mode = mode;
    }
}

JointItem::JointState JointItemTree::jointMode() const
{
    return m_mode;
}

void JointItemTree::setColor(const QColor &color, const QColor &background)
{
    setAutoFillBackground(true);
    QString stileSheet = QString("color: rgb(%1, %2, %3); background-color: rgb(%4, %5, %6)")
            .arg(color.red()).arg(color.green()).arg(color.blue())
            .arg(background.red()).arg(background.green()).arg(background.blue());
    setStyleSheet(stileSheet);
}

void JointItemTree::mouseDoubleClickEvent(QMouseEvent *)
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

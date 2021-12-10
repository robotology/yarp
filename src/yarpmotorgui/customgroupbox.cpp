/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "customgroupbox.h"
#include "ui_customgroupbox.h"

CustomGroupBoxLabel::CustomGroupBoxLabel(QWidget *parent)
    : QPushButton(parent)
{

}

void CustomGroupBoxLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sig_titleDoubleClick();
}

CustomGroupBox::CustomGroupBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomGroupBox),
    m_visible(true)
{
    ui->setupUi(this);
    m_layout = new QVBoxLayout;
    m_layout->setMargin(0);
    ui->frame->setLayout(m_layout);
    connect(ui->arrowButton, SIGNAL(triggered(QAction*)), this, SLOT(onArrowPressed(QAction*)));
    connect(ui->header, SIGNAL(sig_titleDoubleClick()), this, SLOT(onTitleDoubleClick()));

}

CustomGroupBox::~CustomGroupBox()
{
    delete ui;
}

void CustomGroupBox::setTitle(const QString &string)
{
    ui->header->setText(string);
}

void CustomGroupBox::addWidget(QWidget *widget)
{
    m_layout->addWidget(widget);
}

void CustomGroupBox::toggle(bool visible)
{
    ui->contentWidget->setVisible(visible);
    m_visible = visible;
}

void CustomGroupBox::onArrowPressed(QAction *)
{
    toggle(!m_visible);
}

void CustomGroupBox::onTitleDoubleClick()
{
    toggle(!m_visible);
}



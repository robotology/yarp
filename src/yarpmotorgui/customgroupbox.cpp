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

    toggle(true);

    connect(ui->arrowButton, SIGNAL(clicked(bool)), this, SLOT(onArrowPressed(bool)));
    connect(ui->header, SIGNAL(sig_titleDoubleClick()), this, SLOT(onTitleDoubleClick()));

    ui->header->setAttribute(Qt::WA_StyledBackground, true);

    QAction* expandAll = m_contextMenu.addAction("Expand all");
    connect(expandAll, SIGNAL(triggered()), this, SLOT(onExpandAll()));

    QAction* collapseAll = m_contextMenu.addAction("Collapse all");
    connect(collapseAll, SIGNAL(triggered()), this, SLOT(onCollapseAll()));

}

CustomGroupBox::~CustomGroupBox()
{
    delete ui;
}

void CustomGroupBox::setTitle(const QString &string)
{
    ui->header->setText(string);
}

void CustomGroupBox::setTitleBackgroundColor(const QColor &backgroundColor)
{

    QString stileSheet = QString("text-align: left; color: rgb(35, 38, 41); background-color: rgb(%1, %2, %3)")
            .arg(backgroundColor.red()).arg(backgroundColor.green()).arg(backgroundColor.blue());
    ui->header->setStyleSheet(stileSheet);
}

void CustomGroupBox::removeTitleBackground()
{
    QString stileSheet = QString("text-align: left; color: rgb(35, 38, 41);");
    ui->header->setStyleSheet(stileSheet);
}

void CustomGroupBox::setTitleIcon(const QIcon &icon)
{
    ui->header->setIcon(icon);
}

void CustomGroupBox::addWidget(QWidget *widget)
{
    m_layout->addWidget(widget);
}

void CustomGroupBox::toggle(bool visible)
{
    ui->contentWidget->setVisible(visible);
    m_visible = visible;
    ui->arrowButton->setArrowType(m_visible ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
}

void CustomGroupBox::enableCollapseAllContextMenu(bool enable)
{
    if (enable)
    {
        ui->header->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(ui->header, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(onShowContextMenu(QPoint)));
    }
    else
    {
        ui->header->setContextMenuPolicy(Qt::NoContextMenu);

        disconnect(ui->header, SIGNAL(customContextMenuRequested(QPoint)),
                   this, SLOT(onShowContextMenu(QPoint)));
    }
}

void CustomGroupBox::onArrowPressed(bool)
{
    toggle(!m_visible);
}

void CustomGroupBox::onTitleDoubleClick()
{
    toggle(!m_visible);
}

void CustomGroupBox::onExpandAll()
{
    for (int i = 0; i < m_layout->count(); ++i)
    {
        QWidget* child = m_layout->itemAt(i)->widget();
        if (child->objectName() == "CustomGroupBox")
        {
            CustomGroupBox* casted = (CustomGroupBox*)child;
            casted->toggle(true);
        }
    }
}

void CustomGroupBox::onCollapseAll()
{
    for (int i = 0; i < m_layout->count(); ++i)
    {
        QWidget* child = m_layout->itemAt(i)->widget();
        if (child->objectName() == "CustomGroupBox")
        {
            CustomGroupBox* casted = (CustomGroupBox*)child;
            casted->toggle(false);
        }
    }
}

void CustomGroupBox::onShowContextMenu(QPoint pos)
{
    m_contextMenu.exec(mapToGlobal(pos));
}

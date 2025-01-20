/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "genericviewwidget.h"

GenericViewWidget::GenericViewWidget(QWidget *parent) : QWidget(parent),
    type(yarp::manager::NodeType::NODE_OTHER),
    m_modified(false)
{}

/*! \brief return the type of the list*/
yarp::manager::NodeType GenericViewWidget::getType()
{
    return type;
}

bool GenericViewWidget::isModified()
{
    return m_modified;
}

void GenericViewWidget::setModified(bool mod)
{
    m_modified=mod;
}

void GenericViewWidget::onModified(bool mod)
{
    this->m_modified = mod;
    emit modified(m_modified);

}

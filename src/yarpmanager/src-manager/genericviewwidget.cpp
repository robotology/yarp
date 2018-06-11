/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
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

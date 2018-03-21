/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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

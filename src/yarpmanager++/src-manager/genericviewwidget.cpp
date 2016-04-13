/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "genericviewwidget.h"

GenericViewWidget::GenericViewWidget(QWidget *parent) :
    QWidget(parent)
{
    m_modified = false;
}

/*! \brief return the type of the list*/
yarp::manager::NodeType GenericViewWidget::getType()
{
    return type;
}

bool GenericViewWidget::isModified()
{
    return m_modified;
}

void GenericViewWidget::onModified(bool mod)
{
    this->m_modified = mod;
    modified(m_modified);

}

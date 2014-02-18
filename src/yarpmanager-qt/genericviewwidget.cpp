/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
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
}

/*! \brief return the type of the list*/
yarp::manager::NodeType GenericViewWidget::getType()
{
    return type;
}

/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#include "qtyarpview_plugin.h"
#include "qtyarpview.h"

#include <qqml.h>


void QtYARPViewPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.view
    qmlRegisterType<QtYARPView>(uri, 1, 0, "QtYARPViewPlugin");
}



/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "qtyarpview_plugin.h"
#include "qtyarpview.h"

#include <qqml.h>


void QtYARPViewPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.view
    qmlRegisterType<QtYARPView>(uri, 1, 0, "QtYARPViewPlugin");
}



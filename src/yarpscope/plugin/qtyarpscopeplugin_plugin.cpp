/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "qtyarpscopeplugin_plugin.h"
#include "qtyarpscope.h"
#include <qqml.h>

void QtYARPScopePluginPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.scope
    qmlRegisterType<QtYARPScope>(uri, 1, 0, "QtYARPScopePlugin");
}



/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "qtyarpview_plugin.h"
#include "qtyarpview.h"

#include <qqml.h>


void QtYARPViewPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.view
    qmlRegisterType<QtYARPView>(uri, 1, 0, "QtYARPViewPlugin");
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "qtyarpscopeplugin_plugin.h"
#include "qtyarpscope.h"
#include <qqml.h>

void QtYARPScopePluginPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.scope
    qmlRegisterType<QtYARPScope>(uri, 1, 0, "QtYARPScopePlugin");
}

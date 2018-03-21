/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef QTYARPSCOPEPLUGIN_PLUGIN_H
#define QTYARPSCOPEPLUGIN_PLUGIN_H

#include <QQmlExtensionPlugin>
#include <yarp/conf/compiler.h>

class QtYARPScopePluginPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif // QTYARPSCOPEPLUGIN_PLUGIN_H


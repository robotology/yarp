/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
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

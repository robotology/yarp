/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef QTYARPVIEW_PLUGIN_H
#define QTYARPVIEW_PLUGIN_H

#include <QQmlExtensionPlugin>
#include <yarp/conf/compiler.h>

class QtYARPViewPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif // QTYARPVIEW_PLUGIN_H

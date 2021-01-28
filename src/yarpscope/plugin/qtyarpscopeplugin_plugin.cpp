/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qtyarpscopeplugin_plugin.h"
#include "qtyarpscope.h"
#include <qqml.h>

void QtYARPScopePluginPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.scope
    qmlRegisterType<QtYARPScope>(uri, 1, 0, "QtYARPScopePlugin");
}

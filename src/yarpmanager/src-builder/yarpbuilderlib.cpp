/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "yarpbuilderlib.h"


YarpBuilderLib::YarpBuilderLib() = default;

BuilderWindow *YarpBuilderLib::getBuilder(Application *app, Manager *lazyManager, SafeManager *manager, bool editingMode)
{
    return new BuilderWindow(app,lazyManager,manager,editingMode);
}

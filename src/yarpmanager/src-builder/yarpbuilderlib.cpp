/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#include "yarpbuilderlib.h"


YarpBuilderLib::YarpBuilderLib()
{
}

BuilderWindow *YarpBuilderLib::getBuilder(Application *app, Manager *lazyManager, SafeManager *manager, bool editingMode)
{
    return new BuilderWindow(app,lazyManager,manager,editingMode);
}


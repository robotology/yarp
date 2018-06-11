/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef YARPBUILDERLIB_H
#define YARPBUILDERLIB_H

#include "builderwindow.h"
#include "yarpbuilderlib_global.h"
#include <yarp/manager/manager.h>
#include "safe_manager.h"

using namespace yarp::manager;

class YARPBUILDERLIBSHARED_EXPORT YarpBuilderLib
{

public:
    YarpBuilderLib();
    static BuilderWindow *getBuilder(Application *app, Manager *lazyManager, SafeManager *manager,bool editingMode);

};

#endif // YARPBUILDERLIB_H

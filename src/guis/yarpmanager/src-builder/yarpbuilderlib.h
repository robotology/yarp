/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RFPlugin.h>
#include <yarp/os/Time.h>
#include <memory>

using namespace yarp::os;

int main()
{
    std::unique_ptr<RFPlugin> pluginBar(new RFPlugin);

    if (!pluginBar->open("myModuleBar"))
    {
        printf("failed to open myModuleBar\n");
        return 1;
    }

    std::unique_ptr<RFPlugin> pluginFoo(new RFPlugin);

    if (!pluginFoo->open("myModuleFoo"))
    {
        printf("failed to open myModuleFoo\n");
        return 1;
    }

    while (true)
    {
        Time::delay(0.3);
    }

    return 0;
}

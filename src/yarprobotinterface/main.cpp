/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Module.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/dev/Drivers.h>

int main(int argc, char* argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    yarp::os::ResourceFinder& rf(yarp::os::ResourceFinder::getResourceFinderSingleton());
    rf.setDefaultConfigFile("yarprobotinterface.ini");
    rf.configure(argc, argv);

    // Create and run our module
    yarprobotinterface::Module module;
    return module.runModule(rf);
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <module.h>
#include <yarp/os/Network.h>
int main(int argc, char** argv)
{
    yarp::os::Network yarp;
    moduleBar module;
    return module.runModule();
}

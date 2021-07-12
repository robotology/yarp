/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DriverLinkCreator.h>

yarp::dev::DriverLinkCreator::DriverLinkCreator(const std::string& name, PolyDriver& source) :
        name(name)
{
    holding.link(source);
}

yarp::dev::DriverLinkCreator::~DriverLinkCreator()
{
    holding.close();
}

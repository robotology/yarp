/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_PARSENAME_H
#define YARP_SERVERSQL_IMPL_PARSENAME_H

#include <string>


namespace yarp {
namespace serversql {
namespace impl {

class ParseName
{
private:
    std::string carrier;
    std::string networkChoice;
    std::string portName;
public:
    void apply(const std::string& str);

    std::string getPortName()
    {
        return portName;
    }

    std::string getCarrier()
    {
        return carrier;
    }

    std::string getNetworkChoice()
    {
        return networkChoice;
    }

    void resetCarrier()
    {
        carrier = "";
    }
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_PARSENAME_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

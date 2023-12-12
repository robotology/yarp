/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IDEVICEDRIVERPARAMETERS_H
#define YARP_DEV_IDEVICEDRIVERPARAMETERS_H

#include <yarp/dev/api.h>
#include <yarp/os/Searchable.h>
#include <string>

#include <vector>

namespace yarp::dev {
    class IDeviceDriverParams;
}

/**
 * \ingroup dev_class
 *
 * An interface for the management of the parameters of a DeviceDriver.
 * Methods should be reimplemented independently by each DeviceDriver.
 */
class YARP_dev_API yarp::dev::IDeviceDriverParams
{
public:
    virtual ~IDeviceDriverParams();

    /**
     * Parse the DeviceDriver parameters
     * @return true if the parsing is successful, false otherwise
     */
    virtual bool          parseParams(const yarp::os::Searchable& config) = 0;

    /**
     * Get the name of the DeviceDriver class.
     * @return A string containing the name of the DeviceDriver.
     */
    virtual std::string   getDeviceType() const = 0;

    /**
     * Get the documentation of the DeviceDriver's parameters.
     * @return A string containing the documentation.
     */
    virtual std::string   getDocumentationOfDeviceParams() const = 0;

    /**
     * Return a list of all params used by the device.
     * @return A vector containing the names of parameters used by the device.
     */
    virtual std::vector<std::string> getListOfParams() const = 0;
};

#endif //YARP_DEV_IDEVICEDRIVERPARAMETERS_H

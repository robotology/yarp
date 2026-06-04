/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_DEVICEDRIVER_H
#define YARP_DEV_DEVICEDRIVER_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>

#include <yarp/dev/api.h>

namespace yarp::dev {
class DeviceDriver;
class DeprecatedDeviceDriver;
}

/**
 * \ingroup dev_class
 *
 * Interface implemented by all device drivers.
 */
class YARP_dev_API yarp::dev::DeviceDriver
{
public:
    DeviceDriver();
    DeviceDriver(const DeviceDriver& other) = delete;
    DeviceDriver(DeviceDriver&& other) noexcept = delete;
    DeviceDriver& operator=(const DeviceDriver& other) = delete;
    DeviceDriver& operator=(DeviceDriver&& other) noexcept = delete;

    virtual ~DeviceDriver();

    /**
     * Open the DeviceDriver.
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config) { YARP_UNUSED(config); return true; }

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close() { return true; }


    /**
     * Return the id assigned to the PolyDriver.
     * If no name was assigned, returns the name of the device (if set) or an
     * empty string.
     * The value can be set by passing the `id` option when opening the device
     * or with the `setId()` method.
     *
     * @return the id for this device.
     */
    virtual std::string id() const;

    /**
     * Set the id for this device
     */
    virtual void setId(const std::string& id);

    /**
     * Get an interface to the device driver.

     * @param x A pointer of type T which will be set to point to this
     * object if that is possible.

     * @return true iff the desired interface is implemented by
     * the device driver.
     */
    template <class T>
    bool view(T *&x) {
        x = nullptr;

        // This is not super-portable; and it requires RTTI compiled
        // in.  For systems on which this is a problem, suggest:
        // either replace it with a regular cast (and warn user) or
        // implement own method for checking interface support.
        T *v = dynamic_cast<T *>(getImplementation());

        if (v != nullptr) {
            x = v;
            return true;
        }

        return false;
    }

    /**
     * Some drivers are bureaucrats, pointing at others.  Such drivers override
     * this method.
     *
     * @return "real" device driver
     *
     */
    virtual DeviceDriver *getImplementation() {
        return this;
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv = nullptr;
#endif
};

/**
 * \ingroup dev_class
 *
 * Interface implemented by deprecated device drivers.
 *
 * When a device is deprecated, replace yarp::dev::DeviceDriver with
 * yarp::dev::DeprecatedDeviceDriver in the list of interfaces implemented by
 * the class, in order to let YARP know that the class is deprecated.
 *
 * Deprecated device drivers cannot be opened as PolyDriver unless the
 * "allow-deprecated-devices" option is passed in the configuration.
 */
class YARP_dev_API yarp::dev::DeprecatedDeviceDriver : virtual public yarp::dev::DeviceDriver
{
};

#endif // YARP_DEV_DEVICEDRIVER_H

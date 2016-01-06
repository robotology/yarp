// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_POLYDRIVER
#define YARP2_POLYDRIVER

#include <yarp/dev/Drivers.h>

namespace yarp {
    namespace dev {
        class PolyDriver;
    }
}


/**
 * \ingroup dev_class
 *
 * A container for a device driver.
 */
class YARP_dev_API yarp::dev::PolyDriver : public DeviceDriver {
public:
    using DeviceDriver::open;

    /**
     * Constructor.
     */
    PolyDriver() {
        system_resource = 0 /*NULL*/;
        dd = 0 /*NULL*/;
    }

    /**
     * Construct and configure a device by its common name.
     * @param txt common name of the device
     */
    PolyDriver(const yarp::os::ConstString& txt) {
        system_resource = 0 /*NULL*/;
        dd = 0 /*NULL*/;
        open(txt);
    }

    /**
     * Create and configure a device, by name.  The config
     * object should have a property called "device" that
     * is set to the common name of the device.  All other
     * properties are passed on the the device's
     * DeviceDriver::open method.
     * @param config configuration options for the device
     */
    PolyDriver(yarp::os::Searchable& config) {
        system_resource = 0 /*NULL*/;
        dd = 0 /*NULL*/;
        open(config);
    }

    /**
     * Construct and configure a device by its common name.
     * @param txt common name of the device
     * @return true iff the device was created and configured successfully
     */
    bool open(const yarp::os::ConstString& txt);

    /**
     * Create and configure a device, by name.  The config
     * object should have a property called "device" that
     * is set to the common name of the device.  All other
     * properties are passed on the the device's
     * DeviceDriver::open method.
     * @param config configuration options for the device
     * @return true iff the device was created and configured successfully
     */
    bool open(yarp::os::Searchable& config);

    /**
     * Make this device be a link to an existing one.
     * The device will be reference counted, and destroyed
     * when the last relevant call to close() is made.
     * @param alt the device to link to
     * @return true iff link succeeded
     */
    bool link(PolyDriver& alt);


    /**
     * Gets the device this object manages.
     * The user is then responsible for managing it.
     * @return the device this object manages.
     */
    DeviceDriver *take();

    /**
     * Take on management of a device.
     * The PolyDriver may be responsible for destroying it.
     * @param dd the device to manage.
     * @param own true if PolyDriver should destroy device when done.
     * @return true on success.
     */
    bool give(DeviceDriver *dd, bool own);

    /**
     * Destructor.
     */
    virtual ~PolyDriver();

    virtual bool close() {
        return closeMain();
    }

    /**
     * Check if device is valid.
     * @return true iff the device was created and configured successfully
     */
    bool isValid() const {
        return dd != 0 /*NULL*/;
    }

    /**
     * After a call to PolyDriver::open, you can
     * get a list of all the options checked by the
     * device.
     * @return a list of options checked by the device
     */
    yarp::os::Bottle getOptions();

    /**
     * After a call to PolyDriver::open, you can
     * check if the device has documentation on a given option.
     * @param option the name of the option to check
     * @return the human-readable description of the option, if found
     */
    yarp::os::ConstString getComment(const char *option);

    /**
     * After a call to PolyDriver::open, you can
     * check if a given option has a particular default value.
     * @param option the name of the option to check
     * @return the default value of the option, if any.
     */
    yarp::os::Value getDefaultValue(const char *option);

    /**
     * After a call to PolyDriver::open, you can
     * check what value was found for a particular option, if any.
     * @param option the name of the option to check
     * @return the value found for the option, if any.
     */
    yarp::os::Value getValue(const char *option);

    DeviceDriver *getImplementation() {
      if(isValid())
        return dd->getImplementation();
      else
        return NULL;
    }

private:
    bool closeMain();

    DeviceDriver *dd;
    void *system_resource;

    bool coreOpen(yarp::os::Searchable& config);

    // private in order to disable copy
    PolyDriver(const PolyDriver& alt) : DeviceDriver() {
    }

    // private in order to disable copy
    const PolyDriver& operator = (const char *alt) {
        return *this;
    }

    // private in order to disable copy
    const PolyDriver& operator = (const PolyDriver& alt) {
        return *this;
    }
};

#endif

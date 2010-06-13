// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP2_POLYDRIVER__
#define __YARP2_POLYDRIVER__

#include <yarp/dev/Drivers.h>

namespace yarp {
    namespace dev {
        class PolyDriver;
    }
};


/**
 * \ingroup dev_class
 *
 * A container for a device driver.
 */
class yarp::dev::PolyDriver : public DeviceDriver {
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
    PolyDriver(const char *txt) {
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
    bool open(const char *txt);

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
     * Destructor.
     */
    virtual ~PolyDriver();

    virtual bool close() {
        return closeMain();
    }

    /**
     * Get an interface to the device driver.

     * @param x A pointer of type T which will be set to point to this
     * object if that is possible.

     * @return true iff the desired interface is implemented by
     * the device driver.
     */
    template <class T>
    bool view(T *&x) {
        bool result = false;
        x = 0 /*NULL*/;

        // This is not super-portable; and it requires RTTI compiled
        // in.  For systems on which this is a problem, suggest:
        // either replace it with a regular cast (and warn user) or
        // implement own method for checking interface support.
        T *v = dynamic_cast<T *>(dd);

        if (v!=0 /*NULL*/) {
            x = v;
            result = true;
        }
        return result;
    }

    /**
     * Check if device is valid.
     * @return true iff the device was created and configured successfully
     */
    bool isValid() {
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

private:
    bool closeMain();

    DeviceDriver *dd;
    void *system_resource;

    bool coreOpen(yarp::os::Searchable& config);

    // private in order to disable copy
    PolyDriver(const PolyDriver& alt) {
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


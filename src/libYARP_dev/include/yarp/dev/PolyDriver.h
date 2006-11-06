// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
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

    /**
     * Constructor.
     */
    PolyDriver() {
        dd = 0 /*NULL*/;
    }

    /**
     * Construct and configure a device by its common name.
     * @param txt common name of the device
     */
    PolyDriver(const char *txt) {
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
     * Destructor.
     */
    virtual ~PolyDriver() {
        if (dd!=0 /*NULL*/) {
            dd->close();
            delete dd;
            dd = 0 /*NULL*/;
        }
    }

    virtual bool close() {
        bool result = false;
        if (dd!=0 /*NULL*/) {
            result = dd->close();
            delete dd;
            dd = 0 /*NULL*/;
        }
        return result;
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


private:
    DeviceDriver *dd;
};

#endif


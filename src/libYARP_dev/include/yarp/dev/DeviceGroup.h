/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef YARP2_DEVICEGROUP
#define YARP2_DEVICEGROUP

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>


namespace yarp{
    namespace dev {
        class DeviceGroup;
    }
}

/**
 * @ingroup dev_impl_wrapper
 *
 * Lets you make a bunch of devices as a group.
 *
 */
class YARP_dev_API yarp::dev::DeviceGroup : public DeviceDriver,
                                            public IService {

public:
    DeviceGroup() {
        implementation = NULL;
    }

    virtual ~DeviceGroup();

    virtual bool open(yarp::os::Searchable& config);

    virtual bool close() {
        return closeMain();
    }

    virtual bool startService();

    virtual bool stopService() {
        return close();
    }

    virtual bool updateService();

private:
    void *implementation;

    bool closeMain();

    PolyDriver source, sink;

    bool open(const char *key, PolyDriver& poly,
              yarp::os::Searchable& config, const char *comment);
};


#endif

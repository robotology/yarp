// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_POLYDRIVER__
#define __YARP2_POLYDRIVER__

#include <yarp/dev/Drivers.h>

namespace yarp {
    namespace dev {
        class PolyDriver;
    }
};


class yarp::dev::PolyDriver : public DeviceDriver {
public:
    DeviceDriver *dd;

    PolyDriver() {
        dd = NULL;
    }

    PolyDriver(const char *txt) {
        dd = NULL;
        create(txt);
    }

    PolyDriver(const yarp::os::Property& prop) {
        dd = NULL;
        create(prop);
    }

    bool create(const char *txt) {
        dd = Drivers::factory().create(txt);
        return isValid();
    }

    bool create(const yarp::os::Property& prop) {
        dd = Drivers::factory().create(prop);
        return isValid();
    }

    virtual ~PolyDriver() {
        if (dd!=NULL) {
            dd->close();
            delete dd;
            dd = NULL;
        }
    }

    virtual bool close() {
        bool result = false;
        if (dd!=NULL) {
            result = dd->close();
            delete dd;
            dd = NULL;
        }
        return result;
    }

    template <class T>
    bool view(T *&x) {
        bool result = false;
        x = NULL;

        // This is not super-portable; and it requires RTTI compiled
        // in.  For systems on which this is a problem, suggest:
        // either replace it with a regular cast (and warn user) or
        // implement own method for checking interface support.
        T *v = dynamic_cast<T *>(dd);

        if (v!=NULL) {
            x = v;
            result = true;
        }
        return result;
    }

    bool isValid() {
        return dd != NULL;
    }
};

#endif


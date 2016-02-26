#ifndef YARP_DEV_POLYDRIVERLIST_H
#define YARP_DEV_POLYDRIVERLIST_H

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/dev/PolyDriverDescriptor.h>

namespace yarp {
    namespace dev {
        class PolyDriverList;
    }
}

class YARP_dev_API yarp::dev::PolyDriverList
{
public:
    PolyDriverList();
    ~PolyDriverList();

    yarp::dev::PolyDriverDescriptor *operator[](int k);

    const PolyDriverDescriptor *operator[] (int k) const;

    int size() const;

    void push(PolyDriver *p, const char *k);

    void push(PolyDriverDescriptor &v);

    const PolyDriverList &operator=(const PolyDriverList &);

protected:
    //protect use of default copy ctor
    PolyDriverList(const PolyDriverList&);

private:

    void *aceVector;
};

#endif

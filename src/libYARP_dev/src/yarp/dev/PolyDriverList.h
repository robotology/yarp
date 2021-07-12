/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_POLYDRIVERLIST_H
#define YARP_DEV_POLYDRIVERLIST_H

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

    void *descriptors;
};

#endif // YARP_DEV_POLYDRIVERLIST_H

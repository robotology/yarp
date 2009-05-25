// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __POLYDRIVER_LIST
#define __POLYDRIVER_LIST

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#include <yarp/dev/PolyDriverDescriptor.h>

namespace yarp {
    namespace dev {
        class PolyDriverList;
    }
}

class yarp::dev::PolyDriverList
{
public:
    PolyDriverList();
    ~PolyDriverList();

    PolyDriverDescriptor *operator[](int k);
 
    int size();
    
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



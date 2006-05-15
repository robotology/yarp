// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_NAME_
#define _YARP2_NAME_

#include <yarp/String.h>
#include <yarp/Address.h>

namespace yarp {
    class Name;
}

/**
 * Simple abstraction for a YARP name.
 */
class yarp::Name {
public:
    Name(const String& txt);

    bool isRooted() const;

    Address toAddress() const;

private:
    String txt;
};

#endif

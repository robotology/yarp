// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONTACTSTYLE_
#define _YARP2_CONTACTSTYLE_

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class ContactStyle;
    }
}

/**
 * \ingroup comm_class
 *
 * Preferences for how to communicate with a contact.
 *
 */
class YARP_OS_API yarp::os::ContactStyle {
public:
    bool admin;
    bool quiet;
    double timeout;
    ConstString carrier;
    bool expectReply;
    bool persistent;

    explicit ContactStyle();
};

#endif

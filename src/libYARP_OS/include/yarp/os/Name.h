// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NAME
#define YARP2_NAME

#include <yarp/os/Contact.h>

namespace yarp {
    namespace os {
        class Name;
    }
}

/**
 * Simple abstraction for a YARP port name.
 */
class YARP_OS_API yarp::os::Name {
public:

    /**
     * Constructor.
     *
     * @param txt the port name.
     */
    Name(const ConstString& txt);

    /**
     * Check if port name begins with "/"
     *
     *
     * @return true if port name begins with "/"
     */
    bool isRooted() const;

    /**
     * Create an address from the name.  Fills in carrier and name fields.
     * Deals with "tcp://port/name" style syntax.
     *
     * @return a partially filled address (there is no communication with
     * the name server, just parsing of information in the name).
     */
    Contact toAddress() const;

    ConstString getCarrierModifier(const char *mod, bool *hasModifier = NULL);

private:
    ConstString txt;
};

#endif

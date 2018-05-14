/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTINFO_H
#define YARP_OS_PORTINFO_H

#include <string>
#include <yarp/os/Vocab.h>


namespace yarp {
    namespace os {
        class PortInfo;
    }
}


/**
 * \ingroup comm_class
 *
 * Information about a port connection or event.
 */
class YARP_OS_API yarp::os::PortInfo {
public:
    /**
     * Constructor
     */
    explicit PortInfo();

    /// Possible tag values
    enum {
        /// No information.
        PORTINFO_NULL = 0,

        /// Information about an incoming or outgoing connection.
        PORTINFO_CONNECTION = VOCAB4('c', 'o', 'n', 'n'),

        /// Unspecified information.
        PORTINFO_MISC = VOCAB4('m', 'i', 's', 'c')
    };

    /// Type of information. PORTINFO_CONNECTION for information
    /// about an incoming or outgoing connection.
    int tag;

    /// True if a connection is incoming, false if outgoing.
    bool incoming;

    /// True if a connection is created, false if destroyed.
    bool created;

    /// Name of port.
    std::string portName;

    /// Name of connection source, if any.
    std::string sourceName;

    /// Name of connection target, if any.
    std::string targetName;

    /// Name of protocol type, if releveant.
    std::string carrierName;

    /// A human-readable description of contents.
    std::string message;
};


#endif // YARP_OS_PORTINFO_H

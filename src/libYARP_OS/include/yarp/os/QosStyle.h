// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_QOSSTYLE_
#define _YARP2_QOSSTYLE_

#include <yarp/os/Vocab.h>

namespace yarp {
    namespace os {
        class QosStyle;
    }
}

/**
 * \ingroup comm_class
 *
 * Preferences for the port's Quality of Service.
 * All fields have sensible defaults.
 *
 */
class YARP_OS_API yarp::os::QosStyle {
public:

    /**
     * The PacketPriorityType defines the packets
     * quality of service (priority) levels
     */
    enum PacketPriorityType {
        LOW = VOCAB3('L','O','W'),
        NORM = VOCAB4('N','O','R', 'M'),
        HIGH = VOCAB4('H','I','G','H'),
        CRIT = VOCAB4('C','R','I','T')
    };

    /**
     * @brief threadPriority controls the priority of the yarp port's
     * thread (if available) which handles the data transmission.
     * Notice that not every yarp port has dedicated
     * thread for communication (e.g., BufferedPorts have dedicated thread)
     */
    int threadPriority;

    /**
     * @brief threadPolicy controls the scheduling policy of the yarp
     * port's thread (if available) which handles the data transmission.
     * Notice that not every yarp port has dedicated
     * thread for communication (e.g., BufferedPorts have dedicated thread)
     */
    int threadPolicy;


    /**
     * @brief packetPriority controls the communication packets priority levels
     * by adjusting the IPV4/6 DSCP value.
     */
    PacketPriorityType packetPriority;


    /**
     *
     * Constructor.  Sets all options to reasonable defaults.
     *
     */
    explicit QosStyle();
};

#endif

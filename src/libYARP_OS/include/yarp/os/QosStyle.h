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
        NORM = 0,
        LOW  = 10,
        HIGH = 36,
        CRIT = 44
    };

    /**
     * Constructor.  Sets all options to reasonable defaults.
     */
    explicit QosStyle();


    /**
     * @brief sets the packet priority given as DSCP class
     * @param dscp the packet DSCP class
     * @return true if the packet priority is correctly given (e.g., "CS0", "AF42")
     */
    bool setPacketPriorityByDscp(const ConstString& dscp);


    /**
     * @brief sets the packet priority given as PacketPriorityType
     * @param priority the packet priority
     */
    void setPacketPriorityByType(PacketPriorityType priority);


    /**
     * @brief sets the packet priority given as TOS value
     * @param tos the packet TOS
     */
    void setPacketPriority(int tos) {
        packetPriority = tos;
    }


    /**
     * @brief sets the communication thread priority level
     * @param priority the thread's priority
     */
    void setThreadPriority(int priority) {
        threadPriority = priority;
    }


    /**
     * @brief sets the communication thread scheduling policy
     * @param policy the thread's real-time scheduling policy
     */
    void setThreadPolicy(int policy) {
        threadPolicy = policy;
    }


    /**
     * @brief returns the packet TOS value
     * @return the TOS
     */
    int getPacketPriorty() const {
        return packetPriority;
    }


    /**
     * @brief returns the communication thread priority level
     * @return the thread priority
     */
    int getThreadPriority() const {
        return threadPriority;
    }


    /**
     * @brief returns the communication thread scheduling policy
     * @return the thread scheduling policy
     */
    int getThreadPolicy() const {
        return threadPolicy;
    }


    /**
     * @brief returns the IPV4/6 DSCP value given as DSCP code
     * @param vocab a DSCP code (e.g., CS0)
     * @return the actual DSCP value
     */
    static int getDSCPByVocab(int vocab);

private:
    int threadPriority;
    int threadPolicy;
    int packetPriority;

};

#endif

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_QOSSTYLE_H
#define YARP_OS_QOSSTYLE_H

#include <yarp/os/api.h>
#include <yarp/conf/numeric.h>

#include <string>

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * Preferences for the port's Quality of Service.
 * All fields have sensible defaults.
 */
class YARP_os_API QosStyle
{
public:
    /**
     * The PacketPriorityLevel defines the packets
     * quality of service (priority) levels
     */
    enum PacketPriorityLevel
    {
        PacketPriorityInvalid = -1,
        PacketPriorityNormal = 0,
        PacketPriorityLow = 10,
        PacketPriorityHigh = 36,
        PacketPriorityCritical = 44,
        PacketPriorityUndefined = 0x7FFF
    };


    /**
     * The PacketPriorityDSCP defines the packets
     * quality of service (priority) using DSCP
     */
    enum PacketPriorityDSCP
    {
        DSCP_Invalid = -1,
        DSCP_CS0 = 0,
        DSCP_CS1 = 8,
        DSCP_CS2 = 16,
        DSCP_CS3 = 24,
        DSCP_CS4 = 32,
        DSCP_CS5 = 40,
        DSCP_CS6 = 48,
        DSCP_CS7 = 56,
        DSCP_AF11 = 10,
        DSCP_AF12 = 12,
        DSCP_AF13 = 14,
        DSCP_AF21 = 18,
        DSCP_AF22 = 20,
        DSCP_AF23 = 22,
        DSCP_AF31 = 26,
        DSCP_AF32 = 28,
        DSCP_AF33 = 30,
        DSCP_AF41 = 34,
        DSCP_AF42 = 36,
        DSCP_AF43 = 38,
        DSCP_VA = 44,
        DSCP_EF = 46,
        DSCP_Undefined = 0x7FFF
    };


    /**
     * Constructor.  Sets all options to reasonable defaults.
     */
    explicit QosStyle();


    /**
     * @brief sets the packet priority given as DSCP class
     * @param dscp the packet DSCP class
     */
    void setPacketPriorityByDscp(PacketPriorityDSCP dscp);


    /**
     * @brief sets the packet priority given as PacketPriorityLevel
     * @param level the packet priority
     */
    void setPacketPriorityByLevel(PacketPriorityLevel level);


    /**
     * @brief sets the packet priority given as TOS value
     * @param tos the packet TOS
     */
    void setPacketPrioritybyTOS(int tos)
    {
        packetPriority = tos;
    }


    /**
     * @brief sets the packet priority from a string.
     *
     * The string has this syntax:
     *
     * \li LEVEL:[NORM|LOW|HIGH|CRIT]
     * \li DSCP:[CS0|AF11|...]
     * \li TOS:[value]
     *
     * For example the following call
     * \code{.cpp}setPacketPriority("LEVEL:HIGH");\endcode
     * is equivalent to
     * \code{.cpp}setPacketPriorityByLevel(PacketPriorityHigh);\endcode
     *
     * @param priority the string to be interpreted as priority
     * @return true if correctly set, false otherwise
     */
    bool setPacketPriority(const std::string& priority);


    /**
     * @brief sets the communication thread priority level
     * @param priority the thread's priority
     */
    void setThreadPriority(int priority)
    {
        threadPriority = priority;
    }


    /**
     * @brief sets the communication thread scheduling policy
     * @param policy the thread's real-time scheduling policy
     */
    void setThreadPolicy(int policy)
    {
        threadPolicy = policy;
    }


    /**
     * @brief returns the packet TOS value
     * @return the TOS
     */
    int getPacketPriorityAsTOS() const
    {
        return packetPriority;
    }


    /**
     * @brief returns the packet DSCP class
     * @return the DSCP class
     */
    PacketPriorityDSCP getPacketPriorityAsDSCP() const;


    /**
     * @brief returns the packet TOS value
     * @return the TOS
     */
    PacketPriorityLevel getPacketPriorityAsLevel() const;


    /**
     * @brief returns the communication thread priority level
     * @return the thread priority
     */
    int getThreadPriority() const
    {
        return threadPriority;
    }


    /**
     * @brief returns the communication thread scheduling policy
     * @return the thread scheduling policy
     */
    int getThreadPolicy() const
    {
        return threadPolicy;
    }


    /**
     * @brief returns the IPV4/6 DSCP value given as DSCP code
     * @param vocab a DSCP code (e.g., CS0)
     * @return the actual DSCP value
     */
    static PacketPriorityDSCP getDSCPByVocab(yarp::conf::vocab32_t vocab);


    /**
     * @brief returns the priority level
     * @param vocab a level code (e.g., HIGH)
     * @return the priority level
     */
    static PacketPriorityLevel getLevelByVocab(yarp::conf::vocab32_t vocab);


private:
    int threadPriority;
    int threadPolicy;
    int packetPriority;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_QOSSTYLE_H

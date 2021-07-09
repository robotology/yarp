/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_MESSAGESTACK_H
#define YARP_OS_MESSAGESTACK_H

#include <yarp/os/Portable.h>

#include <string>

namespace yarp {
namespace os {

/**
 * Maintain a stack of messages to send asynchronously.
 */
class YARP_os_API MessageStack
{
public:
    /**
     * Constructor.
     *
     * @param max_threads maximum number of worker threads allowed (0 means
     * unlimited)
     */
    explicit MessageStack(size_t max_threads = 0);

    MessageStack(const MessageStack&) = delete;
    MessageStack(MessageStack&&) noexcept = delete;
    MessageStack& operator=(const MessageStack&) = delete;
    MessageStack& operator=(MessageStack&&) = delete;

    /**
     * Destructor.
     */
    virtual ~MessageStack();

    /**
     * @param owner the destination to send messages to
     */
    void attach(PortReader& owner);

    /**
     * Add a message to the message stack, to be sent whenever the gods
     * see fit.
     *
     * @param msg the message to send
     * @param tag an optional string to prefix the message with
     */
    void stack(PortWriter& msg, const std::string& tag = "");

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_MESSAGESTACK_H

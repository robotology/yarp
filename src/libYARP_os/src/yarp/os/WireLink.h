/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_WIRELINK_H
#define YARP_OS_WIRELINK_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class PortReader;
class PortWriter;
class UnbufferedContactable;

/**
 * IDL-friendly object state, used in YARP to set up a port association.
 */
class YARP_os_API WireLink
{
public:
    WireLink();

    ~WireLink();

    /**
     * Check if there is an association of some kind set up for this
     * WireLink.  If not, data has nowhere to go.
     *
     * @return True iff there is an association.
     */
    bool isValid() const;

    /**
     * Set the owner of this WireLink.  Data sent to this link
     * will be passed on to the owner.
     *
     * @param owner owner of this WireLink
     * @return true on success
     */
    bool setOwner(yarp::os::PortReader& owner);

    /**
     * Tag this WireLink as a client, sending data via the specified port.
     * @param port the port to use when sending data.
     * @return true on success
     */
    bool attachAsClient(yarp::os::UnbufferedContactable& port);

    /**
     * Tag this WireLink as a client, sending data via the specified reader.
     *
     * @param reader the reader to use when sending data.
     * @return true on success
     */
    bool attachAsClient(yarp::os::PortReader& reader);

    /**
     * Tag this WireLink as a server, receiving commands via the specified port.
     * @param port the port to monitor for commands.
     * @return true on success
     */
    bool attachAsServer(yarp::os::UnbufferedContactable& port);

    /**
     * For a client WireLink, control whether replies to commands are expected.
     * @param streaming true if replies are unnecessary.
     * @return true on success
     */
    bool setStreamingMode(bool streaming);

    /**
     * Write a message to the associated port or reader.
     * @param writer the message to send.
     * @return true on success
     */
    bool write(PortWriter& writer);

    /**
     * Write a message to the associated port or reader, and read a reply.
     * @param writer the message to send.
     * @param reader a recipient for the reply.
     * @return true on success
     */
    bool write(const PortWriter& writer, PortReader& reader) const;

    /**
     * Put a message in a stack to call later, asynchronously.  Used
     * in implementation of thrift "oneway" messages.
     * @param writer message to send
     * @param reader where to send the message
     * @param tag string to prefix the message with
     * @return true on success
     */
    bool callback(PortWriter& writer, PortReader& reader, const std::string& tag = "");

    /**
     * @return true if writing is allowed over link.
     */
    bool canWrite() const;

    /**
     * @return true if reading from the link is allowed.
     */
    bool canRead() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_WIRELINK_H

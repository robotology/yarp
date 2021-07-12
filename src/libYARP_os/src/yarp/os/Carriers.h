/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CARRIERS_H
#define YARP_OS_CARRIERS_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/Carrier.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Face.h>
#include <yarp/os/OutputProtocol.h>

#include <string>


namespace yarp {
namespace os {

/**
 * Collection of carriers, a singleton.
 *
 * This is the starting point for creating connections
 * between ports.
 */
class YARP_os_API Carriers
{
    Carriers();

public:

    /**
     * Destructor.
     */
    virtual ~Carriers();

    /**
     * Select a carrier by name.
     *
     * @param name the name of the desired carrier.
     * @return the desired carrier, or nullptr if not found.
     */
    static Carrier* chooseCarrier(const std::string& name);

    /**
     * Get template for carrier.
     */
    static Carrier* getCarrierTemplate(const std::string& name);

    /**
     * Select a carrier by 8-byte header.
     *
     * @param bytes the 8-byte header describing the desired carrier.
     * @return the desired carrier, or nullptr if not found.
     */
    static Carrier* chooseCarrier(const Bytes& bytes);

    /**
     * Create a "proto-carrier" interface object that waits for
     * incoming connections prior to a carrier being selected via
     * handshaking.
     *
     * Currently, this is always a tcp server socket.  There is no
     * reason why it couldn't be any kind of stream, and this would be
     * the method to change if you want to do something imaginative
     * here.
     *
     * @param address the address (including initial carrier type) to
     *                listen to.
     * @return the interface object.
     */
    static Face* listen(const Contact& address);

    /**
     * Initiate a connection to an address.
     *
     * @param address the address (including desired carrier type) to
     *                connect to.
     * @return the protocol object.
     */
    static OutputProtocol* connect(const Contact& address);

    /**
     * Remove all carriers.
     */
    void clear();

    /**
     * Add a new connection type.
     *
     * @param carrier a prototype of the desired connection type.
     *                The YARP library will be responsible for
     *                destroying it on shutdown.
     * @return true on success.
     */
    static bool addCarrierPrototype(Carrier* carrier);

    static Carriers& getInstance();

    static Bottle listCarriers();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class Private;
    Private* const mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_CARRIERS_H

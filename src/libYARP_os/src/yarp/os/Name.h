/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NAME_H
#define YARP_OS_NAME_H

#include <yarp/os/Contact.h>

namespace yarp {
namespace os {

/**
 * Simple abstraction for a YARP port name.
 */
class YARP_os_API Name
{
public:
    /**
     * Constructor.
     *
     * @param txt the port name.
     */
    Name(const std::string& txt);

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

    std::string getCarrierModifier(const char* mod, bool* hasModifier = nullptr);

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) txt;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NAME_H

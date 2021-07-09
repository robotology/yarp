/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTABLE_H
#define YARP_OS_PORTABLE_H

#include <yarp/os/api.h>

#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * This is a base class for objects that can be both read from
 * and be written to the YARP network.  It is a simple union of
 * PortReader and PortWriter.
 */
class YARP_os_API Portable : public PortReader, public PortWriter
{
public:
    // reiterate the key inherited virtual methods, just as a reminder

    bool read(ConnectionReader& reader) override = 0;
    bool write(ConnectionWriter& writer) const override = 0;

    virtual Type getType() const;

    /**
     * Copy one portable to another, via writing and reading.
     *
     * @return true iff writer.write and reader.read both succeeded.
     */
    static bool copyPortable(const PortWriter& writer, PortReader& reader);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTABLE_H

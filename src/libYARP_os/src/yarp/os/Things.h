/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_THINGS_H
#define YARP_OS_THINGS_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Portable.h>

namespace yarp {
namespace os {

/**
 * Base class for generic things.
 */
class YARP_os_API Things
{
public:
    Things();

    virtual ~Things();

    /**
     * Set the reference to a PortWriter object
     */
    void setPortWriter(yarp::os::PortWriter* writer);

    yarp::os::PortWriter* getPortWriter();

    /**
     * Set the reference to a PortReader object
     */
    void setPortReader(yarp::os::PortReader* reader);

    yarp::os::PortReader* getPortReader();

    /**
     *  set a reference to a ConnectionReader
     */
    bool setConnectionReader(yarp::os::ConnectionReader& reader);

    /*
     * Things writer
     */
    bool write(yarp::os::ConnectionWriter& connection);

    void reset();

    bool hasBeenRead();

    template <typename T>
    T* cast_as()
    {
        if (this->writer) {
            return dynamic_cast<T*>(this->writer);
        }
        if (this->reader) {
            return dynamic_cast<T*>(this->reader);
        }
        if (!this->portable) {
            if (!this->conReader) {
                return nullptr;
            }
            this->portable = new T();
            if (!this->portable->read(*this->conReader)) {
                delete this->portable;
                this->portable = nullptr;
                return nullptr;
            }
            beenRead = true;
        }
        return dynamic_cast<T*>(this->portable);
    }

private:
    bool beenRead;
    yarp::os::ConnectionReader* conReader;
    yarp::os::PortWriter* writer;
    yarp::os::PortReader* reader;
    yarp::os::Portable* portable;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_THINGS_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TYPEDREADERCALLBACK_H
#define YARP_OS_TYPEDREADERCALLBACK_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

template <typename T>
class TypedReader;

/**
 * A callback for typed data from a port.
 *
 * If you want to get your data while leaving the port free to read more in the
 * background, you can create a callback that implements this interface, and
 * tell the port about it using BufferedPort::useCallback(callback) or
 * PortReaderBuffer::useCallback(callback)
 */
template <typename T>
class TypedReaderCallback
{
public:
    /**
     * Destructor.
     */
    virtual ~TypedReaderCallback();

    /**
     * Callback method.
     *
     * @param datum data read from a port
     */
    virtual void onRead(T& datum);

    /**
     * Callback method.
     *
     * Passes along source of callback.
     * By default, this calls the version of onRead that just takes a
     * datum.
     *
     * @param datum data read from a port
     * @param reader the original port (or delegate object)
     */
    virtual void onRead(T& datum, const yarp::os::TypedReader<T>& reader);
};

} // namespace os
} // namespace yarp

#include <yarp/os/TypedReaderCallback-inl.h>


#endif // YARP_OS_TYPEDREADERCALLBACK_H

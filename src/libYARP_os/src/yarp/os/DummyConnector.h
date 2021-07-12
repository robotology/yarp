/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2007 Freyr Magnusson
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_DUMMYCONNECTOR_H
#define YARP_OS_DUMMYCONNECTOR_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

namespace yarp {
namespace os {

/**
 * A dummy connection to test yarp::os::Portable implementations.
 * This class provides simple means to test read and write methods of the Portable interface.
 *
 * Pseudocode for a test should look something like this:
 * \code{.cpp}
 *   DummyConnector dummy; // create new connection with textmode=false
 *   PortableObject object;
 *   ... // set object values
 *   object.write(dummy.getWriter()); // reset the connection and then write to it
 *   PortableObject newObject;
 *   newObject.read(dummy.getReader()); // write from the connection to the new object
 *   ... // assert newObject values
 * \endcode
 */
class YARP_os_API DummyConnector
{
public:
    /**
     * Default constructor. TextMode set to false.
     */
    DummyConnector();


    /**
     * Destructor.
     */
    virtual ~DummyConnector();

    /**
     * Set the textMode of the dummy connection
     * \param textmode new textmode value
     */
    void setTextMode(bool textmode);

    /**
     * Get a clean/empty dummy ConnectionWriter. This will reset the connection each time.
     * \return a empty dummy ConnectionWriter
     * \sa ConnectionWriter Portable
     */
    ConnectionWriter& getCleanWriter();

    /**
     * Get the dummy ConnectionWriter loaded with whatever was written the ConnectionWriter since
     * it was last reset.
     * \return the current ConnectionWriter
     * \sa ConnectionWriter Portable
     */
    ConnectionWriter& getWriter();

    /**
     * Get the dummy ConnectionReader loaded with whatever was written the ConnectionWriter since
     * it was last reset.
     * \param replyWriter A writer to be used for replies received on the connection.
     * \return a loaded ConnectionReader if it was previously written to
     * \sa ConnectionReader Portable
     * \warning Calling this method twice will reset the reader
     */
    ConnectionReader& getReader(ConnectionWriter* replyWriter = nullptr);

    /**
     * Reset and clear the current ConnectionWriter
     */
    void reset();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_DUMMYCONNECTOR_H

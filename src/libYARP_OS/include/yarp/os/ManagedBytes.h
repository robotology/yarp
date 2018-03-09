/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_MANAGEDBYTES_H
#define YARP_OS_MANAGEDBYTES_H

#include <yarp/os/Bytes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        class ManagedBytes;
    }
}

/**
 * An abstraction for a block of bytes, with optional
 * responsibility for allocating/destroying that block.
 */
class YARP_OS_API yarp::os::ManagedBytes : public Portable {
public:
    /**
     * Constructor.  No data present.
     */
    explicit ManagedBytes();

    /**
     * Constructor. Makes a data block of the specified length that will
     * be deleted if this object is destroyed.
     * @param len length of data block
     */
    explicit ManagedBytes(size_t len);

    /**
     * Constructor. Represent external data.
     * @param ext address and length of data
     * @param owned true if data should be deleted if this object is destroyed
     */
    ManagedBytes(const Bytes& ext, bool owned = false);

    /**
     * Copy constructor.
     * @param alt the data to copy.  If it is "owned" an independent copy
     * is made.
     */
    ManagedBytes(const ManagedBytes& alt);

    /**
     * Assignment operator.
     * @param alt the data to copy.  If it is "owned" an independent copy
     * is made.
     * @return this object
     */
    const ManagedBytes& operator = (const ManagedBytes& alt);

    /**
     * Destructor.
     */
    virtual ~ManagedBytes();

    /**
     * Makes a data block of the specified length that will
     * be deleted if this object is destroyed.
     * @param len length of data block
     */
    void allocate(size_t len);

    bool allocateOnNeed(size_t neededLen, size_t allocateLen);

    /**
     * Makes sure data block is owned, making a copy if necessary.
     */
    void copy();

    /**
     * @return length of data block
     */
    size_t length() const;

    /**
     * @return length of used portion of data block - by default, this
     * is the same as length(), unless setUsed() is called
     */
    size_t used() const;

    /**
     * @return address of data block
     */
    char *get() const;

    /**
     * Disassociate object with any data block (deleting block if appropriate).
     */
    void clear();

    /**
     * @return description of data block associated with this object
     */
    const Bytes& bytes();


    /**
     * @return description of used portion of data block associated
     * with this object
     */
    Bytes usedBytes();


    /**
     *
     * explicitly declare how many of the bytes are in use.
     *
     * @param used byte count
     *
     * @return a confirmation of the number of bytes declared to be in use.
     *
     */
    size_t setUsed(size_t used);

    size_t resetUsed();

    bool read(ConnectionReader& reader) override;

    bool write(ConnectionWriter& writer) override;

    /**
     *
     * @return true iff the managed data block is owned by this object
     *
     */
    bool isOwner() const {
        return owned;
    }

    /**
     * @brief Use this to wrap an external blob of data.
     * @param extData, pointer to the external data.
     * @param len, size in bytes of the external blob
     */
    void setExternal(const char* extData, size_t len);

private:
    Bytes b;
    bool owned;
    size_t use;
    bool use_set;
};

#endif // YARP_OS_MANAGEDBYTES_H

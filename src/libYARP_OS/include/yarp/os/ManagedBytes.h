// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_MANAGEDBYTES
#define YARP2_MANAGEDBYTES

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

    bool read(ConnectionReader& reader);

    bool write(ConnectionWriter& writer);

    /**
     *
     * @return true iff the managed data block is owned by this object
     *
     */
    bool isOwner() const {
        return owned;
    }

private:
    Bytes b;
    bool owned;
    size_t use;
    bool use_set;
};

#endif

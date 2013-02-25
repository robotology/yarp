// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MANAGEDBYTES_
#define _YARP2_MANAGEDBYTES_

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
    ManagedBytes() {
        b = Bytes(0/*NULL*/,0);
        owned = false;
        use = 0;
        use_set = false;
    }

    /**
     * Constructor. Represent external data.
     * @param ext address and length of data
     * @param owned true if data should be deleted if this object is destroyed
     */
    ManagedBytes(const Bytes& ext, bool owned = false) {
        b = ext;
        this->owned = owned;
        use = 0;
        use_set = false;
    }

    /**
     * Copy constructor.
     * @param alt the data to copy.  If it is "owned" an independent copy
     * is made.
     */
    ManagedBytes(const ManagedBytes& alt) : Portable() {
        b = alt.b;
        use = alt.use;
        use_set = alt.use_set;
        owned = false;
        if (alt.owned) {
            copy();
        }
    }

    /**
     * Assignment operator.
     * @param alt the data to copy.  If it is "owned" an independent copy
     * is made.
     * @return this object
     */
    const ManagedBytes& operator = (const ManagedBytes& alt) {
        clear();
        b = alt.b;
        use = alt.use;
        use_set = alt.use_set;
        owned = false;
        if (alt.owned) {
            copy();
        }
        return *this;
    }


    /**
     * Constructor. Makes a data block of the specified length that will
     * be deleted if this object is destroyed.
     * @param len length of data block
     */
    ManagedBytes(size_t len) {
        char *buf = new char[len];
        NetworkBase::assertion(buf!=0/*NULL*/);
        b = Bytes(buf,len);
        owned = true;
        use = 0;
        use_set = false;
    }

    /**
     * Makes a data block of the specified length that will
     * be deleted if this object is destroyed.
     * @param len length of data block
     */
    void allocate(size_t len) {
        clear();
        char *buf = new char[len];
        b = Bytes(buf,len);
        owned = true;
        use = 0;
        use_set = false;
    }

    bool allocateOnNeed(size_t neededLen, size_t allocateLen);

    /**
     * Makes sure data block is owned, making a copy if necessary.
     */
    void copy();

    /**
     * @return length of data block
     */
    size_t length() const {
        return b.length();
    }

    /**
     * @return length of used portion of data block - by default, this
     * is the same as length(), unless setUsed() is called
     */
    size_t used() const {
        return use_set?use:length();
    }

    /**
     * @return address of data block
     */
    char *get() const {
        return b.get();
    }

    /**
     * Destructor.
     */
    virtual ~ManagedBytes() {
        clear();
    }

    /**
     * Disassociate object with any data block (deleting block if appropriate).
     */
    void clear() {
        if (owned) {
            if (get()!=0) {
                delete[] get();
            }
            owned = 0;
        }
        b = Bytes(0/*NULL*/,0);
        use = 0;
        use_set = false;
    }

    /**
     * @return description of data block associated with this object
     */
    const Bytes& bytes() {
        return b;
    }


    /**
     * @return description of used portion of data block associated
     * with this object
     */
    Bytes usedBytes() {
        return Bytes(get(),used());
    }


    /**
     *
     * explicitly declare how many of the bytes are in use.
     *
     * @param used byte count
     *
     * @return a confirmation of the number of bytes declared to be in use.
     *
     */
    size_t setUsed(size_t used) {
        use_set = true;
        use = used;
        return this->used();
    }

    size_t resetUsed() {
        use = 0;
        use_set = false;
        return this->used();
    }

    bool read(ConnectionReader& reader);

    bool write(ConnectionWriter& writer);

private:
    Bytes b;
    bool owned;
    size_t use;
    bool use_set;
};

#endif

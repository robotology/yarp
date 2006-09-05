// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_MANAGEDBYTES_
#define _YARP2_MANAGEDBYTES_

#include <yarp/os/Bytes.h>
#include <yarp/os/Network.h>

namespace yarp {
    namespace os {
        class ManagedBytes;
    }
}

/**
 * An abstraction for a block of bytes, with optional
 * responsibility for allocating/destroying that block.
 */
class yarp::os::ManagedBytes {
public:
    /**
     * Constructor.  No data present.
     */
    ManagedBytes() {
        b = Bytes(0/*NULL*/,0);
        owned = false;
    }

    /**
     * Constructor. Represent external data.
     * @param ext address and length of data
     * @param owned true if data should be deleted if this object is destroyed
     */
    ManagedBytes(const Bytes& ext, bool owned = false) {
        b = ext;
        this->owned = owned;
    }

    /**
     * Copy constructor.
     * @param alt the data to copy.  If it is "owned" an independent copy
     * is made.
     */
    ManagedBytes(const ManagedBytes& alt) {
        b = alt.b;
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
    ManagedBytes(int len) {
        char *buf = new char[len];
        yarp::os::Network::assertion(buf!=0/*NULL*/);
        b = Bytes(buf,len);
        owned = true;
    }

    /**
     * Makes a data block of the specified length that will
     * be deleted if this object is destroyed.
     * @param len length of data block
     */
    void allocate(int len) {
        clear();
        char *buf = new char[len];
        b = Bytes(buf,len);
        owned = true;
    }

    /**
     * Makes sure data block is owned, making a copy if necessary.
     */
    void copy();

    /**
     * @return length of data block
     */
    int length() const {
        return b.length();
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
    }

    /**
     * @return description of data block associated with this object
     */
    const Bytes& bytes() {
        return b;
    }

private:
    Bytes b;
    bool owned;
};

#endif

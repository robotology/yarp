// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPCXX_Portable_INC
#define YARPCXX_Portable_INC

#include <yarp/yarpcxx.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

namespace yarpcxx {
    namespace os {
        class Portable;
    }
}

class yarpcxx::os::Portable {
public:
    Portable() {
        handle.implementation = this;
        handle.write = __impl_write;
        handle.read = __impl_read;
        handle.onCompletion = __impl_onCompletion;
        handle.onCommencement = __impl_onCommencement;
    }

    virtual bool write(ConnectionWriter& connection) { return false; }

    virtual bool read(ConnectionReader& connection) { return false; }

    virtual void onCommencement() {}

    virtual void onCompletion() {}

    virtual ~Portable() {
    }

    yarpPortablePtr getHandle() {
        return &handle;
    }
private:
    yarpPortable handle;

    static int __impl_write(yarpWriterPtr connection, void *impl) {
        ConnectionWriter wrap(connection);
        printf("impl is %ld\n", (void*)impl);
        return ((Portable*)impl)->write(wrap);
    }

    static int __impl_read(yarpReaderPtr connection, void *impl) {
        ConnectionReader wrap(connection);
        printf("impl is %ld\n", (void*)impl);
        return ((Portable*)impl)->read(wrap);
    }

    static int __impl_onCommencement(void *impl) {
        ((Portable*)impl)->onCommencement();
        return 0;
    }

    static int __impl_onCompletion(void *impl) {
        ((Portable*)impl)->onCompletion();
        return 0;
    }
};

#endif


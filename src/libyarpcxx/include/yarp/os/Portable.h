/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
        yarpPortableCallbacks callbacks;
        callbacks.write = __impl_write;
        callbacks.read = __impl_read;
        callbacks.onCompletion = __impl_onCompletion;
        callbacks.onCommencement = __impl_onCommencement;
        yarpPortableCallbacksInstall(&callbacks);
        yarpPortableInit(&handle,NULL);
        handle.client = this;
    }
    
    virtual ~Portable() {
        yarpPortableFini(&handle);
    }

    virtual bool write(ConnectionWriter& connection) { return false; }

    virtual bool read(ConnectionReader& connection) { return false; }

    virtual void onCommencement() {}

    virtual void onCompletion() {}

    yarpPortablePtr getHandle() {
        return &handle;
    }
private:
    yarpPortable handle;

    static int __impl_write(yarpWriterPtr connection, void *client) {
        ConnectionWriter wrap(connection);
        return !((Portable*)client)->write(wrap);
    }

    static int __impl_read(yarpReaderPtr connection, void *client) {
        ConnectionReader wrap(connection);
        return !((Portable*)client)->read(wrap);
    }

    static int __impl_onCommencement(void *client) {
        ((Portable*)client)->onCommencement();
        return 0;
    }

    static int __impl_onCompletion(void *client) {
        ((Portable*)client)->onCompletion();
        return 0;
    }
};

#endif


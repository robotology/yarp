// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHAREDLIBRARYCLASS_
#define _YARP2_SHAREDLIBRARYCLASS_

#include <yarp/os/SharedLibraryClassFactory.h>

namespace yarp {
    namespace os {
        template <class T>
        class SharedLibraryClass;
    }
}


    /**
     *
     * Container for objects created in a DLL.
     * With DLLs compiled against different runtimes, it may be important
     * to match creation and destruction methods.  The easiest way
     * to do this is to isolate creation and destruction of a
     * particular class to a particular DLL.
     *
     */
template <class T>
class yarp::os::SharedLibraryClass {
private:
    void *deleter;
    T *content;
public:

    SharedLibraryClass() {
        content = 0/*NULL*/;
    }

    SharedLibraryClass(SharedLibraryClassFactory<T>& factory) {
        content = 0/*NULL*/;
        open(factory);
    }

    bool open(SharedLibraryClassFactory<T>& factory) {
        content = factory.create();
        deleter = factory.getDestroyFn();
        return content!=0/*NULL*/;
    }

    virtual bool close() {
        if (content!=0/*NULL*/) {
            if (deleter!=0/*NULL*/) {
                SharedClassDeleterFunction del =
                    (SharedClassDeleterFunction)deleter;
                del(content);
            } else {
                delete content;
            }
        }
        content = 0/*NULL*/;
        return true;
    }

    virtual ~SharedLibraryClass() {
        close();
    }

    T& getContent() {
        return *content;
    }

    bool isValid() const {
        return content!=0/*NULL*/;
    }

    T& operator*() {
        return (*content);
    }

    T *operator->() {
        return (content);
    }
};


#endif


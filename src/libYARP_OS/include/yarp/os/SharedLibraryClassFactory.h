/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_SHAREDLIBRARYCLASSFACTORY_H
#define YARP_OS_SHAREDLIBRARYCLASSFACTORY_H

#include <yarp/os/SharedLibraryFactory.h>

namespace yarp {
    namespace os {
        template <class T>
        class SharedLibraryClassFactory;
    }
}


/**
 * A type-safe wrapper for SharedLibraryFactory, committing to
 * creation/destruction of instances of a particular super-class.
 * Note that we take on faith that the named factory method in the 
 * named shared library does in fact create the named type.
 */
template <class T>
class yarp::os::SharedLibraryClassFactory : public SharedLibraryFactory {
public:
    SharedLibraryClassFactory() {
    }

    SharedLibraryClassFactory(const char *dll_name, const char *fn_name = 0/*NULL*/) : SharedLibraryFactory(dll_name,fn_name) {
    }

    T *create() {
        if (!isValid()) return 0/*NULL*/;
        return (T *)getApi().create();
    }

    void destroy(T *obj) {
        if (!isValid()) return;
        getApi().destroy(obj);
    }
};

#endif // YARP_OS_SHAREDLIBRARYCLASSFACTORY_H

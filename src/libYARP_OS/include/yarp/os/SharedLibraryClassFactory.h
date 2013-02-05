// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPSHAREDLIBRARYCLASSFACTORY_
#define _YARP2_YARPSHAREDLIBRARYCLASSFACTORY_

#include <yarp/os/SharedLibraryFactory.h>

namespace yarp {
    namespace os {
        template <class T>
        class SharedLibraryClassFactory;
    }
}

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

#endif

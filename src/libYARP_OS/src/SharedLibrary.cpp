/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/SharedLibrary.h>

#include <yarp/conf/system.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Log.h>

#ifdef YARP_HAS_ACE
#  include <ace/ACE.h>
#  include <ace/DLL.h>
#else
#  include <yarp/os/impl/PlatformDlfcn.h>
#endif


using yarp::os::SharedLibrary;
using yarp::os::ConstString;
using yarp::os::impl::SharedLibraryImpl;


class yarp::os::impl::SharedLibraryImpl
{
public:
    SharedLibraryImpl() : dll(YARP_NULLPTR), error(ConstString()) {}

    inline char* getError()
    {
#ifdef YARP_HAS_ACE
        if(dll != YARP_NULLPTR)
            return dll->error();
        else
            return const_cast<char*>("Unknown error");
#else
        return yarp::os::impl::dlerror();
#endif
    }

#ifdef YARP_HAS_ACE
    ACE_DLL* dll;
#else
    void* dll;
#endif
    ConstString error;
};


SharedLibrary::SharedLibrary() :
    implementation(new SharedLibraryImpl())
{
    yAssert(implementation != YARP_NULLPTR);
}

SharedLibrary::SharedLibrary(const char *filename) :
    implementation(new SharedLibraryImpl())
{
    yAssert(implementation != YARP_NULLPTR);
    open(filename);
}

SharedLibrary::~SharedLibrary()
{
    yAssert(implementation != YARP_NULLPTR);
    close();
    delete implementation;
}

bool SharedLibrary::open(const char *filename)
{
    close();
#ifdef YARP_HAS_ACE
    implementation->dll = new ACE_DLL();
    yAssert(implementation->dll);
    int result = implementation->dll->open(filename);
    if (result != 0) {
        // Save error since close might overwrite it
        ConstString error(implementation->getError());
        close();
        implementation->error = error;
        return false;
    }
    return true;
#else
    implementation->dll = yarp::os::impl::dlopen(filename, RTLD_LAZY);
    if (!implementation->dll) {
        implementation->error = implementation->getError();
        return false;
    }
    return true;
#endif
}

bool SharedLibrary::close()
{
    int result = 0;
    if (implementation->dll != YARP_NULLPTR) {
#ifdef YARP_HAS_ACE
        result = implementation->dll->close();
        delete implementation->dll;
#else
        result = yarp::os::impl::dlclose(implementation->dll);
#endif
        implementation->dll = YARP_NULLPTR;
    }

    if (result != 0) {
        implementation->error = implementation->getError();
    }

    return (result == 0);
}

ConstString SharedLibrary::error()
{
    return SharedLibrary::implementation->error;
}

void *SharedLibrary::getSymbol(const char *symbolName)
{
    if (!implementation->dll) {
        implementation->error = "Library is not open";
        return YARP_NULLPTR;
    }

#ifdef YARP_HAS_ACE
    void* result = implementation->dll->symbol(symbolName);
#else
    void* result = yarp::os::impl::dlsym(implementation->dll, symbolName);
#endif
    if (!result) {
        implementation->error = implementation->getError();
    }

    return result;
}

bool SharedLibrary::isValid() const
{
    return implementation->dll != YARP_NULLPTR;
}

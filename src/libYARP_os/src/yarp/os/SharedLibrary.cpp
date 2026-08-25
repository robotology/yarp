/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SharedLibrary.h>

#include <yarp/conf/system.h>

#include <yarp/os/Log.h>

#include <string>

#ifdef YARP_HAS_ACE
#    include <ace/ACE.h>
#    include <ace/DLL.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <yarp/os/impl/PlatformDlfcn.h>
#endif


using yarp::os::SharedLibrary;
using yarp::os::impl::SharedLibraryImpl;


class yarp::os::impl::SharedLibraryImpl
{
public:
    SharedLibraryImpl() = default;

    inline char* getError()
    {
#ifdef YARP_HAS_ACE
        if (dll != nullptr) {
            return dll->error();
        }
        return const_cast<char*>("Unknown error");
#else
        return yarp::os::impl::dlerror();
#endif
    }

#ifdef YARP_HAS_ACE
    ACE_DLL* dll{nullptr};
#else
    void* dll;
#endif
    std::string error;
};


SharedLibrary::SharedLibrary() :
        implementation(new SharedLibraryImpl())
{
    yAssert(implementation != nullptr);
}

SharedLibrary::SharedLibrary(const char* filename) :
        implementation(new SharedLibraryImpl())
{
    yAssert(implementation != nullptr);
    open(filename);
}

SharedLibrary::~SharedLibrary()
{
    yAssert(implementation != nullptr);
    close();
    delete implementation;
}

bool SharedLibrary::open(const char* filename)
{
    close();
#ifdef YARP_HAS_ACE
    implementation->dll = new ACE_DLL();
    yAssert(implementation->dll);
    int result = implementation->dll->open(filename);
    if (result != 0) {
        // Save error since close might overwrite it
        std::string error(implementation->getError());
        close();
        implementation->error = error;
        return false;
    }
    return true;
#else
    // NOTE: RTLD_NODELETE is required to work around a known glibc issue
    // (see e.g. glibc bugzilla #24773 and related reports) where a real
    // dlclose() of a library that pulled in NSS modules (via
    // getaddrinfo()/gethostbyname(), used internally by YARP for name
    // resolution) or that registered thread-specific data destructors,
    // can lead to a SIGSEGV in __nptl_deallocate_tsd() when a thread
    // that used those symbols terminates *after* the library has been
    // unmapped. RTLD_NODELETE keeps the reference counting semantics of
    // dlopen/dlclose (so open()/close() still behave correctly), but
    // guarantees the library is never actually unmapped from memory,
    // sidestepping the glibc bug entirely.
#ifndef RTLD_NODELETE
#    define RTLD_NODELETE 0
#endif
    implementation->dll = yarp::os::impl::dlopen(filename, RTLD_LAZY | RTLD_NODELETE);
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
    if (implementation->dll != nullptr) {
#ifdef YARP_HAS_ACE
        result = implementation->dll->close();
        delete implementation->dll;
#else
        result = yarp::os::impl::dlclose(implementation->dll);
#endif
        implementation->dll = nullptr;
    }

    if (result != 0) {
        implementation->error = implementation->getError();
    }

    return (result == 0);
}

std::string SharedLibrary::error()
{
    return SharedLibrary::implementation->error;
}

void* SharedLibrary::getSymbol(const char* symbolName)
{
    if (implementation->dll == nullptr) {
        implementation->error = "Library is not open";
        return nullptr;
    }

#ifdef YARP_HAS_ACE
    void* result = implementation->dll->symbol(symbolName);
#else
    void* result = yarp::os::impl::dlsym(implementation->dll, symbolName);
#endif
    if (result == nullptr) {
        implementation->error = implementation->getError();
    }

    return result;
}

bool SharedLibrary::isValid() const
{
    return implementation->dll != nullptr;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SHAREDLIBRARYCLASSFACTORY_H
#define YARP_OS_SHAREDLIBRARYCLASSFACTORY_H

#include <yarp/os/SharedLibraryFactory.h>

namespace yarp {
namespace os {

/**
 * A type-safe wrapper for SharedLibraryFactory, committing to
 * creation/destruction of instances of a particular super-class.
 * Note that we take on faith that the named factory method in the
 * named shared library does in fact create the named type.
 */
template <class T>
class SharedLibraryClassFactory : public SharedLibraryFactory
{
public:
    SharedLibraryClassFactory() = default;

    SharedLibraryClassFactory(const char* dll_name, const char* fn_name = nullptr) :
            SharedLibraryFactory(dll_name, fn_name)
    {
    }

    T* create()
    {
        if (!isValid()) {
            return nullptr;
        }
        return (T*)getApi().create();
    }

    void destroy(T* obj) const
    {
        if (!isValid()) {
            return;
        }
        getApi().destroy(obj);
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SHAREDLIBRARYCLASSFACTORY_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SHAREDLIBRARYCLASSAPI_H
#define YARP_OS_SHAREDLIBRARYCLASSAPI_H

#include <yarp/conf/system.h>

#include <yarp/os/Vocab.h>

#include <cstring>

// Be careful loading C++ classes from DLLs.  Generally you
// need an exact or very close match between compilers used
// to compile those DLLs and your own code.

#define YARP_SHAREDLIBRARYCLASSAPI_PADDING (30 - 2 * (YARP_POINTER_SIZE / 4))

namespace yarp {
namespace os {

extern "C" {

/**
 * Collection of hooks for creating/destroying a plugin.
 * Be careful to check carefully for compatibility before
 * using create() or destroy().
 */
YARP_BEGIN_PACK
struct SharedLibraryClassApi
{
public:
    NetInt32 startCheck; // Constant: this should be 'Y' 'A' 'R' 'P'.
                         // Don't touch anything further if it isn't.
    NetInt32 structureSize; // size of the SharedLibraryClassApi.
                            // If this doesn't match what you expect,
                            // Don't touch anything further if it isn't.
    NetInt32 systemVersion; // Overall version of plugin system.
                            // This does *not* cover compiler version etc.
    void* (*create)();                     // Instantiate a plugin object.
    void (*destroy)(void* obj);            // Destroy a plugin object.
    int (*getVersion)(char* ver, int len); // Plugin-related version.
    int (*getAbi)(char* abi, int len);     // Compiler-related version.
    int (*getClassName)(char* name, int len); // Name of plugin (subclass).
    int (*getBaseClassName)(char* name, int len); // Name superclass.
    NetInt32 roomToGrow[YARP_SHAREDLIBRARYCLASSAPI_PADDING]; // Padding.
    NetInt32 endCheck;      // Constant: should be 'P' 'L' 'U' 'G'.
};
YARP_END_PACK

} // extern "C"

} // namespace os
} // namespace yarp

#define YARP_SHARED_CLASS_FN extern "C" YARP_EXPORT

/**
 *
 * Macro to create a bunch of functions with undecorated names that can
 * be found within a plugin library to handle creation/deletion of that
 * plugin.  Use with care.
 *
 * @param factoryname the name of the "hook" function to make.  A collection
 * of other helper functions with names composed of the factoryname with
 * _create/_destroy/... appended.
 *
 * @param classname the class that the hook will be able to instantiate.
 *
 * @param basename the superclass that the user of the plugin should be
 * working with.
 *
 */
#define YARP_DEFINE_SHARED_SUBCLASS(factoryname, classname, basename)                                \
    YARP_SHARED_CLASS_FN void* factoryname##_create()                                                \
    {                                                                                                \
        classname* cn = new classname;                                                               \
        basename* bn = dynamic_cast<basename*>(cn);                                                  \
        if (!bn)                                                                                     \
            delete cn;                                                                               \
        return static_cast<void*>(bn);                                                               \
    }                                                                                                \
    YARP_SHARED_CLASS_FN void factoryname##_destroy(void* obj)                                       \
    {                                                                                                \
        classname* cn = dynamic_cast<classname*>(static_cast<basename*>(obj));                       \
        if (cn)                                                                                      \
            delete cn;                                                                               \
    }                                                                                                \
    YARP_SHARED_CLASS_FN int factoryname##_getVersion(char* ver, int len)                            \
    {                                                                                                \
        return 0;                                                                                    \
    }                                                                                                \
    YARP_SHARED_CLASS_FN int factoryname##_getAbi(char* abi, int len)                                \
    {                                                                                                \
        return 0;                                                                                    \
    }                                                                                                \
    YARP_SHARED_CLASS_FN int factoryname##_getClassName(char* name, int len)                         \
    {                                                                                                \
        char cname[] = #classname;                                                                   \
        strncpy(name, cname, len);                                                                   \
        return strlen(cname) + 1;                                                                    \
    }                                                                                                \
    YARP_SHARED_CLASS_FN int factoryname##_getBaseClassName(char* name, int len)                     \
    {                                                                                                \
        char cname[] = #basename;                                                                    \
        strncpy(name, cname, len);                                                                   \
        return strlen(cname) + 1;                                                                    \
    }                                                                                                \
    YARP_SHARED_CLASS_FN int factoryname(void* api, int len)                                         \
    {                                                                                                \
        struct yarp::os::SharedLibraryClassApi* sapi = (struct yarp::os::SharedLibraryClassApi*)api; \
        if (len < (int)sizeof(yarp::os::SharedLibraryClassApi))                                      \
            return -1;                                                                               \
        sapi->startCheck = yarp::os::createVocab32('Y', 'A', 'R', 'P');                                \
        sapi->structureSize = sizeof(yarp::os::SharedLibraryClassApi);                               \
        sapi->systemVersion = 5;                                                                     \
        sapi->create = factoryname##_create;                                                         \
        sapi->destroy = factoryname##_destroy;                                                       \
        sapi->getVersion = factoryname##_getVersion;                                                 \
        sapi->getAbi = factoryname##_getAbi;                                                         \
        sapi->getClassName = factoryname##_getClassName;                                             \
        sapi->getBaseClassName = factoryname##_getBaseClassName;                                     \
        for (int i = 0; i < YARP_SHAREDLIBRARYCLASSAPI_PADDING; i++) {                               \
            sapi->roomToGrow[i] = 0;                                                                 \
        }                                                                                            \
        sapi->endCheck = yarp::os::createVocab32('P', 'L', 'U', 'G');                                  \
        return sapi->startCheck;                                                                     \
    }
// The double cast in the _create() and _destroy() functions are
// required to ensure that everything works when `basename` is not the
// first inherited class:
// _create() will return a valid `basename` or a null pointer if
// `classname` does not inherit from `basename`.
// _destroy() will ensure that we are calling `classname` destructor
// even if `basename` is not the first inherited class. If the
// dynamic_cast fails, it will not delete the object (that is probably
// leaked), but it is less dangerous than executing some other random
// function.

#define YARP_DEFAULT_FACTORY_NAME "yarp_default_factory"
#define YARP_DEFINE_DEFAULT_SHARED_CLASS(classname) YARP_DEFINE_SHARED_SUBCLASS(yarp_default_factory, classname, classname)
#define YARP_DEFINE_SHARED_CLASS(factoryname, classname) YARP_DEFINE_SHARED_SUBCLASS(factoryname, classname, classname)

#endif // YARP_OS_SHAREDLIBRARYCLASS_H

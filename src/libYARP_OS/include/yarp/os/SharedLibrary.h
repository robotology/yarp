/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_SHAREDLIBRARY_H
#define YARP_OS_SHAREDLIBRARY_H

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class SharedLibrary;
        class ConstString;
        namespace impl {
            class SharedLibraryImpl;
        }
    }
}

/**
 * Low-level wrapper for loading shared libraries (DLLs) and accessing
 * symbols within it.
 */
class YARP_OS_API yarp::os::SharedLibrary {
public:
    /**
     * Initialize, without opening a shared library yet.
     */
    SharedLibrary();

    /**
     * Load the named shared library / DLL.
     *
     * @param filename name of file (see open method)
     */
    SharedLibrary(const char *filename);

    /**
     * Destructor.  Will close() if needed.
     */
    virtual ~SharedLibrary();

    /**
     * Load the named shared library / DLL.  The library is found
     * using the algoithm of ACE::ldfind.  Operating-system-specific
     * extensions will be tried, and the relevant path for shared
     * libraries.
     *
     * @param filename name of file.
     * @return true on success
     */
    bool open(const char *filename);

    /**
     * Shared library no longer needed, unload if not in use elsewhere.
     * @return true on success
     */
    bool close();

    /**
     * Returns a human-readable string describing the most recent error that
     * occurred from a call to one of its functions.
     *
     * @return the most recent error
     */
    ConstString error();

    /**
     * Look up a symbol in the shared library.
     */
    void *getSymbol(const char *symbolName);

    /**
     * Check if the shared library is valid
     *
     * @return true iff a valid library has been loaded.
     */
    bool isValid() const;

private:
    yarp::os::impl::SharedLibraryImpl* const implementation;

    SharedLibrary(const SharedLibrary&); // Not implemented
    SharedLibrary& operator=(const SharedLibrary&); // Not implemented
};

#endif // YARP_OS_SHAREDLIBRARY_H

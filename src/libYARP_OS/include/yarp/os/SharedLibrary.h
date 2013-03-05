// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHAREDLIBRARY_
#define _YARP2_SHAREDLIBRARY_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class SharedLibrary;
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
    void *implementation;
};

#endif

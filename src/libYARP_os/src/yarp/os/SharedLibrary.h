/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SHAREDLIBRARY_H
#define YARP_OS_SHAREDLIBRARY_H

#include <yarp/os/api.h>

#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace yarp {
namespace os {
namespace impl {
class SharedLibraryImpl;
}
} // namespace os
} // namespace yarp
#endif // DOXYGEN_SHOULD_SKIP_THIS

namespace yarp {
namespace os {

/**
 * Low-level wrapper for loading shared libraries (DLLs) and accessing
 * symbols within it.
 */
class YARP_os_API SharedLibrary
{
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
    SharedLibrary(const char* filename);

    SharedLibrary(const SharedLibrary&) = delete;
    SharedLibrary& operator=(const SharedLibrary&) = delete;

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
    bool open(const char* filename);

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
    std::string error();

    /**
     * Look up a symbol in the shared library.
     */
    void* getSymbol(const char* symbolName);

    /**
     * Check if the shared library is valid
     *
     * @return true iff a valid library has been loaded.
     */
    bool isValid() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    yarp::os::impl::SharedLibraryImpl* const implementation;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SHAREDLIBRARY_H

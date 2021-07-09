/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SHAREDLIBRARYFACTORY_H
#define YARP_OS_SHAREDLIBRARYFACTORY_H

#include <yarp/os/api.h>

#include <yarp/os/SharedLibrary.h>
#include <yarp/os/SharedLibraryClassApi.h>
#include <yarp/os/Vocab.h>

#include <string>


namespace yarp {
namespace os {

/**
 * A wrapper for a named factory method in a named shared library.
 * This wrapper will do some basic checks that the named method does
 * indeed behave like a YARP plugin hook before offering access to it.
 * This is to avoid accidents, it is not a security mechanism.
 */
class YARP_os_API SharedLibraryFactory
{
public:
    /**
     * The status of a factory can be:
     *  - STATUS_NONE: Not configured yet
     *  - STATUS_OK: Present and sane
     *  - STATUS_LIBRARY_NOT_FOUND: Named shared library was not found
     *  - STATUS_LIBRARY_NOT_LOADED: Named shared library failed to load
     *  - STATUS_FACTORY_NOT_FOUND: Named method wasn't present in library
     *  - STATUS_FACTORY_NOT_FUNCTIONAL: Named method is not working right
     */
    enum
    {
        STATUS_NONE,                                                           //!< Not configured yet.
        STATUS_OK = yarp::os::createVocab32('o', 'k'),                           //!< Present and sane.
        STATUS_LIBRARY_NOT_FOUND = yarp::os::createVocab32('f', 'o', 'u', 'n'),  //!< Named shared library was not found.
        STATUS_LIBRARY_NOT_LOADED = yarp::os::createVocab32('l', 'o', 'a', 'd'), //!< Named shared library failed to load.
        STATUS_FACTORY_NOT_FOUND = yarp::os::createVocab32('f', 'a', 'c', 't'),  //!< Named method wasn't present in library.
        STATUS_FACTORY_NOT_FUNCTIONAL = yarp::os::createVocab32('r', 'u', 'n')   //!< Named method is not working right.
    };

    /**
     * Constructor for unconfigured factory.
     */
    explicit SharedLibraryFactory();

    /**
     * Constructor.
     *
     * @param dll_name name/path of shared library.
     * @param fn_name name of factory method, a symbol within the shared library.
     */
    SharedLibraryFactory(const char* dll_name,
                         const char* fn_name = nullptr);

    /**
     * Destructor
     */
    virtual ~SharedLibraryFactory();

    /**
     * Configure the factory.
     *
     * @param dll_name name/path of shared library.
     * @param fn_name name of factory method, a symbol within the shared library.
     * @return true on success.
     */
    bool open(const char* dll_name, const char* fn_name = nullptr);

    /**
     * Check if factory is configured and present.
     *
     * @return true iff factory is good to go.
     */
    bool isValid() const;

    /**
     * Get the status of the factory.
     *
     * @return one of the SharedLibraryFactory::STATUS_* codes.
     */
    int getStatus() const;

    /**
     * Get the latest error of the factory.
     *
     * @return the latest error.
     */
    std::string getError() const;

    /**
     * Get the factory API, which has creation/deletion methods.
     *
     * @return the factory API
     */
    const SharedLibraryClassApi& getApi() const;

    /**
     * Get the current reference count of this factory.
     *
     * @return the current reference count of this factory.
     */
    int getReferenceCount() const;

    /**
     * Increment the reference count of this factory.
     *
     * @return the current reference count of this factory, after increment.
     */
    int addRef();

    /**
     * Decrement the reference count of this factory.
     *
     * @return the current reference count of this factory, after decrement.
     */
    int removeRef();

    /**
     * Get the name associated with this factory.
     *
     * @return the name associated with this factory.
     */
    std::string getName() const;

    /**
     * Get the type associated with this factory.
     *
     * @return the type associated with this factory.
     */
    std::string getClassName() const;

    /**
     * Get the base type associated with this factory.
     *
     * @return the base type associated with this factory.
     */
    std::string getBaseClassName() const;

    /**
     *
     * Specify function to use as factory.
     *
     * @param factory function to use as factory.
     *
     * @result true on success.
     *
     */
    bool useFactoryFunction(void* factory);

private:
    SharedLibrary lib;
    int status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct; // FIXME Remove this reference counter and use a shared_ptr instead.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) className;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) baseClassName;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) error;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SHAREDLIBRARYFACTORY_H

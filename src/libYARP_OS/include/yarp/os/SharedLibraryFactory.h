// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPSHAREDLIBRARYFACTORY_
#define _YARP2_YARPSHAREDLIBRARYFACTORY_

#include <yarp/os/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/SharedLibrary.h>
#include <yarp/os/SharedLibraryClassApi.h>
#include <yarp/os/ConstString.h>


namespace yarp {
    namespace os {
        class SharedLibraryFactory;
    }
}

/**
 * A wrapper for a named factory method in a named shared library.
 * This wrapper will do some basic checks that the named method does
 * indeed behave like a YARP plugin hook before offering access to it.
 * This is to avoid accidents, it is not a security mechanism.
 */
class YARP_OS_API yarp::os::SharedLibraryFactory {
public:
    /**
     * The status of a factory can be:
     *  - STATUS_NONE: Not configured yet
     *  - STATUS_OK: Present and sane
     *  - STATUS_LIBRARY_NOT_LOADED: Named shared library failed to load
     *  - STATUS_FACTORY_NOT_FOUND: Named method wasn't present in library
     *  - STATUS_FACTORY_NOT_FUNCTIONAL: Named method is not working right
     */
    enum {
        STATUS_NONE,                                         //!< Not configured yet.
        STATUS_OK = VOCAB2('o','k'),                         //!< Present and sane.
        STATUS_LIBRARY_NOT_LOADED = VOCAB4('l','o','a','d'), //!< Named shared library failed to load.
        STATUS_FACTORY_NOT_FOUND = VOCAB4('f','a','c','t'),  //!< Named method wasn't present in library.
        STATUS_FACTORY_NOT_FUNCTIONAL = VOCAB3('r','u','n'), //!< Named method is not working right.
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
    SharedLibraryFactory(const char *dll_name,
                         const char *fn_name = NULL);

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
    bool open(const char *dll_name, const char *fn_name = NULL);

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
    ConstString getName() const;

private:
    SharedLibrary lib;
    int status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct;
    ConstString name;

    bool useFactoryFunction(void *factory);
};


#endif

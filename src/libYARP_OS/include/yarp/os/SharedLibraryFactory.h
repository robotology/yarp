// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPSHAREDLIBRARYFACTORY_
#define _YARP2_YARPSHAREDLIBRARYFACTORY_

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
    SharedLibraryFactory() {
        api.startCheck = 0;
        status = STATUS_NONE;
        rct = 0;
        returnValue = 0;
    }

    /**
     * Constructor.
     *
     * @param dll_name name/path of shared library.
     * @param fn_name name of factory method, a symbol within the shared library.
     */
    SharedLibraryFactory(const char *dll_name,
                         const char *fn_name = 0/*NULL*/) {
        rct = 0;
        returnValue = 0;
        open(dll_name,fn_name);
    }

    /**
     * Configure the factory.
     *
     * @param dll_name name/path of shared library.
     * @param fn_name name of factory method, a symbol within the shared library.
     * @return true on success.
     */
    bool open(const char *dll_name, const char *fn_name = 0/*NULL*/) {
        returnValue = 0;
        name = "";
        status = STATUS_NONE;
        api.startCheck = 0;
        if (!lib.open(dll_name)) {
            //fprintf(stderr,"Failed to open library %s\n", dll_name);
            status = STATUS_LIBRARY_NOT_LOADED;
            return false;
        }
        void *fn = lib.getSymbol((fn_name!=0/*NULL*/)?fn_name:YARP_DEFAULT_FACTORY_NAME);
        if (fn==0/*NULL*/) {
            lib.close();
            status = STATUS_FACTORY_NOT_FOUND;
            return false;
        }
        useFactoryFunction(fn);
        if (!isValid()) {
            status = STATUS_FACTORY_NOT_FUNCTIONAL;
            return false;
        }
        status = STATUS_OK;
        name = dll_name;
        return true;
    }

    /**
     * Check if factory is configured and present.
     *
     * @return true iff factory is good to go.
     */
    bool isValid() const {
        if (returnValue!=VOCAB4('Y','A','R','P')) return false;
        if (api.startCheck!=VOCAB4('Y','A','R','P')) return false;
        if (api.structureSize!=sizeof(SharedLibraryClassApi)) return false;
        if (api.systemVersion!=2) return false;
        if (api.endCheck!=VOCAB4('P','L','U','G')) return false;
        return true;
    }

    /**
     * Get the status of the factory.
     *
     * @return one of the SharedLibraryFactory::STATUS_* codes.
     */
    int getStatus() const {
        return status;
    }

    /**
     * Get the factory API, which has creation/deletion methods.
     *
     * @return the factory API
     */
    const SharedLibraryClassApi& getApi() const {
        return api;
    }

    /**
     * Get the current reference count of this factory.
     *
     * @return the current reference count of this factory.
     */
    int getReferenceCount() const {
        return rct;
    }

    /**
     * Increment the reference count of this factory.
     *
     * @return the current reference count of this factory, after increment.
     */
    int addRef() {
        rct++;
        return rct;
    }

    /**
     * Decrement the reference count of this factory.
     *
     * @return the current reference count of this factory, after decrement.
     */
    int removeRef() {
        rct--;
        return rct;
    }

    /**
     * Get the name associated with this factory.
     *
     * @return the name associated with this factory.
     */
    ConstString getName() const {
        return name;
    }

private:
    SharedLibrary lib;
    int status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct;
    ConstString name;

    bool useFactoryFunction(void *factory) {
        api.startCheck = 0;
        if (factory==0/*NULL*/) return false;
        isValid();
        returnValue =
            ((int (*)(void *ptr,int len)) factory)(&api,sizeof(SharedLibraryClassApi));
        return isValid();
    }
};


#endif

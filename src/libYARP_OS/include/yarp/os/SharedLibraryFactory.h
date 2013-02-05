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

        extern "C" {
            typedef int (*SharedClassFactoryFunction)(void *ptr,int len);
            typedef void (*SharedClassDeleterFunction)(void *ptr);
        }
    }
}

class YARP_OS_API yarp::os::SharedLibraryFactory {
private:
    SharedLibrary lib;
    int status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct;
    ConstString name;
public:
    enum {
        STATUS_NONE,
        STATUS_OK = VOCAB2('o','k'),
        STATUS_LIBRARY_NOT_LOADED = VOCAB4('l','o','a','d'),
        STATUS_FACTORY_NOT_FOUND = VOCAB4('f','a','c','t'),
        STATUS_FACTORY_NOT_FUNCTIONAL = VOCAB3('r','u','n'),
    };

    SharedLibraryFactory() {
        api.startCheck = 0;
        status = STATUS_NONE;
        rct = 0;
        returnValue = 0;
	}

    SharedLibraryFactory(const char *dll_name, const char *fn_name = 0/*NULL*/) {
        rct = 0;
        returnValue = 0;
		open(dll_name,fn_name);
	}

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

    bool useFactoryFunction(void *factory) {
        api.startCheck = 0;
        if (factory==0/*NULL*/) return false;
        SharedClassFactoryFunction fn = (SharedClassFactoryFunction)factory;
        isValid();
        returnValue = fn(&api,sizeof(SharedLibraryClassApi));
        return isValid();
    }

	bool isValid() {
        if (returnValue!=VOCAB4('Y','A','R','P')) return false;
	    if (api.startCheck!=VOCAB4('Y','A','R','P')) return false;
	    if (api.structureSize!=sizeof(SharedLibraryClassApi)) return false;
	    if (api.systemVersion!=2) return false;
        if (api.endCheck!=VOCAB4('P','L','U','G')) return false;
        return true;
    }

    int getStatus() {
        return status;
    }

    void *getDestroyFn() {
        if (!isValid()) return 0/*NULL*/;
        return (void *)api.destroy;
    }

    const SharedLibraryClassApi& getApi() const {
        return api;
    }

    int getReferenceCount() const {
        return rct;
    }

    int addRef() {
        rct++;
        return rct;
    }

    int removeRef() {
        rct--;
        return rct;
    }

    ConstString getName() const {
        return name;
    }
};


#endif

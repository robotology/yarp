// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/SharedLibraryFactory.h>
#include <yarp/os/ConstString.h>


yarp::os::SharedLibraryFactory::SharedLibraryFactory() :
        status(STATUS_NONE),
        returnValue(0),
        rct(1) {
}

yarp::os::SharedLibraryFactory::SharedLibraryFactory(const char *dll_name,
                                                     const char *fn_name) :
        status(STATUS_NONE),
        returnValue(0),
        rct(1) {
    open(dll_name,fn_name);
}

yarp::os::SharedLibraryFactory::~SharedLibraryFactory() {
}

bool yarp::os::SharedLibraryFactory::open(const char *dll_name, const char *fn_name) {
    returnValue = 0;
    name = "";
    className = "";
    baseClassName = "";
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

    char buf[256];
    api.getClassName(buf, 256);
    className = buf;
    api.getBaseClassName(buf, 256);
    baseClassName = buf;

    return true;
}

bool yarp::os::SharedLibraryFactory::isValid() const {
    if (returnValue != VOCAB4('Y','A','R','P')) {
        return false;
    }
    if (api.startCheck != VOCAB4('Y','A','R','P')) {
        return false;
    }
    if (api.structureSize != sizeof(SharedLibraryClassApi)) {
        return false;
    }
    if (api.systemVersion != 4) {
        return false;
    }
    if (api.endCheck != VOCAB4('P','L','U','G')) {
        return false;
    }
    return true;
}

int yarp::os::SharedLibraryFactory::getStatus() const {
    return status;
}
const yarp::os::SharedLibraryClassApi& yarp::os::SharedLibraryFactory::getApi() const {
    return api;
}

int yarp::os::SharedLibraryFactory::getReferenceCount() const {
    return rct;
}


int yarp::os::SharedLibraryFactory::addRef() {
    rct++;
    return rct;
}

int yarp::os::SharedLibraryFactory::removeRef() {
    rct--;
    return rct;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getName() const {
    return name;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getClassName() const {
    return className;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getBaseClassName() const {
    return baseClassName;
}

bool yarp::os::SharedLibraryFactory::useFactoryFunction(void *factory) {
    api.startCheck = 0;
    if (factory == NULL) {
        return false;
    }
    returnValue =
        ((int (*)(void *ptr,int len)) factory)(&api,sizeof(SharedLibraryClassApi));
    return isValid();
}

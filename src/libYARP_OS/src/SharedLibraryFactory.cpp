/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/SharedLibraryFactory.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Os.h>

yarp::os::SharedLibraryFactory::SharedLibraryFactory() :
        status(STATUS_NONE),
        returnValue(0),
        rct(1)
{
    memset(&api, 0, sizeof(SharedLibraryClassApi));
}

yarp::os::SharedLibraryFactory::SharedLibraryFactory(const char *dll_name,
                                                     const char *fn_name) :
        status(STATUS_NONE),
        returnValue(0),
        rct(1)
{
    open(dll_name, fn_name);
}

yarp::os::SharedLibraryFactory::~SharedLibraryFactory()
{
}

bool yarp::os::SharedLibraryFactory::open(const char *dll_name, const char *fn_name)
{
    returnValue = 0;
    name = "";
    className = "";
    baseClassName = "";
    status = STATUS_NONE;
    error = "";
    api.startCheck = 0;
    if (!lib.open(dll_name)) {
        if (yarp::os::stat(dll_name) != 0) {
            status = STATUS_LIBRARY_NOT_FOUND;
        } else {
            status = STATUS_LIBRARY_NOT_LOADED;
        }
        error = lib.error();
        return false;
    }
    void *fn = lib.getSymbol((fn_name != nullptr) ? fn_name : YARP_DEFAULT_FACTORY_NAME);
    if (fn == nullptr) {
        status = STATUS_FACTORY_NOT_FOUND;
        error = lib.error();
        lib.close();
        return false;
    }
    if (!useFactoryFunction(fn)) {
        status = STATUS_FACTORY_NOT_FUNCTIONAL;
        error = "YARP hook in shared library misbehaved";
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

bool yarp::os::SharedLibraryFactory::isValid() const
{
    if (returnValue != VOCAB4('Y', 'A', 'R', 'P')) {
        return false;
    }
    if (api.startCheck != VOCAB4('Y', 'A', 'R', 'P')) {
        return false;
    }
    if (api.structureSize != sizeof(SharedLibraryClassApi)) {
        return false;
    }
    if (api.systemVersion != 5) {
        return false;
    }
    if (api.endCheck != VOCAB4('P', 'L', 'U', 'G')) {
        return false;
    }
    return true;
}

int yarp::os::SharedLibraryFactory::getStatus() const
{
    return status;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getError() const
{
    return error;
}

const yarp::os::SharedLibraryClassApi& yarp::os::SharedLibraryFactory::getApi() const
{
    return api;
}

int yarp::os::SharedLibraryFactory::getReferenceCount() const
{
    return rct;
}


int yarp::os::SharedLibraryFactory::addRef()
{
    rct++;
    return rct;
}

int yarp::os::SharedLibraryFactory::removeRef()
{
    rct--;
    return rct;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getName() const
{
    return name;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getClassName() const
{
    return className;
}

yarp::os::ConstString yarp::os::SharedLibraryFactory::getBaseClassName() const
{
    return baseClassName;
}

bool yarp::os::SharedLibraryFactory::useFactoryFunction(void *factory)
{
    api.startCheck = 0;
    if (factory == nullptr) {
        return false;
    }
    returnValue =
        ((int (*)(void *ptr, int len)) factory)(&api, sizeof(SharedLibraryClassApi));
    return isValid();
}

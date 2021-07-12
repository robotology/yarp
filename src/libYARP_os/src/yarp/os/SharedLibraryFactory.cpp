/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SharedLibraryFactory.h>

#include <yarp/os/Os.h>

#include <string>

yarp::os::SharedLibraryFactory::SharedLibraryFactory() :
        status(STATUS_NONE),
        returnValue(0),
        rct(1)
{
    memset(&api, 0, sizeof(SharedLibraryClassApi));
}

yarp::os::SharedLibraryFactory::SharedLibraryFactory(const char* dll_name,
                                                     const char* fn_name) :
        status(STATUS_NONE),
        returnValue(0),
        rct(1)
{
    open(dll_name, fn_name);
}

yarp::os::SharedLibraryFactory::~SharedLibraryFactory() = default;

bool yarp::os::SharedLibraryFactory::open(const char* dll_name, const char* fn_name)
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
    void* fn = lib.getSymbol((fn_name != nullptr) ? fn_name : YARP_DEFAULT_FACTORY_NAME);
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
    if (returnValue != yarp::os::createVocab32('Y', 'A', 'R', 'P')) {
        return false;
    }
    if (api.startCheck != yarp::os::createVocab32('Y', 'A', 'R', 'P')) {
        return false;
    }
    if (api.structureSize != sizeof(SharedLibraryClassApi)) {
        return false;
    }
    if (api.systemVersion != 5) {
        return false;
    }
    if (api.endCheck != yarp::os::createVocab32('P', 'L', 'U', 'G')) {
        return false;
    }
    return true;
}

int yarp::os::SharedLibraryFactory::getStatus() const
{
    return status;
}

std::string yarp::os::SharedLibraryFactory::getError() const
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

std::string yarp::os::SharedLibraryFactory::getName() const
{
    return name;
}

std::string yarp::os::SharedLibraryFactory::getClassName() const
{
    return className;
}

std::string yarp::os::SharedLibraryFactory::getBaseClassName() const
{
    return baseClassName;
}

bool yarp::os::SharedLibraryFactory::useFactoryFunction(void* factory)
{
    api.startCheck = 0;
    if (factory == nullptr) {
        return false;
    }
    returnValue = ((int (*)(void* ptr, int len))factory)(&api, sizeof(SharedLibraryClassApi));
    return isValid();
}

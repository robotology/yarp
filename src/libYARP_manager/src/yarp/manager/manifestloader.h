/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_MANIFESTLOADER
#define YARP_MANAGER_MANIFESTLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/module.h>
#include <yarp/manager/application.h>
#include <yarp/manager/resource.h>

namespace yarp {
namespace manager {


/**
 * Abstract Class AppLoader
 */
class AppLoader {

public:
    AppLoader(void) {}
    virtual ~AppLoader() {}
    virtual bool init(void) = 0;
    virtual void fini(void) = 0;
    virtual void reset(void) = 0;
    virtual Application* getNextApplication(void) = 0;

protected:

private:

};

class AppSaver
{
public:
    AppSaver() {}
    virtual ~AppSaver() {}
    virtual bool save(Application* application) = 0;

protected:

private:
};



/**
 * Abstract Class ModuleLoader
 */
class ModuleLoader {

public:
    ModuleLoader(void) {}
    virtual ~ModuleLoader() {}
    virtual bool init(void) = 0;
    virtual void reset(void) = 0;
    virtual void fini(void) = 0;
    virtual Module* getNextModule(void) = 0;

protected:

private:

};


/**
 * Abstract Class ResourceLoader
 */
class ResourceLoader {

public:
    ResourceLoader(void) {}
    virtual ~ResourceLoader() {}
    virtual bool init(void) = 0;
    virtual void reset(void) = 0;
    virtual void fini(void) = 0;
    virtual GenericResource* getNextResource(void) = 0;

protected:

private:

};


/**
 * Abstract Class TempLoader
 */

struct AppTemplate {
    std::string name;
    std::string tmpFileName;
};


class TempLoader {
public:
    TempLoader(void) {}
    virtual ~TempLoader() {}
    virtual bool init(void) = 0;
    virtual void fini(void) = 0;
    virtual void reset(void) = 0;
    virtual AppTemplate* getNextAppTemplate(void) = 0;

protected:

private:

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_MANIFESTLOADER__

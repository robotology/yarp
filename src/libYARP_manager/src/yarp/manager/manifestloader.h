/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    AppLoader() = default;
    virtual ~AppLoader() = default;
    virtual bool init() = 0;
    virtual void fini() = 0;
    virtual void reset() = 0;
    virtual Application* getNextApplication() = 0;

protected:

private:

};

class AppSaver
{
public:
    AppSaver() = default;
    virtual ~AppSaver() = default;
    virtual bool save(Application* application) = 0;

protected:

private:
};



/**
 * Abstract Class ModuleLoader
 */
class ModuleLoader {

public:
    ModuleLoader() = default;
    virtual ~ModuleLoader() = default;
    virtual bool init() = 0;
    virtual void reset() = 0;
    virtual void fini() = 0;
    virtual Module* getNextModule() = 0;

protected:

private:

};


/**
 * Abstract Class ResourceLoader
 */
class ResourceLoader {

public:
    ResourceLoader() = default;
    virtual ~ResourceLoader() = default;
    virtual bool init() = 0;
    virtual void reset() = 0;
    virtual void fini() = 0;
    virtual GenericResource* getNextResource() = 0;

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
    TempLoader() = default;
    virtual ~TempLoader() = default;
    virtual bool init() = 0;
    virtual void fini() = 0;
    virtual void reset() = 0;
    virtual AppTemplate* getNextAppTemplate() = 0;

protected:

private:

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_MANIFESTLOADER__

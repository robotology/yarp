// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARP_MANAGER_MANIFESTLOADER__
#define __YARP_MANAGER_MANIFESTLOADER__

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

typedef struct _AppTemplate {
    string name;
    string tmpFileName;
} AppTemplate;


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

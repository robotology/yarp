// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_MANAGER_SINGLEAPPLOADER
#define YARP_MANAGER_SINGLEAPPLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/manifestloader.h>

namespace yarp {
namespace manager {


/**
 * Class SingleAppLoader
 */
class SingleAppLoader : public AppLoader {

public:
    SingleAppLoader(const char* szModule, const char* strHost);
    virtual ~SingleAppLoader();
    virtual bool init(void);
    virtual void fini(void);
    virtual Application* getNextApplication(void);

protected:

private:
    Application app;
    string strModule;
    string strHost;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_SINGLEAPPLOADER__

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    virtual bool init(void) override;
    virtual void fini(void) override;
    virtual Application* getNextApplication(void) override;

protected:

private:
    Application app;
    std::string strModule;
    std::string strHost;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_SINGLEAPPLOADER__

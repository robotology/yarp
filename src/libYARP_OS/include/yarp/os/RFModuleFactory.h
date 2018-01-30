/*
* Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
* All rights reserved.
*
* This software may be modified and distributed under the terms of the
* BSD-3-Clause license. See the accompanying LICENSE file for details.
*/


#include <yarp/os/RFModule.h>
#include <string>

namespace yarp {
namespace os {

class YARP_OS_API RFPlugin
{
    struct RFPlugin_Private;
    RFPlugin_Private* impl{nullptr};

public:
    RFPlugin();

    virtual ~RFPlugin();

    virtual bool open(const std::string& command);

    virtual void close();

    virtual bool isRunning();

    virtual std::string getCmd();

    virtual std::string getAlias();

    virtual int getThreadKey();
};

class YARP_OS_API RFModuleFactory
{
private:
    struct Private;
    Private* impl;
    RFModuleFactory();
public:

    static RFModuleFactory& GetInstance();
    static void AddModule(const std::string& name, RFModule*(*moduleCreate)(void));
    RFModule* GetModule(const std::string name);
};

}
}

#endif // YARP_OS_RFPLUGIN_H

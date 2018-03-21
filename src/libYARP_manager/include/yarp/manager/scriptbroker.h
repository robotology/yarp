/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_SCRIPT_BROKER
#define YARP_MANAGER_SCRIPT_BROKER

#include <string>

#include <yarp/os/Log.h>

#include <yarp/manager/yarpbroker.h>
#include <yarp/manager/localbroker.h>

namespace yarp {
namespace manager {

class ScriptLocalBroker: public LocalBroker
{

public:
     ScriptLocalBroker(const char* sc) : LocalBroker() {
        yAssert(sc);
        script = sc;
     }
     virtual ~ScriptLocalBroker() {}
     using LocalBroker::init;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv) override;
private:
    std::string script;
};


class ScriptYarprunBroker: public YarpBroker
{

public:
    ScriptYarprunBroker(const char* sc) : YarpBroker() {
        yAssert(sc);
        script = sc;
    }
     virtual ~ScriptYarprunBroker() {}
     using YarpBroker::init;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv) override;
private:
    bool whichFile(const char* server, const char* filename, std::string& filenameWithPath);
    std::string script;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_SCRIPT_BROKER__

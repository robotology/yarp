/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_YARPDEVBROKER
#define YARP_MANAGER_YARPDEVBROKER

#include <string>

#include <yarp/manager/yarpbroker.h>
#include <yarp/manager/localbroker.h>


namespace yarp {
namespace manager {

class YarpdevLocalBroker: public LocalBroker
{

public:
    YarpdevLocalBroker() : LocalBroker() {}
     virtual ~YarpdevLocalBroker() {}
     using yarp::manager::LocalBroker::init;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv) override {

            OSTRINGSTREAM strDevParam;
            std::string strParam;
            std::string strCmd;
            if (szcmd) {
                strCmd = szcmd;
            }
            if (szparam) {
                strParam = szparam;
            }
            strDevParam<<"--device "<<strCmd<<" "<<strParam;
            return LocalBroker::init("yarpdev", strDevParam.str().c_str(),
                                     szhost, szstdio, szworkdir, szenv);
     }
};


class YarpdevYarprunBroker: public YarpBroker
{

public:
    YarpdevYarprunBroker() : YarpBroker() {}
     virtual ~YarpdevYarprunBroker() {}
     using yarp::manager::YarpBroker::init;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv) override {

            OSTRINGSTREAM strDevParam;
            std::string strParam;
            std::string strCmd;
            if (szcmd) {
                strCmd = szcmd;
            }
            if (szparam) {
                strParam = szparam;
            }
            strDevParam<<"--device "<<strCmd<<" "<<strParam;
            return YarpBroker::init("yarpdev", strDevParam.str().c_str(),
                                     szhost, szstdio, szworkdir, szenv);
     }
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_YARPDEVBROKER__

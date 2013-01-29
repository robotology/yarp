// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __YARPDEVBROKER__
#define __YARPDEVBROKER__

#include "yarpbroker.h" 
#include "localbroker.h"

#include <string>

class YarpdevLocalBroker: public LocalBroker
{

public: 
    YarpdevLocalBroker() : LocalBroker() {}
     virtual ~YarpdevLocalBroker() {}
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) {

            OSTRINGSTREAM strDevParam;
            std::string strParam;
            std::string strCmd;
            if(szcmd) strCmd = szcmd;
            if(szparam) strParam = szparam;
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
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) {

            OSTRINGSTREAM strDevParam;
            std::string strParam;
            std::string strCmd;
            if(szcmd) strCmd = szcmd;
            if(szparam) strParam = szparam;
            strDevParam<<"--device "<<strCmd<<" "<<strParam;
            return YarpBroker::init("yarpdev", strDevParam.str().c_str(), 
                                     szhost, szstdio, szworkdir, szenv);
     }
};
 

#endif //__YARPDEVBROKER__

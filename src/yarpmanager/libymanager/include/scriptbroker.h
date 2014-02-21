// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>, Elena Ceseracciu <elena.ceseracciu@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __SCRIPT_BROKER__
#define __SCRIPT_BROKER__

#include "yarpbroker.h" 
#include "localbroker.h"
#include "yarp/os/Log.h"
#include <string>

class ScriptLocalBroker: public LocalBroker
{

public: 
     ScriptLocalBroker(const char* sc) : LocalBroker() {
        YARP_ASSERT(sc);
        script = sc;
     }
     virtual ~ScriptLocalBroker() {}
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv );
private:
    std::string script;
};
 

class ScriptYarprunBroker: public YarpBroker
{

public: 
    ScriptYarprunBroker(const char* sc) : YarpBroker() {
        YARP_ASSERT(sc);
        script = sc;
    }
     virtual ~ScriptYarprunBroker() {}
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv );
private:
    bool whichFile(const char* server, const char* filename, std::string& filenameWithPath);
    std::string script;
};
 
#endif //__SCRIPT_BROKER__



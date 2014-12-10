// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>, Elena Ceseracciu <elena.ceseracciu@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/scriptbroker.h>

#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ConstString.h>

#define CONNECTION_TIMEOUT      5.0         //seconds


using namespace yarp::os;
using namespace yarp::manager;

static char slash = NetworkBase::getDirectorySeparator()[0];

////// adapted from libYARP_OS: ResourceFinder.cpp
static Bottle parsePaths(const ConstString& txt) {
    char slash = NetworkBase::getDirectorySeparator()[0];
    char sep = NetworkBase::getPathSeparator()[0];
    Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (ConstString::size_type i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch==sep) {
            result.addString(ConstString(at,len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(ConstString(at,len-slash_tweak));
    }
    return result;
}

static bool fileExists(const char *fname) {
        FILE *fp=NULL;
        fp = fopen(fname,"r");
        if(fp == NULL)
            return false;
        else
        {
            fclose(fp);
            return true;
        }
    }


bool ScriptLocalBroker::init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv )
{
    OSTRINGSTREAM strDevParam;
    std::string strParam;
    std::string strCmd="";
    if(szcmd)
    {
        yarp::os::Bottle possiblePaths = parsePaths(yarp::os::NetworkBase::getEnvironment("PATH"));
        for (int i=0; i<possiblePaths.size(); ++i)
        {
            ConstString guessString=possiblePaths.get(i).asString() + slash + szcmd;
            const char* guess=guessString.c_str();
            if (fileExists (guess))
            {
#ifdef WIN32
                strCmd = "\"" + std::string(guess) + "\"";
#else
                strCmd = guess;
#endif
                break;
            }
        }

    }
    if(strCmd=="")
        return false;
    if(szparam) strParam = szparam;
    strDevParam<<strCmd<<" "<<strParam;
    return LocalBroker::init(script.c_str(), strDevParam.str().c_str(),
                                szhost, szstdio, szworkdir, szenv);
     }


bool ScriptYarprunBroker::whichFile(const char* server, const char* filename, std::string& filenameWithPath)
{
    if(!strlen(server))
        return false;

    yarp::os::Bottle msg, grp;
    grp.clear();
    grp.addString("which");
    grp.addString(filename);
    msg.addList() = grp;

    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    //style.carrier = carrier;
    yarp::os::Port port;
    port.open("...");

    bool connected = yarp::os::NetworkBase::connect(port.getName(), server, style);
    if(!connected)
    {
        return false;
    }

    yarp::os::Value filenameReader;
    bool ret = port.write(msg, filenameReader);
    filenameWithPath=filenameReader.asString();
    NetworkBase::disconnect(port.getName().c_str(), server);
    port.close();

    if(!ret)
    {
        return false;
    }
    return true;
}

bool ScriptYarprunBroker::init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv )
{

    OSTRINGSTREAM strDevParam;
    std::string strParam;
    std::string strCmd;
    if(szcmd)
    {
        std::string strHost;
        if(szhost[0] != '/')
            strHost = string("/") + string(szhost);
        else
            strHost = szhost;
        whichFile(strHost.c_str(), szcmd, strCmd);
    }
    if(szparam) strParam = szparam;
    strDevParam<<strCmd<<" "<<strParam;
    return YarpBroker::init(script.c_str(), strDevParam.str().c_str(),
                                szhost, szstdio, szworkdir, szenv);
}

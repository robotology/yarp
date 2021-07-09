/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/scriptbroker.h>

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>

#include <string>

#define CONNECTION_TIMEOUT      5.0         //seconds


using namespace yarp::os;
using namespace yarp::manager;
using namespace std;
namespace fs = yarp::conf::filesystem;

constexpr fs::value_type slash = fs::preferred_separator;
constexpr auto sep = yarp::conf::environment::path_separator;

////// adapted from YARP_os: ResourceFinder.cpp
static Bottle parsePaths(const std::string& txt) {
    Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (char ch : txt) {
        if (ch==sep) {
            result.addString(std::string(at,len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(std::string(at,len-slash_tweak));
    }
    return result;
}

static bool fileExists(const char *fname) {
        FILE *fp=nullptr;
        fp = fopen(fname,"r");
        if(fp == nullptr)
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
    std::string strCmd;
    if(szcmd)
    {
        yarp::os::Bottle possiblePaths = parsePaths(yarp::conf::environment::get_string("PATH"));
        for (size_t i=0; i<possiblePaths.size(); ++i)
        {
            std::string guessString=possiblePaths.get(i).asString() +
            std::string{slash} + szcmd;
            const char* guess=guessString.c_str();
            if (fileExists (guess))
            {
#if defined(_WIN32)
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
    NetworkBase::disconnect(port.getName(), server);
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

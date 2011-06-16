// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007-2009 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <stdio.h>
#include <signal.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/RunReadWrite.h>

#ifndef YARP_HAS_ACE
#ifndef __APPLE__
#include <wait.h>
#else
#include <sys/wait.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

#if defined(WIN32) || defined(WIN64)
static void sigbreakHandler(int sig)
{
    raise(SIGINT);
}
#endif

/////////////////////////////////////

static void wSigintHandler(int sig);

int RunWrite::loop(const char* wPortName)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,wSigintHandler);
    signal(SIGTERM,wSigintHandler);

#if defined(WIN32) || defined(WIN64)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    if (!mWPort.open(wPortName))
    {
        return 1;
    }

    mWPortName=wPortName;

    while (!feof(stdin)) 
    {
        std::string txt=getStdin();

        if (!feof(stdin)) 
        {
            if (txt[0]>=32 || txt[0]=='\n' || txt[0]=='\r' || txt[0]=='\t' || txt[0]=='\0') 
            {
                yarp::os::Bottle bot;
                bot.fromString(txt.c_str());
                mWPort.write(bot);
            }
            else
            {
                break;
            }
        }
    }

    mWPort.interrupt();
    mWPort.close();
    yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

    return 0;
}

void RunWrite::close()
{
    mWPort.interrupt();
    mWPort.close();
    yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

    exit(0);
}

std::string RunWrite::getStdin() 
{
    bool done=false;
    std::string txt="";
    char buf[2048];

    while (!done) 
    {
        char *result=ACE_OS::fgets(buf,sizeof(buf),stdin);
        if (result!=NULL) 
        {
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 

            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
            txt+=buf;
        } 
        else 
        {
            done=true;
        }
    }
    return txt;
}

yarp::os::Port RunWrite::mWPort;
yarp::os::ConstString RunWrite::mWPortName;

static void wSigintHandler(int sig)
{
    RunWrite::close();
}

///////////////////////////////////////////

static void rSigintHandler(int sig);

int RunRead::loop(const char* rPortName)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rSigintHandler);
    signal(SIGTERM,rSigintHandler);

#if defined(WIN32) || defined(WIN64)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    mRPort.setReader(*this);

    if (!mRPort.open(rPortName))
    {
        return 1;
    }

    mRPortName=rPortName;

    mDone.wait();

    mRPort.interrupt();
    mRPort.close();
    yarp::os::NetworkBase::unregisterName(mRPortName.c_str());

    return 0;
}

bool RunRead::read(yarp::os::ConnectionReader& reader) 
{
    if (!reader.isValid()) return false;

    yarp::os::Bottle bot;

    if (bot.read(reader))
    {
        if (bot.size()==2 && bot.get(0).isInt() && bot.get(1).isString()) 
        {
            if (bot.get(0).asInt()!=1) 
            {
                ACE_OS::printf("%s\n",bot.get(1).asString().c_str());
                ACE_OS::fflush(stdout);
            }
        } 
        else
        {
            ACE_OS::printf("%s\n", bot.toString().c_str());
            ACE_OS::fflush(stdout);
        }
        return true;
    }

    return false;
}

std::string RunRead::getStdin() 
{
    bool done=false;
    std::string txt="";
    char buf[2048];

    while (!done) 
    {
        char *result=ACE_OS::fgets(buf,sizeof(buf),stdin);
        if (result!=NULL) 
        {
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 

            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
            txt+=buf;
        } 
        else 
        {
            done=true;
        }
    }
    return txt;
}

yarp::os::Semaphore RunRead::mDone(0);
yarp::os::Port RunRead::mRPort;
yarp::os::ConstString RunRead::mRPortName;

static void rSigintHandler(int sig)
{
    RunRead::close();
}

///////////////////////////////////////////

static void rwSigintHandler(int sig);

#if !defined(WIN32) && !defined(WIN64)
static void sighupHandler(int sig);
#endif

int RunReadWrite::loop(const char* rPortName,const char* wPortName)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rwSigintHandler);
    signal(SIGTERM,rwSigintHandler);

#if defined(WIN32) || defined(WIN64)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,sighupHandler);
#endif

    mRPort.setReader(*this);

    if (!mRPort.open(rPortName))
    {
        return 1;
    }

    if (!mWPort.open(wPortName))
    {
        mRPort.close();
        return 1;
    }

    mRPortName=rPortName;
    mWPortName=wPortName;

    while (!feof(stdin)) 
    {
        std::string txt=getStdin();

        if (!feof(stdin)) 
        {
            if (txt[0]>=32 || txt[0]=='\n' || txt[0]=='\r' || txt[0]=='\0' || txt[0]=='\t') 
            {
                yarp::os::Bottle bot;
                bot.fromString(txt.c_str());
                mWPort.write(bot);
            }
            else
            {
                break;
            }
        }
    }

#if !defined(WIN32) && !defined(WIN64)
    mDone.wait();        
    if (!mClosed) 
    {
        mClosed=true;
#endif            

        mRPort.interrupt();
        mRPort.close();
        yarp::os::NetworkBase::unregisterName(mRPortName.c_str());

        mWPort.interrupt();
        mWPort.close();
        yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

#if !defined(WIN32) && !defined(WIN64)
    }
    mDone.post();
#endif

    return 0;
}

void RunReadWrite::close()
{
#if !defined(WIN32) && !defined(WIN64)
    mDone.wait();        
    if (!mClosed) 
    {
        mClosed=true;
#endif  

        mRPort.interrupt();
        mRPort.close();
        yarp::os::NetworkBase::unregisterName(mRPortName.c_str());

        mWPort.interrupt();
        mWPort.close();
        yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

#if !defined(WIN32) && !defined(WIN64)
    }
    mDone.post();
#endif

    exit(0);
}

bool RunReadWrite::read(yarp::os::ConnectionReader& reader) 
{
    if (!reader.isValid()) return false;

    yarp::os::Bottle bot;

    if (bot.read(reader))
    {
        if (bot.size()==2 && bot.get(0).isInt() && bot.get(1).isString()) 
        {
            if (bot.get(0).asInt()!=1) 
            {
                ACE_OS::printf("%s\n",bot.get(1).asString().c_str());
                ACE_OS::fflush(stdout);
            }
        } 
        else
        {
            ACE_OS::printf("%s\n", bot.toString().c_str());
            ACE_OS::fflush(stdout);
        }
        return true;
    }

    return false;
}

std::string RunReadWrite::getStdin() 
{
    bool done=false;
    std::string txt="";
    char buf[2048];

    while (!done) 
    {
        char *result=ACE_OS::fgets(buf,sizeof(buf),stdin);
        if (result!=NULL) 
        {
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 

            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
            txt+=buf;
        } 
        else 
        {
            done=true;
        }
    }
    return txt;
}

bool RunReadWrite::mClosed=false;
yarp::os::Semaphore RunReadWrite::mDone(1);
yarp::os::Port RunReadWrite::mRPort;
yarp::os::Port RunReadWrite::mWPort;
yarp::os::ConstString RunReadWrite::mRPortName;
yarp::os::ConstString RunReadWrite::mWPortName;

static void rwSigintHandler(int sig)
{
    RunReadWrite::close();
}

#if !defined(WIN32) && !defined(WIN64)
static void sighupHandler(int sig)
{
    RunReadWrite::close();
}
#endif

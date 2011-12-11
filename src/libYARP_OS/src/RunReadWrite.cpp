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

#if defined(WIN32)
static void sigbreakHandler(int sig)
{
    raise(SIGINT);
}
#endif

/////////////////////////////////////

static void wSigintHandler(int sig);

int RunWrite::loop(yarp::os::ConstString& uuid)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,wSigintHandler);
    signal(SIGTERM,wSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    mWPortName=uuid+"/stdout";

    if (!mWPort.open(mWPortName.c_str()))
    {
        return 1;
    }

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect(mWPortName.c_str(),(TOPIC+uuid+"/topic_i").c_str());

    while (!feof(stdin)) 
    {
        std::string txt=getStdin();

        if (!feof(stdin)) 
        {
            if (txt[0]>=32 || txt[0]=='\n' || txt[0]=='\r' || txt[0]=='\t' || txt[0]=='\0') 
            {
                yarp::os::Bottle bot;
                bot.addString(txt.c_str());
                //bot.fromString(txt.c_str());
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
#if 0
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 

            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
#endif
            txt+=buf;
            done=true;
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

int RunRead::loop(yarp::os::ConstString& uuid)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rSigintHandler);
    signal(SIGTERM,rSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    mRPortName=uuid+"/stdin";
    mRPort.setReader(*this);

    if (!mRPort.open(mRPortName.c_str()))
    {
        return 1;
    }

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect((TOPIC+uuid+"/topic_o").c_str(),mRPortName.c_str());

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
        if (bot.size()==1)
        {
            ACE_OS::printf("%s",bot.get(0).asString().c_str());
            ACE_OS::fflush(stdout);
            return true;
        }

        if (bot.size()==2 && bot.get(0).isInt() && bot.get(1).isString()) 
        {
            if (bot.get(0).asInt()!=1) 
            {
                ACE_OS::printf("%s",bot.get(1).asString().c_str());
                ACE_OS::fflush(stdout);
            }
        } 
        else
        {
            //ACE_OS::printf("%s\n", bot.toString().c_str());
            ACE_OS::printf("%s",bot.get(0).asString().c_str());
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
#if 0
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 

            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
#endif
            txt+=buf;
            done=true;
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

#if !defined(WIN32)
static void sighupHandler(int sig);
#endif

int RunReadWrite::loop(yarp::os::ConstString &uuid)
{
    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rwSigintHandler);
    signal(SIGTERM,rwSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,sighupHandler);
#endif

    mUUID=uuid;
    mWPortName=uuid+"/stdio:o";
    mRPortName=uuid+"/stdio:i";

    mRPort.setReader(*this);

    if (!mRPort.open(mRPortName.c_str()))
    {
        return 1;
    }

    if (!mWPort.open(mWPortName.c_str()))
    {
        mRPort.close();
        yarp::os::NetworkBase::unregisterName(mRPortName.c_str());
        return 1;
    }

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect(mWPortName.c_str(),(TOPIC+uuid+"/topic_o").c_str());
    yarp::os::Network::connect((TOPIC+uuid+"/topic_i").c_str(),mRPortName.c_str());

    while (!feof(stdin)) 
    {
        std::string txt=getStdin();

        if (!feof(stdin)) 
        {
            if (txt[0]>=32 || txt[0]=='\n' || txt[0]=='\r' || txt[0]=='\0' || txt[0]=='\t') 
            {
                yarp::os::Bottle bot;
                //bot.fromString(txt.c_str());
                bot.addString(txt.c_str());
                mWPort.write(bot);
            }
            else
            {
                break;
            }
        }
    }

/*
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
*/
    close();

    return 0;
}

void RunReadWrite::close()
{
#if !defined(WIN32)
    mDone.wait();        
    if (!mClosed) 
    {
        mClosed=true;
#endif  

        static const yarp::os::ConstString TOPIC("topic:/");
        yarp::os::Network::disconnect((mUUID+"/stdout").c_str(),(TOPIC+mUUID+"/topic_i").c_str());
        yarp::os::Network::disconnect((TOPIC+mUUID+"/topic_i").c_str(),(mUUID+"/stdio:i").c_str());
        yarp::os::Network::disconnect((mUUID+"/stdio:o").c_str(),(TOPIC+mUUID+"/topic_o").c_str());
        yarp::os::Network::disconnect((TOPIC+mUUID+"/topic_o").c_str(),(mUUID+"/stdin").c_str());

        mRPort.interrupt();
        mRPort.close();
        yarp::os::NetworkBase::unregisterName(mRPortName.c_str());

        mWPort.interrupt();
        mWPort.close();
        yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

        system((yarp::os::ConstString("yarp topic --remove ")+mUUID+"/topic_i").c_str());
        system((yarp::os::ConstString("yarp topic --remove ")+mUUID+"/topic_o").c_str());

#if !defined(WIN32)
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
        if (bot.size()==1)
        {
            ACE_OS::printf("%s",bot.get(0).asString().c_str());
            ACE_OS::fflush(stdout);
            return true;
        }

        if (bot.size()==2 && bot.get(0).isInt() && bot.get(1).isString()) 
        {
            if (bot.get(0).asInt()!=1) 
                {
                    ACE_OS::printf("%s",bot.get(1).asString().c_str());
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
#if 0
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 
            {
                if (buf[i]=='\n') 
                {
                    buf[i]='\0';
                    done=true;
                    break;
                }
            }
#endif
            txt+=buf;
			done=true;
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
yarp::os::ConstString RunReadWrite::mUUID;

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

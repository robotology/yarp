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

#if !defined(WIN32)
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

#include <yarp/os/impl/RunCheckpoints.h>

#if defined(WIN32)
static void sigbreakHandler(int sig)
{
    _BEGIN
    raise(SIGINT);
    _RETURN_VOID
}
#endif

/////////////////////////////////////

static void wSigintHandler(int sig);

int RunWrite::loop(yarp::os::ConstString& uuid)
{
    _BEGIN

    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,wSigintHandler);
    signal(SIGTERM,wSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    _CHECKPOINT

    mWPortName=uuid+"/stdout";

    if (!mWPort.open(mWPortName.c_str()))
    {
        YARP_ERROR(yarp::os::impl::Logger::get(),"RunWrite: could not open output port\n");

        _RETURN(1)
    }

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect(mWPortName.c_str(),(TOPIC+uuid+"/topic_i").c_str());

    _CHECKPOINT

    char txt[2048];
    
    while (ACE_OS::fgets(txt,2048,stdin))
    {
        if (ferror(stdin)) break;

        if (feof(stdin)) break;

        if (txt[0]<32 && txt[0]!='\n' && txt[0]!='\r' && txt[0]!='\t' && txt[0]!='\0') break; 
       
        yarp::os::Bottle bot;
        bot.addString(txt);
        _CHECKPOINT
        mWPort.write(bot);
        _CHECKPOINT
    }

    _CHECKPOINT

    mWPort.interrupt();

    _CHECKPOINT

    mWPort.close();

    _CHECKPOINT

    yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

    _RETURN(0)
}

void RunWrite::close()
{
    _BEGIN

#if defined(WIN32)
    mWPort.interrupt();
    _CHECKPOINT
    mWPort.close();
    _CHECKPOINT
    yarp::os::NetworkBase::unregisterName(mWPortName.c_str());
    _EXIT(0)
#else
    fclose(stdin);
    _RETURN_VOID
#endif
}

yarp::os::Port RunWrite::mWPort;
yarp::os::ConstString RunWrite::mWPortName;

static void wSigintHandler(int sig)
{
    _BEGIN
    RunWrite::close();
    _RETURN_VOID
}

///////////////////////////////////////////

static void rSigintHandler(int sig);

int RunRead::loop(yarp::os::ConstString& uuid)
{
    _BEGIN

    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rSigintHandler);
    signal(SIGTERM,rSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,SIG_IGN);
#endif

    _CHECKPOINT

    mRPortName=uuid+"/stdin";
    mRPort.setReader(*this);

    if (!mRPort.open(mRPortName.c_str()))
    {
        YARP_ERROR(yarp::os::impl::Logger::get(),"RunRead: could not open input port\n");
        
        _RETURN(1)
    }

    _CHECKPOINT

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect((TOPIC+uuid+"/topic_o").c_str(),mRPortName.c_str());

    _CHECKPOINT
    mDone.wait();
    _CHECKPOINT

    mRPort.interrupt();
    _CHECKPOINT
    mRPort.close();
    _CHECKPOINT
    yarp::os::NetworkBase::unregisterName(mRPortName.c_str());
    
    _RETURN(0)
}

bool RunRead::read(yarp::os::ConnectionReader& reader) 
{
    _BEGIN
    
    if (!reader.isValid())
    {
        _RETURN(false)
    }

    _CHECKPOINT

    yarp::os::Bottle bot;

    if (bot.read(reader))
    {
        if (bot.size()==1)
        {
            ACE_OS::printf("%s",bot.get(0).asString().c_str());
            ACE_OS::fflush(stdout);
            
            _RETURN(true)
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

        _RETURN(true)
    }

    _RETURN(false)
}

yarp::os::Semaphore RunRead::mDone(0);
yarp::os::Port RunRead::mRPort;
yarp::os::ConstString RunRead::mRPortName;

static void rSigintHandler(int sig)
{
    _BEGIN
    RunRead::close();
    _RETURN_VOID
}

///////////////////////////////////////////

static void rwSigintHandler(int sig);

#if !defined(WIN32)
static void sighupHandler(int sig);
#endif

int RunReadWrite::loop(yarp::os::ConstString &uuid)
{
    _BEGIN

    yarp::os::impl::Logger::get().setVerbosity(-1);

    signal(SIGINT,rwSigintHandler);
    signal(SIGTERM,rwSigintHandler);

#if defined(WIN32)
    signal(SIGBREAK,(ACE_SignalHandler)sigbreakHandler);
#else
    signal(SIGHUP,sighupHandler);
#endif

    _CHECKPOINT

    mUUID=uuid;
    mWPortName=uuid+"/stdio:o";
    mRPortName=uuid+"/stdio:i";

    mRPort.setReader(*this);

    if (!mRPort.open(mRPortName.c_str()))
    {
        YARP_ERROR(yarp::os::impl::Logger::get(),"RunReadWrite: could not open input port\n");
        
        _RETURN(1)
    }

    if (!mWPort.open(mWPortName.c_str()))
    {
        _CHECKPOINT
        YARP_ERROR(yarp::os::impl::Logger::get(),"RunReadWrite: could not open output port\n");
        _CHECKPOINT
        mRPort.close();
        _CHECKPOINT
        yarp::os::NetworkBase::unregisterName(mRPortName.c_str());
        _RETURN(1)
    }

    _CHECKPOINT

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::connect(mWPortName.c_str(),(TOPIC+uuid+"/topic_o").c_str());
    yarp::os::Network::connect((TOPIC+uuid+"/topic_i").c_str(),mRPortName.c_str());

    _CHECKPOINT

    char txt[2048];
    
    while (ACE_OS::fgets(txt,2048,stdin))
    {
        if (ferror(stdin)) break;

        if (feof(stdin)) break;

        if (txt[0]<32 && txt[0]!='\n' && txt[0]!='\r' && txt[0]!='\t' && txt[0]!='\0') break; 
       
        yarp::os::Bottle bot;
        bot.addString(txt);
        _CHECKPOINT
        mWPort.write(bot);
        _CHECKPOINT
    }

    _CHECKPOINT

    close();

    _RETURN(0)
}

void RunReadWrite::close()
{
    _BEGIN

#if defined(WIN32)

    if (mClosed)
    {
        _RETURN_VOID
    }

    mClosed=true;

#else    

    mDone.wait();
    if (mClosed)
    {
        _CHECKPOINT
        mDone.post();
        _RETURN_VOID
    }

    mClosed=true;
    mDone.post();

#endif    

    static const yarp::os::ConstString TOPIC("topic:/");
    yarp::os::Network::disconnect((mUUID+"/stdout").c_str(),(TOPIC+mUUID+"/topic_i").c_str());
    yarp::os::Network::disconnect((TOPIC+mUUID+"/topic_i").c_str(),(mUUID+"/stdio:i").c_str());
    yarp::os::Network::disconnect((mUUID+"/stdio:o").c_str(),(TOPIC+mUUID+"/topic_o").c_str());
    yarp::os::Network::disconnect((TOPIC+mUUID+"/topic_o").c_str(),(mUUID+"/stdin").c_str());

    _CHECKPOINT

    mRPort.interrupt();
    mRPort.close();
    yarp::os::NetworkBase::unregisterName(mRPortName.c_str());

    _CHECKPOINT

    mWPort.interrupt();
    mWPort.close();
    yarp::os::NetworkBase::unregisterName(mWPortName.c_str());

    _CHECKPOINT

    int ret=0;
    ret=system((yarp::os::ConstString("yarp topic --remove ")+mUUID+"/topic_i").c_str());

    _CHECKPOINT

    if (ret!=0) YARP_LOG_ERROR("call to system returned error");

    ret=system((yarp::os::ConstString("yarp topic --remove ")+mUUID+"/topic_o").c_str());

    _CHECKPOINT

    if (ret!=0) YARP_LOG_ERROR("call to system returned error");

    _EXIT(0)
}

bool RunReadWrite::read(yarp::os::ConnectionReader& reader) 
{
    _BEGIN
    
    if (!reader.isValid())
    { 
        _RETURN(false)
    }

    yarp::os::Bottle bot;

    if (bot.read(reader))
    {
        if (bot.size()==1)
        {
            ACE_OS::printf("%s",bot.get(0).asString().c_str());
            ACE_OS::fflush(stdout);
 
            _RETURN(true)
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

        _RETURN(true)
    }

    _RETURN(false)
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
    _BEGIN
    RunReadWrite::close();
    _RETURN_VOID
}

#if !defined(WIN32)
static void sighupHandler(int sig)
{
    _BEGIN
    RunReadWrite::close();
    _RETURN_VOID
}
#endif

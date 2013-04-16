// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007-2009 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef __RUN_PROC_MANAGER_H__
#define __RUN_PROC_MANAGER_H__

#if defined(WIN32)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <errno.h>
#  include <stdlib.h>
#  include <fcntl.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <yarp/os/Run.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ConstString.h>

#if defined(WIN32)
typedef DWORD PID;
typedef HANDLE FDESC;
#else
typedef int PID;
typedef int FDESC;
typedef void* HANDLE;
int CLOSE(int h);
int SIGNAL(int pid,int signum);
void sigchild_handler(int sig);
#endif

#define YARPRUN_ERROR -1

class YarpRunProcInfo
{
public:
    YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold);
    virtual ~YarpRunProcInfo(){}
    virtual bool Match(yarp::os::ConstString& alias){ return mAlias==alias; }

    virtual bool Signal(int signum);

    virtual bool Clean();

    virtual bool IsActive();

    void setCmd(yarp::os::ConstString cmd) { mCmd = cmd; }
    void setEnv(yarp::os::ConstString env) { mEnv = env; }

protected:
    yarp::os::ConstString mAlias;
    yarp::os::ConstString mOn;

    PID mPidCmd;

    HANDLE mHandleCmd; // only windows
    bool mHold;        // only linux

    yarp::os::ConstString mCmd;
    yarp::os::ConstString mEnv;

    friend class YarpRunInfoVector; 
};

class YarpRunInfoVector
{
public:
    YarpRunInfoVector();
    ~YarpRunInfoVector();

    int Size(){ return m_nProcesses; }
    bool Add(YarpRunProcInfo *process);
    int Signal(yarp::os::ConstString& alias,int signum);
    int Killall(int signum);

#if defined(WIN32)
    void GetHandles(HANDLE* &lpHandles,DWORD &nCount);
#else
    void CleanZombies();
#endif

    yarp::os::Bottle PS();
    bool IsRunning(yarp::os::ConstString &alias);

    yarp::os::Semaphore mutex;

protected:
    void Pack();

    static const int MAX_PROCESSES=1024;
    int m_nProcesses;
    YarpRunProcInfo* m_apList[MAX_PROCESSES];
    YarpRunInfoVector *m_pStdioMate;
};

class YarpRunCmdWithStdioInfo : public YarpRunProcInfo
{
public:
    YarpRunCmdWithStdioInfo(yarp::os::ConstString& alias,
        yarp::os::ConstString& on,
        yarp::os::ConstString& stdio,
        PID pidCmd,
        yarp::os::ConstString& stdioUUID,
        YarpRunInfoVector* stdioVector,
        PID pidStdin,
        PID pidStdout,
        FDESC readFromPipeStdinToCmd,
        FDESC writeToPipeStdinToCmd,
        FDESC readFromPipeCmdToStdout,
        FDESC writeToPipeCmdToStdout,
        HANDLE handleCmd,
        bool hold);

    virtual ~YarpRunCmdWithStdioInfo(){}

    virtual bool Clean();

    void TerminateStdio();

protected:
    PID mPidStdin;
    PID mPidStdout;

    bool mKillingCmd;
    bool mKillingStdio;
    bool mKillingStdin;
    bool mKillingStdout;

    yarp::os::ConstString mStdio;
    yarp::os::ConstString mStdioUUID;

    FDESC mWriteToPipeStdinToCmd;
    FDESC mReadFromPipeStdinToCmd;
    FDESC mWriteToPipeCmdToStdout;
    FDESC mReadFromPipeCmdToStdout;

    YarpRunInfoVector *mStdioVector;
};

inline yarp::os::ConstString int2String(int x)
{
    char buff[16];
    sprintf(buff,"%d",x);
    return yarp::os::ConstString(buff);
}

#endif

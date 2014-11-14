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
#  include <unistd.h>
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

#include <yarp/os/Thread.h>

typedef int PID;
typedef int FDESC;
typedef void* HANDLE;

int CLOSE(int h);
int SIGNAL(int pid,int signum);

class ZombieHunterThread : public yarp::os::Thread
{
public:
    ZombieHunterThread()
    {
        int warn_suppress=pipe(pipe_sync);
    }
   ~ZombieHunterThread(){}

    void onStop()
    {
        close(pipe_sync[0]);
        close(pipe_sync[1]);
    }

    void run()
    {
        char dummy[8];

        while (!isStopping())
        {
            if (read(pipe_sync[0],dummy,1)<=0) break;

            while (true)
            {
                PID zombie=wait(NULL);

                //PID zombie=waitpid(-1,NULL,WNOHANG);

                if (zombie>0)
                {
                    yarp::os::Run::CleanZombie(zombie);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void sigchldHandler()
    {
        ssize_t warn_suppress=write(pipe_sync[1],"zombie",1);
    }

protected:
    int pipe_sync[2];
};

#endif


#define YARPRUN_ERROR -1

class YarpRunProcInfo
{
public:
    YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold);
    virtual ~YarpRunProcInfo()
    {

    }
    bool Match(yarp::os::ConstString& alias){ return mAlias==alias; }
#ifndef WIN32
    virtual bool Clean(PID pid,YarpRunProcInfo* &pRef)
    {
        if (mPidCmd==pid)
        {
            mPidCmd=0;
            pRef=this;
            return true;
        }

        pRef=NULL;
        return false;
    }
#endif
    virtual bool Signal(int signum);

    virtual bool Clean();

    virtual bool IsActive();

    virtual void finalize(){}

    void setCmd(yarp::os::ConstString cmd) { mCmd = cmd; }
    void setEnv(yarp::os::ConstString env) { mEnv = env; }

protected:
    yarp::os::ConstString mAlias;
    yarp::os::ConstString mOn;

    PID mPidCmd;
    bool mCleanCmd;

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
    HANDLE hZombieHunter;
    void GetHandles(HANDLE* &lpHandles,DWORD &nCount);
#else
    bool CleanZombie(int zombie);
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
        PID pidStdout,
        FDESC readFromPipeCmdToStdout,
        FDESC writeToPipeCmdToStdout,
        HANDLE handleCmd,
        bool hold);

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

    virtual void finalize()
    {
        TerminateStdio();
    }

    void TerminateStdio();

#ifndef WIN32
    virtual bool Clean(PID pid,YarpRunProcInfo* &pRef)
    {
        pRef=NULL;

        if (mPidCmd==pid)
        {
            mPidCmd=0;

            if (!mKillingStdin && mPidStdin) kill(mPidStdin, SIGTERM);
            if (!mKillingStdout && mPidStdout) kill(mPidStdout,SIGTERM);

            mKillingStdin=mKillingStdout=true;
        }
        else if (mPidStdin==pid)
        {
            mPidStdin=0;

            if (!mKillingCmd && mPidCmd) kill(mPidCmd,SIGTERM);
            if (!mKillingStdout && mPidStdout) kill(mPidStdout,SIGTERM);

            mKillingCmd=mKillingStdout=true;
        }
        else if (mPidStdout==pid)
        {
            mPidStdout=0;

            if (!mKillingCmd && mPidCmd) kill(mPidCmd,SIGTERM);
            if (!mKillingStdin && mPidStdin) kill(mPidStdin,SIGTERM);

            mKillingCmd=mKillingStdin=true;
        }
        else return false;

        if (!mKillingStdio)
        {
            mKillingStdio=true;

            if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
            if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
            if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
            if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

            mWriteToPipeStdinToCmd=0;
            mReadFromPipeStdinToCmd=0;
            mWriteToPipeCmdToStdout=0;
            mReadFromPipeCmdToStdout=0;
        }

        if (!mPidCmd && !mPidStdin && !mPidStdout) pRef=this;

        return true;
    }
#endif
protected:
    PID mPidStdin;
    PID mPidStdout;
    bool mCleanStdin;
    bool mCleanStdout;

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

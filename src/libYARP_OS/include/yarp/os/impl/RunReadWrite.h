// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef __RUN_READ_WRITE_H__
#define __RUN_READ_WRITE_H__

#include <string>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/RunCheckpoints.h>

#include <stdlib.h>
#include <fcntl.h>

#if defined(WIN32)
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

///////////////////////////////////////////

class RunStdio
{
public:
    virtual ~RunStdio(){}
    virtual void exit()=0;
};

/*
class RunTerminator : public yarp::os::Thread
{
public:
    RunTerminator(RunStdio* pStdio)
    {
        mStdio=pStdio;
        CreatePipe(&hReadPipe,&hWritePipe,NULL,0);
    }

    ~RunTerminator()
    {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
    }

    void run()
    {
        DWORD nr;
        char dummy[24];
        ReadFile(hReadPipe,dummy,1,&nr,NULL);
        RUNLOG("mStdio->exit()")      
        mStdio->exit();
    }

    void exit()
    {
        DWORD nw;
        WriteFile(hWritePipe,"*",1,&nw,NULL);
    }

protected:
    HANDLE hReadPipe,hWritePipe;
    RunStdio* mStdio;
};
*/

class RunTerminator
#if !defined(WIN32)
    : public yarp::os::Thread
#endif
{
public:
    RunTerminator(RunStdio* pStdio)
    {
        mStdio=pStdio;
#if !defined(WIN32)
        int pipe_block[2];
        pipe(pipe_block);
        fwait=fdopen(pipe_block[0],"r");
        fpost=fdopen(pipe_block[1],"w");
#endif
    }

#if defined(WIN32)
    void start(){}
#endif

    ~RunTerminator()
    {
#if !defined(WIN32)
        fclose(fwait);
        fclose(fpost);
#endif
    }

    void run()
    {
#if !defined(WIN32)
        char dummy[24];
        fgets(dummy,16,fwait);
        RUNLOG("mStdio->exit()")      
        mStdio->exit();
#endif
    }

    void exit()
    {
#if defined(WIN32)
        mStdio->exit();
#else
        fprintf(fpost,"SHKIATTETE!\n");
        fflush(fpost);
#endif
    }

protected:
    FILE *fwait,*fpost;
    RunStdio* mStdio;
};

///////////////////////////////////////////

class RunWrite : public RunStdio
{
public:
    RunWrite()
    { 
        mRunning=true;
    }
   ~RunWrite(){}

    int loop(yarp::os::ConstString& uuid);

    void exit()
    {
        RUNLOG("<<<exit()")
        
        mRunning=false;
        wPort.close();

#if defined(WIN32)
        RUNLOG(">>>exit()")
        ::exit(0);
#else
        int term_pipe[2];
        pipe(term_pipe);
        dup2(term_pipe[0],STDIN_FILENO);
        FILE* file_term_pipe=fdopen(term_pipe[1],"w");
        fprintf(file_term_pipe,"SHKIATTETE!\n");
        fflush(file_term_pipe);
        fclose(file_term_pipe);
        RUNLOG(">>>exit()")
#endif
    }

protected:
    bool mRunning;

    yarp::os::ConstString UUID;
    yarp::os::ConstString wPortName;
    yarp::os::Port wPort;
};

///////////////////////////////////////////

class RunRead : public RunStdio
{
public:
    RunRead(){ mRunning=true; }
   ~RunRead(){}

    int loop(yarp::os::ConstString& uuid);

    void exit()
    {
        RUNLOG("<<<exit()")

        mRunning=false;
        
        rPort.interrupt();

        RUNLOG(">>>exit()")
    }

protected:
    bool mRunning;

    yarp::os::ConstString UUID;
    yarp::os::ConstString rPortName;
    yarp::os::Port rPort;
};

///////////////////////////////////////////

class RunReadWrite : public RunStdio, public yarp::os::Thread
{
public:
    RunReadWrite(){ mRunning=true; }
   ~RunReadWrite(){}

    void run();
    int loop(yarp::os::ConstString& uuid);

    void exit()
    {
        RUNLOG("<<<exit()")
        
        mRunning=false;

        rPort.interrupt();

#if defined(WIN32)
        for (int t=0; t<10; ++t) yarp::os::Time::delay(1.0);
#endif
        RUNLOG(">>>exit()")
    }

protected:
    bool mRunning;

    yarp::os::ConstString UUID;
    yarp::os::ConstString wPortName;
    yarp::os::ConstString rPortName;
    yarp::os::Port wPort;
    yarp::os::Port rPort;
};

#endif

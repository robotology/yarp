/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_RUNREADWRITE_H
#define YARP_RUN_IMPL_RUNREADWRITE_H

#include <string>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>
#include <yarp/os/PortReader.h>
#include <string>
#include <yarp/os/Network.h>
#include <yarp/run/impl/PlatformUnistd.h>
#include <yarp/run/impl/RunCheckpoints.h>

#include <cstdlib>
#include <fcntl.h>

#if defined(_WIN32)
#include <windows.h>
#include <process.h>
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
        CreatePipe(&hReadPipe, &hWritePipe, nullptr, 0);
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
        ReadFile(hReadPipe, dummy, 1, &nr, nullptr);
        RUNLOG("mStdio->exit()")
        mStdio->exit();
    }

    void exit()
    {
        DWORD nw;
        WriteFile(hWritePipe, "*", 1, &nw, nullptr);
    }

protected:
    HANDLE hReadPipe, hWritePipe;
    RunStdio* mStdio;
};
*/

class RunTerminator
#if !defined(_WIN32)
    : public yarp::os::Thread
#endif
{
public:
    RunTerminator(RunStdio* pStdio)
    {
        mStdio=pStdio;
#if !defined(_WIN32)
        int pipe_block[2];
        int warn_suppress = yarp::run::impl::pipe(pipe_block);
        YARP_UNUSED(warn_suppress);
        fwait=fdopen(pipe_block[0], "r");
        fpost=fdopen(pipe_block[1], "w");
#endif
    }

#if defined(_WIN32)
    void start(){}
#endif

    ~RunTerminator()
    {
#if !defined(_WIN32)
        fclose(fwait);
        fclose(fpost);
#endif
    }

    void run()
#if !defined(_WIN32)
    override
#endif
    {
#if !defined(_WIN32)
        char dummy[24];
        char* warn_suppress = fgets(dummy, 16, fwait);
        YARP_UNUSED(warn_suppress);
        RUNLOG("mStdio->exit()")
        mStdio->exit();
#endif
    }

    void exit()
    {
#if defined(_WIN32)
        mStdio->exit();
#else
        fprintf(fpost, "SHKIATTETE!\n");
        fflush(fpost);
#endif
    }

protected:
    FILE *fwait, *fpost;
    RunStdio* mStdio;
};

///////////////////////////////////////////

class RunWrite : public RunStdio
{
public:
    RunWrite(std::string& portName, std::string& loggerName)
    {
        mVerbose=true;

        char buff[16];
        sprintf(buff, "/%d", getpid());
        wPortName=portName+buff;
        wLoggerName=loggerName;

        /*
        //persistent connection to the logger, off by default
        yarp::os::ContactStyle style;
        style.persistent=true;
        yarp::os::Network::connect(wPortName.c_str(), loggerName.c_str(), style);
        */

        mRunning=true;
    }

    RunWrite(std::string& portName)
    {
        mVerbose=false;

        wPortName=portName+"/stdout";

        mRunning=true;
    }

   ~RunWrite(){}

    int loop();

    void exit() override
    {
        RUNLOG("<<<exit()")

        mRunning=false;
        wPort.close();

#if defined(_WIN32)
        RUNLOG(">>>exit()")
        ::exit(0);
#else
        int term_pipe[2];
        int warn_suppress = yarp::run::impl::pipe(term_pipe);
        YARP_UNUSED(warn_suppress);
        yarp::run::impl::dup2(term_pipe[0], STDIN_FILENO);
        FILE* file_term_pipe=fdopen(term_pipe[1], "w");
        fprintf(file_term_pipe, "SHKIATTETE!\n");
        fflush(file_term_pipe);
        fclose(file_term_pipe);
        RUNLOG(">>>exit()")
#endif
    }

protected:
    bool mRunning;
    bool mVerbose;

    std::string wPortName;
    std::string wLoggerName;
    yarp::os::Port wPort;
};

///////////////////////////////////////////

class RunRead : public RunStdio
{
public:
    RunRead(std::string &portName)
    {
        rPortName=portName+"/stdin";

        mRunning=true;
    }
   ~RunRead(){}

    int loop();

    void exit() override
    {
        RUNLOG("<<<exit()")

        mRunning=false;

        rPort.interrupt();

        RUNLOG(">>>exit()")
    }

protected:
    bool mRunning;

    std::string rPortName;
    yarp::os::Port rPort;
};

///////////////////////////////////////////

class RunReadWrite : public RunStdio, public yarp::os::Thread
{
public:
    RunReadWrite(std::string &portsName, std::string &fpName, std::string &lpName)
    {
        UUID=portsName;
        wPortName=portsName+"/stdio:o";
        rPortName=portsName+"/stdio:i";
        mRunning=true;

        if (fpName!="")
        {
            char buff[16];
            sprintf(buff, "/%d", getpid());
            mForwarded=true;
            fPortName=fpName+buff;

            yarp::os::ContactStyle style;
            style.persistent=true;
            yarp::os::Network::connect(fPortName.c_str(), lpName.c_str(), style);
        }
        else
        {
            mForwarded=false;
        }
    }

   ~RunReadWrite(){}

    void run() override;
    int loop();

    void exit() override
    {
        RUNLOG("<<<exit()")

        mRunning=false;

        rPort.interrupt();

#if defined(_WIN32)
        for (int t=0; t<10; ++t) yarp::os::SystemClock::delaySystem(1.0);
#endif
        RUNLOG(">>>exit()")
    }

protected:
    bool mRunning;
    bool mForwarded;

    std::string UUID;
    std::string wPortName;
    std::string rPortName;
    std::string fPortName;
    yarp::os::Port wPort;
    yarp::os::Port rPort;
    yarp::os::Port fPort;
};

#endif // YARP_RUN_IMPL_RUNREADWRITE_H

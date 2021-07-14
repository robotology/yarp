/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_RUNPROCMANAGER_H
#define YARP_RUN_IMPL_RUNPROCMANAGER_H

#if defined(_WIN32)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <sys/types.h>
#  include <cerrno>
#  include <cstdlib>
#  include <fcntl.h>
#  include <yarp/os/impl/PlatformSysWait.h>
#  include <yarp/run/impl/PlatformUnistd.h>
#endif

#include <cstdio>
#include <yarp/run/Run.h>
#include <yarp/os/Bottle.h>
#include <mutex>
#include <string>
#include <yarp/os/impl/PlatformSignal.h>


#if defined(_WIN32)
typedef DWORD PID;
typedef HANDLE FDESC;
#else
#include <yarp/os/Thread.h>
typedef pid_t PID;
typedef int FDESC;
typedef void* HANDLE;

int CLOSE(int h);
int SIGNAL(int pid, int signum);

class ZombieHunterThread : public yarp::os::Thread
{
public:
    ZombieHunterThread()
    {
        int warn_suppress = yarp::run::impl::pipe(pipe_sync);
        YARP_UNUSED(warn_suppress);
    }
    virtual ~ZombieHunterThread(){}

    void onStop() override
    {
        close(pipe_sync[0]);
        close(pipe_sync[1]);
    }

    void run() override
    {
        char dummy[8];

        while (!isStopping())
        {

            if (read(pipe_sync[0], dummy, sizeof(char) * 8) <= 0) {
                //If EOF or error
                break;
            }
            //else if I'm here it means a child has terminated
            //If I call wait I can find the exit status of the child process

            while (true)
            {
                //check exit status of the child
                PID zombie = yarp::os::impl::wait(nullptr);
                //PID can be:
                // - Child stopped or terminated => PID of child
                // - Error => -1

                //PID zombie=waitpid(-1, nullptr, WNOHANG);

                if (zombie > 0)
                {
                    //Remove child information from the process info table
                    yarp::run::Run::CleanZombie(zombie);
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
        ssize_t warn_suppress = write(pipe_sync[1], "zombie", sizeof(char) * (strlen("zombie") + 1));
        YARP_UNUSED(warn_suppress);
    }

protected:
    int pipe_sync[2];
};

#endif


#define YARPRUN_ERROR -1

class YarpRunProcInfo
{
public:
    YarpRunProcInfo(std::string& alias, std::string& on, PID pidCmd, HANDLE handleCmd, bool hold);
    virtual ~YarpRunProcInfo()
    {

    }
    bool Match(std::string& alias){ return mAlias==alias; }
#if !defined(_WIN32)
    virtual bool Clean(PID pid, YarpRunProcInfo* &pRef)
    {
        if (mPidCmd==pid)
        {
            mPidCmd=0;
            pRef=this;
            return true;
        }

        pRef = nullptr;
        return false;
    }
#endif
    virtual bool Signal(int signum);

    virtual bool Clean();

    virtual bool IsActive();

    virtual void finalize(){}

    void setCmd(const std::string& cmd) { mCmd = cmd; }
    void setEnv(const std::string& env) { mEnv = env; }

protected:
    std::string mAlias;
    std::string mOn;

    PID mPidCmd;
    bool mCleanCmd;

    HANDLE mHandleCmd; // only windows
    bool mHold;        // only linux

    std::string mCmd;
    std::string mEnv;

    friend class YarpRunInfoVector;
};

class YarpRunInfoVector
{
public:
    YarpRunInfoVector();
    ~YarpRunInfoVector();

    int Size(){ return m_nProcesses; }
    bool Add(YarpRunProcInfo *process);
    int Signal(std::string& alias, int signum);
    int Killall(int signum);

#if defined(_WIN32)
    HANDLE hZombieHunter;
    void GetHandles(HANDLE* &lpHandles, DWORD &nCount);
#else
    bool CleanZombie(int zombie);
#endif

    yarp::os::Bottle PS();
    bool IsRunning(std::string &alias);

    std::mutex mutex;

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
    YarpRunCmdWithStdioInfo(std::string& alias,
        std::string& on,
        std::string& stdio,
        PID pidCmd,
        PID pidStdout,
        FDESC readFromPipeCmdToStdout,
        FDESC writeToPipeCmdToStdout,
        HANDLE handleCmd,
        bool hold);

    YarpRunCmdWithStdioInfo(std::string& alias,
        std::string& on,
        std::string& stdio,
        PID pidCmd,
        std::string& stdioUUID,
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

    bool Clean() override;

    void finalize() override
    {
        TerminateStdio();
    }

    void TerminateStdio();

#if !defined(_WIN32)
    bool Clean(PID pid, YarpRunProcInfo* &pRef) override
    {
        pRef = nullptr;

        if (mPidCmd==pid)
        {
            mPidCmd=0;

            if (!mKillingStdin && mPidStdin) {
                yarp::os::impl::kill(mPidStdin, SIGTERM);
            }
            if (!mKillingStdout && mPidStdout) {
                yarp::os::impl::kill(mPidStdout, SIGTERM);
            }

            mKillingStdin=mKillingStdout=true;
        }
        else if (mPidStdin==pid)
        {
            mPidStdin=0;

            if (!mKillingCmd && mPidCmd) {
                yarp::os::impl::kill(mPidCmd, SIGTERM);
            }
            if (!mKillingStdout && mPidStdout) {
                yarp::os::impl::kill(mPidStdout, SIGTERM);
            }

            mKillingCmd=mKillingStdout=true;
        }
        else if (mPidStdout==pid)
        {
            mPidStdout=0;

            if (!mKillingCmd && mPidCmd) {
                yarp::os::impl::kill(mPidCmd, SIGTERM);
            }
            if (!mKillingStdin && mPidStdin) {
                yarp::os::impl::kill(mPidStdin, SIGTERM);
            }

            mKillingCmd=mKillingStdin=true;
        } else {
            return false;
        }

        if (!mKillingStdio)
        {
            mKillingStdio=true;

            if (mWriteToPipeStdinToCmd) {
                CLOSE(mWriteToPipeStdinToCmd);
            }
            if (mReadFromPipeStdinToCmd) {
                CLOSE(mReadFromPipeStdinToCmd);
            }
            if (mWriteToPipeCmdToStdout) {
                CLOSE(mWriteToPipeCmdToStdout);
            }
            if (mReadFromPipeCmdToStdout) {
                CLOSE(mReadFromPipeCmdToStdout);
            }

            mWriteToPipeStdinToCmd=0;
            mReadFromPipeStdinToCmd=0;
            mWriteToPipeCmdToStdout=0;
            mReadFromPipeCmdToStdout=0;
        }

        if (!mPidCmd && !mPidStdin && !mPidStdout) {
            pRef = this;
        }

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

    std::string mStdio;
    std::string mStdioUUID;

    FDESC mWriteToPipeStdinToCmd;
    FDESC mReadFromPipeStdinToCmd;
    FDESC mWriteToPipeCmdToStdout;
    FDESC mReadFromPipeCmdToStdout;

    YarpRunInfoVector *mStdioVector;
};

inline std::string int2String(int x)
{
    char buff[16];
    sprintf(buff, "%d", x);
    return std::string(buff);
}

#endif // YARP_RUN_IMPL_RUNPROCMANAGER_H

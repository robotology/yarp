/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/run/impl/RunProcManager.h>
#include <yarp/run/impl/RunCheckpoints.h>

#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/PlatformSysWait.h>

#include <cstring>

#define WAIT() { RUNLOG("<<<mutex.lock()") mutex.lock(); RUNLOG(">>>mutex.lock()") }
#define POST() { RUNLOG("<<<mutex.unlock()") mutex.unlock(); RUNLOG(">>>mutex.unlock()") }

#if defined(_WIN32)
    #include <process.h>

    #define SIGKILL 9
    static bool KILL(HANDLE handle)
    {
        BOOL bRet=TerminateProcess(handle, 0);

        CloseHandle(handle);
        fprintf(stderr, "brutally terminated by TerminateProcess\n");

        return bRet?true:false;
    }
    static bool TERMINATE(PID pid);
    #define CLOSE(h) CloseHandle(h)
    #ifndef __GNUC__
    static DWORD WINAPI ZombieHunter(__in LPVOID lpParameter)
    #else
    static DWORD WINAPI ZombieHunter(LPVOID lpParameter)
    #endif
    {
        YarpRunInfoVector* pProcessVector=(YarpRunInfoVector*)lpParameter;

        while (true)
        {
            DWORD nCount=0;
            HANDLE* aHandlesVector = nullptr;
            pProcessVector->GetHandles(aHandlesVector, nCount);

            if (nCount)
            {
                WaitForMultipleObjects(nCount, aHandlesVector, FALSE, INFINITE);
                delete [] aHandlesVector;
            }
            else
            {
                //hZombieHunter = nullptr;

                return 0;
            }
        }

        return 0;
    }
#else // LINUX
    #include <unistd.h>
    #include <fcntl.h>

    int CLOSE(int h)
    {
        int ret=(close(h)==0);
        return ret;
    }

    int SIGNAL(int pid, int signum)
    {
        int ret=!yarp::os::impl::kill(pid, signum);
        return ret;
    }
#endif // LINUX

YarpRunProcInfo::YarpRunProcInfo(std::string& alias, std::string& on, PID pidCmd, HANDLE handleCmd, bool hold)
{
    mAlias=alias;
    mOn=on;
    mPidCmd=pidCmd;
    mCleanCmd=false;
    mHandleCmd=handleCmd;
    mHold=hold;
}

bool YarpRunProcInfo::Signal(int signum)
{
#if defined(_WIN32)
    if (signum==SIGKILL)
    {
        if (mHandleCmd)
        {
            bool ret=KILL(mHandleCmd);
            return ret;
        }
    }
    else
    {
        if (mPidCmd)
        {
            bool ret=TERMINATE(mPidCmd);
            return ret;
        }
    }
#else
    if (mPidCmd && !mHold)
    {
        bool ret=SIGNAL(mPidCmd, signum);
        return ret;
    }
#endif

    return true;
}

bool YarpRunProcInfo::IsActive()
{
    if (!mPidCmd)
    {
        return false;
    }
#if defined(_WIN32)
    DWORD status;
    RUNLOG("<<<GetExitCodeProcess(mHandleCmd, &status)")
    bool ret=(::GetExitCodeProcess(mHandleCmd, &status) && status==STILL_ACTIVE);
    RUNLOG(">>>GetExitCodeProcess(mHandleCmd, &status)")
    return ret;
#else
    bool ret=!yarp::os::impl::kill(mPidCmd, 0);
    return ret;
#endif
}

bool YarpRunProcInfo::Clean()
{
#if !defined(_WIN32)
    if (!mCleanCmd && yarp::os::impl::waitpid(mPidCmd, nullptr, WNOHANG) == mPidCmd)
    {
        fprintf(stderr, "CLEANUP cmd %d\n", mPidCmd);
        mCleanCmd=true;
    }

    return mCleanCmd;
#else
    return true;
#endif
}

YarpRunInfoVector::YarpRunInfoVector()
{
    m_nProcesses=0;
    m_pStdioMate = nullptr;
    for (auto & i : m_apList)
    {
        i = nullptr;
    }
}

YarpRunInfoVector::~YarpRunInfoVector()
{
    WAIT()

    for (auto & i : m_apList)
    {
        if (i)
        {
            delete i;
            i = nullptr;
        }
    }

#if defined(_WIN32)
    if (hZombieHunter)
    {
        HANDLE hkill=hZombieHunter;
        hZombieHunter = nullptr;
        TerminateThread(hkill, 0);
    }
#endif

    POST()
}

bool YarpRunInfoVector::Add(YarpRunProcInfo *process)
{
    WAIT()

    if (m_nProcesses>=MAX_PROCESSES)
    {
        fprintf(stderr, "ERROR: maximum process limit reached\n");
        POST()
        return false;
    }

#if defined(_WIN32)
    if (hZombieHunter)
    {
        HANDLE hkill=hZombieHunter;
        hZombieHunter = nullptr;
        TerminateThread(hkill, 0);
    }
#endif

    m_apList[m_nProcesses++]=process;

#if defined(_WIN32)
    hZombieHunter=CreateThread(0, 0, ZombieHunter, this, 0, 0);
#endif

    POST()
    return true;
}

int YarpRunInfoVector::Signal(std::string& alias, int signum)
{
    WAIT()

    auto* *aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias)) // && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i];
        }
    }

    POST()

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr, "SIGNAL %s (%d)\n", aKill[k]->mAlias.c_str(), aKill[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    return nKill;
}

int YarpRunInfoVector::Killall(int signum)
{
    WAIT()

    auto* *aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i];
        }
    }

    POST()

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr, "SIGNAL %s (%d)\n", aKill[k]->mAlias.c_str(), aKill[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    return nKill;
}

#if defined(_WIN32)
void YarpRunInfoVector::GetHandles(HANDLE* &lpHandles, DWORD &nCount)
{
    WAIT()

    if (lpHandles) delete [] lpHandles;

    if (m_nProcesses>0) lpHandles=new HANDLE[m_nProcesses];

    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
    {
        if (!m_apList[i]->IsActive())
        {
            fprintf(stderr, "CLEANUP %s (%d)\n", m_apList[i]->mAlias.c_str(), m_apList[i]->mPidCmd);
            fflush(stderr);

            m_apList[i]->Clean();
            delete m_apList[i];
            m_apList[i]=0;
        }
    }

    Pack();

    for (int i=0; i<m_nProcesses; ++i)
    {
        lpHandles[nCount+i]=m_apList[i]->mHandleCmd;
    }

    nCount+=m_nProcesses;

    POST()
}

#else

bool YarpRunInfoVector::CleanZombie(int zombie)
{
    bool bFound=false;

    YarpRunProcInfo *pZombie = nullptr;

    WAIT()

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Clean(zombie, pZombie))
        {
            bFound=true;
            if (pZombie) {
                m_apList[i] = nullptr;
            }
            break;
        }
    }

    Pack();

    POST()

    if (pZombie)
    {
        pZombie->finalize();
        delete pZombie;
    }

    return bFound;
}
#endif

yarp::os::Bottle YarpRunInfoVector::PS()
{
    WAIT()

    yarp::os::Bottle ps, line, grp;

    for (int i = 0; i < m_nProcesses; ++i) {
        if (m_apList[i]) {
            line.clear();

            grp.clear();
            grp.addString("pid");
            grp.addInt32(m_apList[i]->mPidCmd);
            line.addList() = grp;

            grp.clear();
            grp.addString("tag");
            grp.addString(m_apList[i]->mAlias.c_str());
            line.addList() = grp;

            grp.clear();
            grp.addString("status");
            grp.addString(m_apList[i]->IsActive() ? "running" : "zombie");
            line.addList() = grp;

            grp.clear();
            grp.addString("cmd");
            grp.addString(m_apList[i]->mCmd.c_str());
            line.addList() = grp;

            grp.clear();
            grp.addString("env");
            grp.addString(m_apList[i]->mEnv.c_str());
            line.addList() = grp;

            ps.addList() = line;
        }
    }

    POST()

    return ps;
}

bool YarpRunInfoVector::IsRunning(std::string &alias)
{
    WAIT()

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                POST()
                return true;
            }
            else
            {
                POST()
                return false;
            }
        }
    }

    POST()

    return false;
}

void YarpRunInfoVector::Pack()
{
    int tot=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i])
        {
            m_apList[tot++]=m_apList[i];
        }
    }

    for (int i=tot; i<m_nProcesses; ++i)
    {
        m_apList[i]=nullptr;
    }

    m_nProcesses=tot;
}

YarpRunCmdWithStdioInfo::YarpRunCmdWithStdioInfo(std::string& alias,
                                                 std::string& on,
                                                 std::string& stdio,
                                                 PID pidCmd,
                                                 PID pidStdout,
                                                 FDESC readFromPipeCmdToStdout,
                                                 FDESC writeToPipeCmdToStdout,
                                                 HANDLE handleCmd,
                                                 bool hold)
                                                 :
YarpRunProcInfo(alias, on, pidCmd, handleCmd, hold)
{
    mPidStdin=0;
    mPidStdout=pidStdout;
    mStdio=stdio;
    mStdioUUID="";
    mStdioVector = nullptr;

    mReadFromPipeStdinToCmd = 0;
    mWriteToPipeStdinToCmd = 0;
    mReadFromPipeCmdToStdout=readFromPipeCmdToStdout;
    mWriteToPipeCmdToStdout=writeToPipeCmdToStdout;

    mKillingCmd=false;
    mKillingStdio=false;
    mKillingStdin=false;
    mKillingStdout=false;

    mCleanStdin=true;
    mCleanStdout=false;
}

YarpRunCmdWithStdioInfo::YarpRunCmdWithStdioInfo(std::string& alias,
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
                                                 bool hold)
                                                 :
YarpRunProcInfo(alias, on, pidCmd, handleCmd, hold)
{
    mPidStdin=pidStdin;
    mPidStdout=pidStdout;
    mStdio=stdio;
    mStdioUUID=stdioUUID;

    mStdioVector=stdioVector;

    mReadFromPipeStdinToCmd=readFromPipeStdinToCmd;
    mWriteToPipeStdinToCmd=writeToPipeStdinToCmd;
    mReadFromPipeCmdToStdout=readFromPipeCmdToStdout;
    mWriteToPipeCmdToStdout=writeToPipeCmdToStdout;

    mKillingCmd=false;
    mKillingStdio=false;
    mKillingStdin=false;
    mKillingStdout=false;

    mCleanStdin=false;
    mCleanStdout=false;
}

bool YarpRunCmdWithStdioInfo::Clean()
{
#if defined(_WIN32)
    if (mPidCmd)
    {
        mPidCmd=0;

        if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
        if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
        if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
        if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

        mWriteToPipeStdinToCmd=0;
        mReadFromPipeStdinToCmd=0;
        mWriteToPipeCmdToStdout=0;
        mReadFromPipeCmdToStdout=0;

        if (mPidStdin)  TERMINATE(mPidStdin);

        if (mPidStdout) TERMINATE(mPidStdout);

        TerminateStdio();
    }

    return false;

#else

    if (!mCleanCmd && yarp::os::impl::waitpid(mPidCmd, nullptr, WNOHANG) == mPidCmd)
    {
        fprintf(stderr, "CLEANUP cmd %d\n", mPidCmd);
        mCleanCmd=true;
    }

    if (!mCleanStdin && yarp::os::impl::waitpid(mPidStdin, nullptr, WNOHANG) == mPidStdin)
    {
        fprintf(stderr, "CLEANUP stdin %d\n", mPidStdin);
        mCleanStdin=true;
    }

    if (!mCleanStdout && yarp::os::impl::waitpid(mPidStdout, nullptr, WNOHANG) == mPidStdout)
    {
        fprintf(stderr, "CLEANUP stdout %d\n", mPidStdout);
        mCleanStdout=true;
    }

    if (!(mCleanCmd || mCleanStdin || mCleanStdout))
    {
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

    if (!mCleanCmd && !mKillingCmd)
    {
        yarp::os::impl::kill(mPidCmd, SIGTERM);
        mKillingCmd=true;
    }

    if (!mCleanStdin && !mKillingStdin)
    {
        yarp::os::impl::kill(mPidStdin, SIGTERM);
        mKillingStdin=true;
    }

    if (!mCleanStdout && !mKillingStdout)
    {
        yarp::os::impl::kill(mPidStdout, SIGTERM);
        mKillingStdout=true;
    }

    if (mCleanCmd && mCleanStdin && mCleanStdout)
    {
        return true;
    }

    return false;
#endif
}

void YarpRunCmdWithStdioInfo::TerminateStdio()
{
    if (!mStdioVector) {
        return;
    }

    if (mOn==mStdio)
    {
        mStdioVector->Signal(mAlias, SIGTERM);
    }
    else
    {
        yarp::os::Bottle msg;
        msg.fromString(std::string("(killstdio ")+mAlias+")");
        yarp::run::Run::sendMsg(msg, mStdio);
    }
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

#if defined(_WIN32)

#define TA_FAILED        0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL  2

class TerminateParams
{
public:
    TerminateParams(DWORD id)
    {
        nWin=0;
        dwID=id;
    }

    ~TerminateParams(){}

    int nWin;
    DWORD dwID;
};

BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
    TerminateParams* params=(TerminateParams*)lParam;

    DWORD dwID;
    GetWindowThreadProcessId(hwnd, &dwID) ;

    if (dwID==params->dwID)
    {
        params->nWin++;
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    return TRUE;
}

/*----------------------------------------------------------------
Purpose:
Shut down a 32-Bit Process

Parameters:
dwPID
Process ID of the process to shut down.
----------------------------------------------------------------*/
bool TERMINATE(PID dwPID)
{
    HANDLE hProc;

    // If we can't open the process with PROCESS_TERMINATE rights,
    // then we give up immediately.
    hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPID);

    if (hProc == nullptr)
    {
        return false;
    }

    // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
    // matches your process's.

    TerminateParams params(dwPID);

    EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)&params);

    if (params.nWin)
    {
        fprintf(stderr, "%d terminated by WM_CLOSE (sending anyway CTRL_C_EVENT/CTRL_BREAK_EVENT)\n", dwPID);
    }
    else
    {
        //GenerateConsoleCtrlEvent(CTRL_C_EVENT, dwPID);
        //GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, dwPID);
        fprintf(stderr, "%d terminated by CTRL_C_EVENT/CTRL_BREAK_EVENT\n", dwPID);
    }

    GenerateConsoleCtrlEvent(CTRL_C_EVENT, dwPID);
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, dwPID);

    CloseHandle(hProc);

    return true;
}

#endif

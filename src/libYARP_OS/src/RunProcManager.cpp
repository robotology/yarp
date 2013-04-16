// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007-2009 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/RunProcManager.h>

#if defined(WIN32)
#include <process.h>
#endif

#include <string.h>

#include <yarp/os/impl/RunCheckpoints.h>

#if defined(WIN32)
    extern HANDLE  hZombieHunter;
    extern HANDLE* aHandlesVector;
#endif

#if defined(WIN32)
    #define SIGKILL 9
    static bool KILL(HANDLE handle)
    {
        CHECK_ENTER("KILL")
 
        BOOL bRet=TerminateProcess(handle,0);

        CHECKPOINT()

        CloseHandle(handle);
        fprintf(stdout,"brutally terminated by TerminateProcess\n");
        fflush(stdout);
        
        CHECK_EXIT()

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
        CHECK_ENTER("ZombieHunter")

        DWORD nCount;

        while (true)
        {
            if (aHandlesVector)
            {
                delete [] aHandlesVector;
                aHandlesVector=NULL;
            }
            nCount=0;

            aHandlesVector=new HANDLE[yarp::os::Run::mProcessVector.Size()+yarp::os::Run::mStdioVector.Size()];

            yarp::os::Run::mProcessVector.GetHandles(aHandlesVector,nCount);
            yarp::os::Run::mStdioVector.GetHandles(aHandlesVector,nCount);

            if (nCount)
            {
                CHECKPOINT()

                WaitForMultipleObjects(nCount,aHandlesVector,FALSE,INFINITE);

                CHECKPOINT()
            }
            else
            {
                hZombieHunter=NULL;
                CHECK_EXIT()
                return 0;
            }
        }

        CHECK_EXIT()

        return 0;
    }
#else // LINUX
    #include <unistd.h>
    #include <fcntl.h>

    int CLOSE(int h)
    { 
        CHECK_ENTER("CLOSE");
        int ret=(close(h)==0);
        CHECK_EXIT()
        return ret; 
    }

    int SIGNAL(int pid,int signum)
    {
        CHECK_ENTER("SIGNAL") 
        int ret=!kill(pid,signum);
        CHECK_EXIT()
        return ret;
    }

    void sigchild_handler(int sig)
    {
        CHECK_ENTER("sigchild_handler")   
        yarp::os::Run::CleanZombies();
        CHECK_EXIT()
    }

#endif // LINUX

YarpRunProcInfo::YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold)
{
    CHECK_ENTER("YarpRunProcInfo::YarpRunProcInfo")
    mAlias=alias;
    mOn=on;
    mPidCmd=pidCmd;
    mHandleCmd=handleCmd;
    mHold=hold;
    CHECK_EXIT()
}

bool YarpRunProcInfo::Signal(int signum)
{
    CHECK_ENTER("YarpRunProcInfo::Signal")    
#if defined(WIN32)
    if (signum==SIGKILL)
    {
        if (mHandleCmd)
        {
            CHECKPOINT()
            bool ret=KILL(mHandleCmd);
            CHECK_EXIT()
            return ret;
        }
    }
    else
    {
        if (mPidCmd)
        {
            CHECKPOINT() 
            bool ret=TERMINATE(mPidCmd);
            CHECK_EXIT()
            return ret;
        }
    }
#else
    if (mPidCmd && !mHold)
    {
        CHECKPOINT()
        bool ret=SIGNAL(mPidCmd,signum);
        CHECK_EXIT()
        return ret;
    }
#endif
    CHECK_EXIT()
    return true;
}

bool YarpRunProcInfo::IsActive()
{
    CHECK_ENTER("YarpRunProcInfo::IsActive")
    if (!mPidCmd)
    {
        CHECK_EXIT()
        return false;
    }
#if defined(WIN32)
    DWORD status;
    bool ret=(::GetExitCodeProcess(mHandleCmd,&status) && status==STILL_ACTIVE);
    CHECK_EXIT()
    return ret;
#else
    bool ret=!kill(mPidCmd,0);
    CHECK_EXIT()
    return ret;
#endif
}

bool YarpRunProcInfo::Clean()
{
    CHECK_ENTER("YarpRunProcInfo::Clean")

#if !defined(WIN32)
    if (mPidCmd && waitpid(mPidCmd,NULL,WNOHANG)==mPidCmd)
    {
        CHECKPOINT() 
        fprintf(stderr,"CLEANUP cmd %d\n",mPidCmd);
        mPidCmd=0;
    } 

    if (mPidCmd)
    {
        CHECK_EXIT()
        return false;
    }
#endif

    CHECK_EXIT()
    return true;
}

YarpRunInfoVector::YarpRunInfoVector()
{
    CHECK_ENTER("YarpRunInfoVector::YarpRunInfoVector")

    m_nProcesses=0;
    
    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        m_apList[i]=0;
    }
    CHECK_EXIT()
}

YarpRunInfoVector::~YarpRunInfoVector()
{
    CHECK_ENTER("YarpRunInfoVector::~YarpRunInfoVector")
    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        if (m_apList[i])
        {
            delete m_apList[i];
        }
    }
    CHECK_EXIT()
}

bool YarpRunInfoVector::Add(YarpRunProcInfo *process)
{
    CHECK_ENTER("YarpRunInfoVector::Add")
    mutex.wait();
    CHECKPOINT()

    if (m_nProcesses>=MAX_PROCESSES)
    {
        fprintf(stderr,"ERROR: maximum process limit reached\n");
        CHECKPOINT()
        mutex.post();
        CHECK_EXIT()        
        return false;
    }

#if defined(WIN32)
    CHECKPOINT()
    if (hZombieHunter) TerminateThread(hZombieHunter,0);
    CHECKPOINT()
#endif

    m_apList[m_nProcesses++]=process;

#if defined(WIN32)
    CHECKPOINT()
    hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
    CHECKPOINT()
#endif

    CHECKPOINT()
    mutex.post();
    CHECK_EXIT()

    return true;
}

int YarpRunInfoVector::Signal(yarp::os::ConstString& alias,int signum)
{
    CHECK_ENTER("YarpRunInfoVector::Signal")
    mutex.wait();
    CHECKPOINT()

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;    

    CHECKPOINT()
    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias) && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i]; 
        }
    }
    
    CHECKPOINT()
    mutex.post();
    CHECKPOINT()

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL  %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    CHECK_EXIT()

    return nKill;
}

int YarpRunInfoVector::Killall(int signum)
{
    CHECK_ENTER("YarpRunInfoVector::Killall")
    mutex.wait();
    CHECKPOINT()

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;        

    for (int i=0; i<m_nProcesses; ++i)
    {    
        if (m_apList[i] && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i]; 
        }
    }

    CHECKPOINT()
    mutex.post();
    CHECKPOINT()

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    CHECK_EXIT()

    return nKill;
}

#if defined(WIN32) 
void YarpRunInfoVector::GetHandles(HANDLE* &lpHandles,DWORD &nCount)
{
    CHECK_ENTER("YarpRunInfoVector::GetHandles")
    mutex.wait();
    CHECKPOINT()

    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
    {
        if (!m_apList[i]->IsActive())
        {
            fprintf(stderr,"CLEANUP %s (%d)\n",m_apList[i]->mAlias.c_str(),m_apList[i]->mPidCmd);
            fflush(stderr);

            m_apList[i]->Clean();
            delete m_apList[i];
            m_apList[i]=0;
        }
    }

    CHECKPOINT()

    Pack();

    CHECKPOINT()

    for (int i=0; i<m_nProcesses; ++i)
    {
        lpHandles[nCount+i]=m_apList[i]->mHandleCmd;
    }

    nCount+=m_nProcesses;

    CHECKPOINT()
    mutex.post();
    CHECK_EXIT()
}
#else
void YarpRunInfoVector::CleanZombies()
{    
    CHECK_ENTER("YarpRunInfoVector::CleanZombies")
    mutex.wait();
    CHECKPOINT()

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Clean())
        {
            delete m_apList[i];
            m_apList[i]=NULL;                
        }
    }

    CHECKPOINT()
    
    Pack();

    CHECKPOINT()
    mutex.post();
    CHECK_EXIT()
}
#endif

yarp::os::Bottle YarpRunInfoVector::PS()
{
    CHECK_ENTER("YarpRunInfoVector::PS")
    mutex.wait();
    CHECKPOINT()

    yarp::os::Bottle ps,line,grp;

    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
    {
        line.clear();

        grp.clear();
        grp.addString("pid");
        grp.addInt(m_apList[i]->mPidCmd);
        line.addList()=grp;

        grp.clear();
        grp.addString("tag"); 
        grp.addString(m_apList[i]->mAlias.c_str());
        line.addList()=grp;

        grp.clear();
        grp.addString("status");
        grp.addString(m_apList[i]->IsActive()?"running":"zombie");
        line.addList()=grp;

        grp.clear();
        grp.addString("cmd"); 
        grp.addString(m_apList[i]->mCmd.c_str());
        line.addList()=grp;

        grp.clear();
        grp.addString("env"); 
        grp.addString(m_apList[i]->mEnv.c_str());
        line.addList()=grp;

        ps.addList()=line;
    }

    CHECKPOINT()
    mutex.post();
    CHECK_EXIT()

    return ps;
}

bool YarpRunInfoVector::IsRunning(yarp::os::ConstString &alias)
{
    CHECK_ENTER("YarpRunInfoVector::IsRunning")
    mutex.wait();
    CHECKPOINT()

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                CHECKPOINT()
                mutex.post();
                CHECK_EXIT()
                return true;
            }
            else
            {
                CHECKPOINT()
                mutex.post();
                CHECK_EXIT()
                return false;
            }
        }
    }

    CHECKPOINT()
    mutex.post();
    CHECK_EXIT()

    return false;
}

void YarpRunInfoVector::Pack()
{
    CHECK_ENTER("YarpRunInfoVector::Pack")

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
        m_apList[i]=0;
    }

    m_nProcesses=tot;

    CHECK_EXIT()
}

YarpRunCmdWithStdioInfo::YarpRunCmdWithStdioInfo(yarp::os::ConstString& alias,
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
                                                 bool hold)
                                                 : 
YarpRunProcInfo(alias,on,pidCmd,handleCmd,hold)
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::YarpRunCmdWithStdioInfo")

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

    CHECK_EXIT()
}

bool YarpRunCmdWithStdioInfo::Clean()
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::Clean")

#if defined(WIN32)
    if (mPidCmd)
    {
        mPidCmd=0;
     
        CHECKPOINT()
   
        if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
        if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
        if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
        if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

        mWriteToPipeStdinToCmd=0;
        mReadFromPipeStdinToCmd=0;
        mWriteToPipeCmdToStdout=0;
        mReadFromPipeCmdToStdout=0;

        CHECKPOINT()

        TERMINATE(mPidStdin);

        CHECKPOINT()

        TERMINATE(mPidStdout);

        CHECKPOINT()
        
        TerminateStdio();

        CHECKPOINT()
    }

    CHECK_EXIT()

    return false;
#else

    CHECKPOINT()

    if (mPidCmd && waitpid(mPidCmd,NULL,WNOHANG)==mPidCmd)
    {
        CHECKPOINT() 
        fprintf(stderr,"CLEANUP cmd %d\n",mPidCmd);
        mPidCmd=0;
    } 

    CHECKPOINT()
    
    if (mPidStdin && waitpid(mPidStdin,NULL,WNOHANG)==mPidStdin)
    {
        CHECKPOINT() 
        fprintf(stderr,"CLEANUP stdin %d\n",mPidStdin);
        mPidStdin=0;
    }    

    CHECKPOINT()

    if (mPidStdout && waitpid(mPidStdout,NULL,WNOHANG)==mPidStdout)
    {
        CHECKPOINT()
        fprintf(stderr,"CLEANUP stdout %d\n",mPidStdout);
        mPidStdout=0;
    } 

    if (mPidCmd && mPidStdin && mPidStdout)
    {
        CHECK_EXIT()

        return false;
    }

    if (!mKillingStdio)
    {
        CHECKPOINT()

        mKillingStdio=true;
        
        if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
        if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
        if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
        if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

        mWriteToPipeStdinToCmd=0;
        mReadFromPipeStdinToCmd=0;
        mWriteToPipeCmdToStdout=0;
        mReadFromPipeCmdToStdout=0;
        
        CHECKPOINT()
        TerminateStdio();
        CHECKPOINT()
    }

    CHECKPOINT()
    
    if (mPidCmd && !mKillingCmd)
    {
        kill(mPidCmd,SIGTERM);
        mKillingCmd=true;

        CHECKPOINT()
    }

    if (mPidStdin && !mKillingStdin)
    { 
        kill(mPidStdin,SIGTERM);
        mKillingStdin=true;

        CHECKPOINT()
    }

    if (mPidStdout && !mKillingStdout)
    {
        kill(mPidStdout,SIGTERM);
        mKillingStdout=true;

        CHECKPOINT()
    }
        
    if (mPidCmd || mPidStdin || mPidStdout)
    {
        CHECK_EXIT()

        return false;
    }

    CHECK_EXIT()
    
    return true;
#endif
}

void YarpRunCmdWithStdioInfo::TerminateStdio()
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::TerminateStdio")

    if (mOn==mStdio)
    {
        CHECKPOINT()

        mStdioVector->Signal(mAlias,SIGTERM);
        //mStdioVector->Signal(mAlias,SIGKILL);
    }
    else
    {
        CHECKPOINT()

        yarp::os::Bottle msg;
        msg.fromString((yarp::os::ConstString("(killstdio ")+mAlias+")").c_str());

        yarp::os::Port port;
        port.open("...");
        bool connected=yarp::os::NetworkBase::connect(port.getName(), mStdio);

        CHECKPOINT()

        if (connected)
        {
            CHECKPOINT()

            port.write(msg);
            yarp::os::NetworkBase::disconnect(port.getName().c_str(),mStdio.c_str());

            CHECKPOINT()
        }

        CHECKPOINT()
        port.close();
    }

    CHECK_EXIT()
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

#if defined(WIN32)

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

BOOL CALLBACK TerminateAppEnum(HWND hwnd,LPARAM lParam)
{
    CHECK_ENTER("TerminateAppEnum")

    TerminateParams* params=(TerminateParams*)lParam;

    DWORD dwID;
    GetWindowThreadProcessId(hwnd,&dwID) ;

    CHECKPOINT()

    if (dwID==params->dwID)
    {
        CHECKPOINT()
        params->nWin++;
        PostMessage(hwnd,WM_CLOSE,0,0);
    }

    CHECK_EXIT()

    return TRUE ;
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
    CHECK_ENTER("TERMINATE")

    HANDLE hProc;

    // If we can't open the process with PROCESS_TERMINATE rights,
    // then we give up immediately.
    hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,dwPID);

    if (hProc==NULL)
    {
        CHECK_EXIT()
        return false;
    }

    // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
    // matches your process's.

    CHECKPOINT()

    TerminateParams params(dwPID);

    CHECKPOINT()

    EnumWindows((WNDENUMPROC)TerminateAppEnum,(LPARAM)&params);

    CHECKPOINT()

    if (params.nWin)
    {
        CHECKPOINT()
        fprintf(stdout,"%d terminated by WM_CLOSE (sending anyway CTRL_C_EVENT/CTRL_BREAK_EVENT)\n",dwPID);
    }
    else
    {
        CHECKPOINT()
        //GenerateConsoleCtrlEvent(CTRL_C_EVENT,dwPID);
        //GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);
        fprintf(stdout,"%d terminated by CTRL_C_EVENT/CTRL_BREAK_EVENT\n",dwPID);    
    }

    GenerateConsoleCtrlEvent(CTRL_C_EVENT,dwPID);
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);

    fflush(stdout);

    CHECKPOINT()

    CloseHandle(hProc);

    CHECK_EXIT()

    return true;
}

#endif

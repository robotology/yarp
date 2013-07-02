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
        _BEGIN
 
        BOOL bRet=TerminateProcess(handle,0);

        _CHECKPOINT

        CloseHandle(handle);
        fprintf(stdout,"brutally terminated by TerminateProcess\n");
        fflush(stdout);
        
        _RETURN(bRet?true:false)
    }
    static bool TERMINATE(PID pid); 
    #define CLOSE(h) CloseHandle(h)
    #ifndef __GNUC__
    static DWORD WINAPI ZombieHunter(__in LPVOID lpParameter)
    #else
    static DWORD WINAPI ZombieHunter(LPVOID lpParameter)
    #endif
    {
        _BEGIN

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
                _CHECKPOINT

                WaitForMultipleObjects(nCount,aHandlesVector,FALSE,INFINITE);

                _CHECKPOINT
            }
            else
            {
                hZombieHunter=NULL;
                
                _RETURN(0)
            }
        }

        _RETURN(0)
    }
#else // LINUX
    #include <unistd.h>
    #include <fcntl.h>

    int CLOSE(int h)
    { 
        _BEGIN
        int ret=(close(h)==0);
        _RETURN(ret)
    }

    int SIGNAL(int pid,int signum)
    {
        _BEGIN
        int ret=!kill(pid,signum);
        _RETURN(ret)
    }

    void sigchild_handler(int sig)
    {
        _BEGIN   
        yarp::os::Run::CleanZombies();
        _RETURN_VOID
    }

#endif // LINUX

YarpRunProcInfo::YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold)
{
    _BEGIN
    mAlias=alias;
    mOn=on;
    mPidCmd=pidCmd;
    mHandleCmd=handleCmd;
    mHold=hold;
    _RETURN_VOID
}

bool YarpRunProcInfo::Signal(int signum)
{
    _BEGIN   
#if defined(WIN32)
    if (signum==SIGKILL)
    {
        if (mHandleCmd)
        {
            _CHECKPOINT
            bool ret=KILL(mHandleCmd);
            _RETURN(ret)
        }
    }
    else
    {
        if (mPidCmd)
        {
            _CHECKPOINT 
            bool ret=TERMINATE(mPidCmd);
            _RETURN(ret)
        }
    }
#else
    if (mPidCmd && !mHold)
    {
        _CHECKPOINT
        bool ret=SIGNAL(mPidCmd,signum);
        _RETURN(ret)
    }
#endif
    
    _RETURN(true)
}

bool YarpRunProcInfo::IsActive()
{
    _BEGIN
    if (!mPidCmd)
    {
        _RETURN(false)
    }
#if defined(WIN32)
    DWORD status;
    bool ret=(::GetExitCodeProcess(mHandleCmd,&status) && status==STILL_ACTIVE);
    _RETURN(ret)
#else
    bool ret=!kill(mPidCmd,0);
    _RETURN(ret)
#endif
}

bool YarpRunProcInfo::Clean()
{
    _BEGIN

#if !defined(WIN32)
    if (mPidCmd && waitpid(mPidCmd,NULL,WNOHANG)==mPidCmd)
    {
        _CHECKPOINT 
        fprintf(stderr,"CLEANUP cmd %d\n",mPidCmd);
        mPidCmd=0;
    } 

    if (mPidCmd)
    {
        _RETURN(false)
    }
#endif

    _RETURN(true)
}

YarpRunInfoVector::YarpRunInfoVector()
{
    _BEGIN

    m_nProcesses=0;
    
    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        m_apList[i]=0;
    }

    _RETURN_VOID
}

YarpRunInfoVector::~YarpRunInfoVector()
{
    _BEGIN
    mutex.wait();
    
    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        if (m_apList[i])
        {
            delete m_apList[i];
            m_apList[i]=NULL;
        }
    }

    mutex.post();
    _RETURN_VOID
}

bool YarpRunInfoVector::Add(YarpRunProcInfo *process)
{
    _BEGIN
    mutex.wait();
    _CHECKPOINT

    if (m_nProcesses>=MAX_PROCESSES)
    {
        fprintf(stderr,"ERROR: maximum process limit reached\n");
        _CHECKPOINT
        mutex.post();
        _RETURN(false)
    }

#if defined(WIN32)
    _CHECKPOINT
    if (hZombieHunter) TerminateThread(hZombieHunter,0);
    _CHECKPOINT
#endif

    m_apList[m_nProcesses++]=process;

#if defined(WIN32)
    _CHECKPOINT
    hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
    _CHECKPOINT
#endif

    _CHECKPOINT
    mutex.post();
    _RETURN(true)
}

int YarpRunInfoVector::Signal(yarp::os::ConstString& alias,int signum)
{
    _BEGIN
    mutex.wait();
    _CHECKPOINT

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;    

    _CHECKPOINT
    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias) && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i]; 
        }
    }
    
    _CHECKPOINT
    mutex.post();
    _CHECKPOINT

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL  %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    _RETURN(nKill)
}

int YarpRunInfoVector::Killall(int signum)
{
    _BEGIN
    mutex.wait();
    _CHECKPOINT

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;        

    for (int i=0; i<m_nProcesses; ++i)
    {    
        if (m_apList[i] && m_apList[i]->IsActive())
        {
            aKill[nKill++]=m_apList[i]; 
        }
    }

    _CHECKPOINT
    mutex.post();
    _CHECKPOINT

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    _RETURN(nKill)
}

#if defined(WIN32) 
void YarpRunInfoVector::GetHandles(HANDLE* &lpHandles,DWORD &nCount)
{
    _BEGIN
    mutex.wait();
    _CHECKPOINT

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

    _CHECKPOINT

    Pack();

    _CHECKPOINT

    for (int i=0; i<m_nProcesses; ++i)
    {
        lpHandles[nCount+i]=m_apList[i]->mHandleCmd;
    }

    nCount+=m_nProcesses;

    _CHECKPOINT
    mutex.post();
    _RETURN_VOID
}
#else
void YarpRunInfoVector::run() // zombie hunter
{
    _BEGIN

    char dummy;

    while(!isStopping())
    {
        _CHECKPOINT

        if (read(pipe_sigchld_handler_to_zombie_hunter[0],&dummy,1)!=1)
        {
            _RETURN_VOID
        }

        _CHECKPOINT
        mutex.wait();
        _CHECKPOINT

        for (int i=0; i<m_nProcesses; ++i)
        {
            if (m_apList[i] && m_apList[i]->Clean())
            {
                delete m_apList[i];
                m_apList[i]=NULL;
            }
        }

        _CHECKPOINT
    
        Pack();

        _CHECKPOINT
        mutex.post();
        _CHECKPOINT
    }

    _RETURN_VOID
}

#endif

yarp::os::Bottle YarpRunInfoVector::PS()
{
    _BEGIN

    mutex.wait();

    _CHECKPOINT

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

    _CHECKPOINT

    mutex.post();

    _RETURN(ps)
}

bool YarpRunInfoVector::IsRunning(yarp::os::ConstString &alias)
{
    _BEGIN

    mutex.wait();

    _CHECKPOINT

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                _CHECKPOINT
                mutex.post();
                _RETURN(true)
            }
            else
            {
                _CHECKPOINT
                mutex.post();
                _RETURN(false)
            }
        }
    }

    _CHECKPOINT
    
    mutex.post();

    _RETURN(false)
}

void YarpRunInfoVector::Pack()
{
    _BEGIN

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

    _RETURN_VOID
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
    _BEGIN

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

    _RETURN_VOID
}

bool YarpRunCmdWithStdioInfo::Clean()
{
    _BEGIN

#if defined(WIN32)
    if (mPidCmd)
    {
        mPidCmd=0;
     
        _CHECKPOINT
   
        if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
        if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
        if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
        if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

        mWriteToPipeStdinToCmd=0;
        mReadFromPipeStdinToCmd=0;
        mWriteToPipeCmdToStdout=0;
        mReadFromPipeCmdToStdout=0;

        _CHECKPOINT

        TERMINATE(mPidStdin);

        _CHECKPOINT

        TERMINATE(mPidStdout);

        _CHECKPOINT
        
        TerminateStdio();
    }

    _RETURN(false)

#else

    _CHECKPOINT

    if (mPidCmd && waitpid(mPidCmd,NULL,WNOHANG)==mPidCmd)
    {
        _CHECKPOINT 
        fprintf(stderr,"CLEANUP cmd %d\n",mPidCmd);
        mPidCmd=0;
    } 

    _CHECKPOINT
    
    if (mPidStdin && waitpid(mPidStdin,NULL,WNOHANG)==mPidStdin)
    {
        _CHECKPOINT 
        fprintf(stderr,"CLEANUP stdin %d\n",mPidStdin);
        mPidStdin=0;
    }    

    _CHECKPOINT

    if (mPidStdout && waitpid(mPidStdout,NULL,WNOHANG)==mPidStdout)
    {
        _CHECKPOINT
        fprintf(stderr,"CLEANUP stdout %d\n",mPidStdout);
        mPidStdout=0;
    } 

    if (mPidCmd && mPidStdin && mPidStdout)
    {
        _RETURN(false)
    }

    if (!mKillingStdio)
    {
        _CHECKPOINT

        mKillingStdio=true;
        
        if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
        if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
        if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
        if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);

        mWriteToPipeStdinToCmd=0;
        mReadFromPipeStdinToCmd=0;
        mWriteToPipeCmdToStdout=0;
        mReadFromPipeCmdToStdout=0;
        
        _CHECKPOINT
        TerminateStdio();
        _CHECKPOINT
    }

    _CHECKPOINT
    
    if (mPidCmd && !mKillingCmd)
    {
        kill(mPidCmd,SIGTERM);
        mKillingCmd=true;

        _CHECKPOINT
    }

    if (mPidStdin && !mKillingStdin)
    { 
        kill(mPidStdin,SIGTERM);
        mKillingStdin=true;

        _CHECKPOINT
    }

    if (mPidStdout && !mKillingStdout)
    {
        kill(mPidStdout,SIGTERM);
        mKillingStdout=true;

        _CHECKPOINT
    }
        
    if (mPidCmd || mPidStdin || mPidStdout)
    {
        _RETURN(false)
    }
    
    _RETURN(true)
#endif
}

void YarpRunCmdWithStdioInfo::TerminateStdio()
{
    _BEGIN

    if (mOn==mStdio)
    {
        _CHECKPOINT

        mStdioVector->Signal(mAlias,SIGTERM);
        //mStdioVector->Signal(mAlias,SIGKILL);
    }
    else
    {
        _CHECKPOINT

        yarp::os::Bottle msg;
        msg.fromString((yarp::os::ConstString("(killstdio ")+mAlias+")").c_str());

        yarp::os::Port port;
        port.open("...");
        bool connected=yarp::os::NetworkBase::connect(port.getName(), mStdio);

        _CHECKPOINT

        if (connected)
        {
            _CHECKPOINT

            port.write(msg);
            yarp::os::NetworkBase::disconnect(port.getName().c_str(),mStdio.c_str());
        }

        _CHECKPOINT
        port.close();
    }

    _RETURN_VOID
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
    _BEGIN

    TerminateParams* params=(TerminateParams*)lParam;

    DWORD dwID;
    GetWindowThreadProcessId(hwnd,&dwID) ;

    _CHECKPOINT

    if (dwID==params->dwID)
    {
        _CHECKPOINT
        params->nWin++;
        PostMessage(hwnd,WM_CLOSE,0,0);
    }

    _RETURN(TRUE)
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
    _BEGIN

    HANDLE hProc;

    // If we can't open the process with PROCESS_TERMINATE rights,
    // then we give up immediately.
    hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,dwPID);

    if (hProc==NULL)
    {
        _RETURN(false)
    }

    // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
    // matches your process's.

    _CHECKPOINT

    TerminateParams params(dwPID);

    _CHECKPOINT

    EnumWindows((WNDENUMPROC)TerminateAppEnum,(LPARAM)&params);

    _CHECKPOINT

    if (params.nWin)
    {
        _CHECKPOINT
        fprintf(stdout,"%d terminated by WM_CLOSE (sending anyway CTRL_C_EVENT/CTRL_BREAK_EVENT)\n",dwPID);
    }
    else
    {
        _CHECKPOINT
        //GenerateConsoleCtrlEvent(CTRL_C_EVENT,dwPID);
        //GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);
        fprintf(stdout,"%d terminated by CTRL_C_EVENT/CTRL_BREAK_EVENT\n",dwPID);    
    }

    GenerateConsoleCtrlEvent(CTRL_C_EVENT,dwPID);
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);

    fflush(stdout);

    _CHECKPOINT

    CloseHandle(hProc);

    _RETURN(true)
}

#endif

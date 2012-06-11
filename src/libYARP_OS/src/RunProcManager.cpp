// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007-2009 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/RunProcManager.h>

#ifndef YARP_HAS_ACE
#ifndef __APPLE__
#include <wait.h>
#else
#include <sys/wait.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include <yarp/os/impl/RunCheckpoints.h>

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
	        if (yarp::os::Run::aHandlesVector)
	        {
		        delete [] yarp::os::Run::aHandlesVector;
		        yarp::os::Run::aHandlesVector=0;
	        }
	        nCount=0;

	        yarp::os::Run::aHandlesVector=new HANDLE[yarp::os::Run::mProcessVector.Size()+yarp::os::Run::mStdioVector.Size()];

	        yarp::os::Run::mProcessVector.GetHandles(yarp::os::Run::aHandlesVector,nCount);
	        yarp::os::Run::mStdioVector.GetHandles(yarp::os::Run::aHandlesVector,nCount);

		    if (nCount)
		    {
                CHECKPOINT()
                
			    WaitForMultipleObjects(nCount,yarp::os::Run::aHandlesVector,FALSE,INFINITE);

                CHECKPOINT()
		    }
		    else
		    {
			    yarp::os::Run::hZombieHunter=NULL;
                CHECK_EXIT()
			    return 0;
		    }
	    }

        CHECK_EXIT()

	    return 0;
    }
#else
    #include <unistd.h>
    #include <fcntl.h>
    int CLOSE(int h)
    { 
        CHECK_ENTER("CLOSE");
        int ret=(close(h)==0);
        CHECK_EXIT()
        return ret; 
    }
    int SIGNAL(int pid,int signum,bool wait)
    {
        CHECK_ENTER("SIGNAL") 
        int ret=!kill(pid,signum);
        CHECKPOINT()
        if (wait) waitpid(pid,0,0);
        CHECK_EXIT()
        return ret;
    }
    void sigchild_handler(int sig)
    {
        CHECK_ENTER("sigchild_handler")   
        yarp::os::Run::CleanZombies();
        CHECK_EXIT()
    }
#endif

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
        bool ret=SIGNAL(mPidCmd,signum,false);
        CHECK_EXIT()
        return ret;
    }
#endif
    CHECK_EXIT()
    return true;
}

bool YarpRunProcInfo::IsActive()
{
    CHECK_ENTER("YarpRunProcInfo::IsActive");
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
    if (yarp::os::Run::hZombieHunter) TerminateThread(yarp::os::Run::hZombieHunter,0);
    CHECKPOINT()
#endif

    m_apList[m_nProcesses++]=process;

#if defined(WIN32)
    CHECKPOINT()
    yarp::os::Run::hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
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
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                aKill[nKill++]=m_apList[i]; 
            }
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
        if (m_apList[i])
        {
            if (m_apList[i]->IsActive())
            {
                aKill[nKill++]=m_apList[i]; 
            }
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
            fprintf(stderr,"CLEAN-UP %s (%d)\n",m_apList[i]->mAlias.c_str(),m_apList[i]->mPidCmd);
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
    //yarp::os::Time::delay(1.0); 

    CHECK_ENTER("YarpRunInfoVector::CleanZombies")
    mutex.wait();
    CHECKPOINT()

    YarpRunProcInfo **apZombie=new YarpRunProcInfo*[m_nProcesses];
    int nZombies=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        //if (m_apList[i] && waitpid(m_apList[i]->m_pid_cmd,0,WNOHANG)==m_apList[i]->m_pid_cmd)
        if (m_apList[i] && m_apList[i]->waitPid())
        {	            
            apZombie[nZombies++]=m_apList[i];
            m_apList[i]=NULL;
        }
    }

    CHECKPOINT()

    Pack();

    CHECKPOINT()

    mutex.post();

    CHECKPOINT()

    for (int z=0; z<nZombies; ++z)
    {
        fprintf(stderr,"CLEAN-UP %s (%d)\n",apZombie[z]->mAlias.c_str(),apZombie[z]->mPidCmd);
        fflush(stderr);

        apZombie[z]->Clean();
        delete apZombie[z];
    }

    delete [] apZombie;

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

    CHECK_EXIT()
}

void YarpRunCmdWithStdioInfo::Clean()
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::Clean")

    fflush(stdout);
    fflush(stderr);

    if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
    if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
    if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
    if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);
    mWriteToPipeStdinToCmd=mReadFromPipeStdinToCmd=0;
    mWriteToPipeCmdToStdout=mReadFromPipeCmdToStdout=0;

    yarp::os::ConstString strCmdUUID=yarp::os::ConstString("/")+mOn+"/"+int2String(getpid())+"/"+mAlias+"-"+int2String(mPidCmd);

    //yarp::os::NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(mStdioUUID+"/stdio:i").c_str());
    //yarp::os::NetworkBase::disconnect((mStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

    mPidCmd=0;

    if (mPidStdin)
    {
        CHECKPOINT()  
#if defined(WIN32)
        TERMINATE(mPidStdin);
#else
        SIGNAL(mPidStdin,SIGTERM,true);
#endif
        CHECKPOINT()

        fprintf(stderr,"TERMINATING stdin (%d)\n",mPidStdin);
        mPidStdin=0;
        fflush(stderr);
    }
    if (mPidStdout)
    {
        CHECKPOINT()
#if defined(WIN32)
        TERMINATE(mPidStdout);
#else
        SIGNAL(mPidStdout,SIGTERM,true);
#endif
        CHECKPOINT()

        fprintf(stderr,"TERMINATING stdout (%d)\n",mPidStdout);
        mPidStdout=0;
        fflush(stderr);
    }

    CHECKPOINT()
    TerminateStdio();
    CHECK_EXIT()
}

#if !defined(WIN32)
bool YarpRunCmdWithStdioInfo::waitPid()
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::waitPid")

    bool r0=waitpid(mPidCmd,0,WNOHANG)==mPidCmd;
    bool r1=waitpid(mPidStdin,0,WNOHANG)==mPidStdin;
    bool r2=waitpid(mPidStdout,0,WNOHANG)==mPidStdout;

    CHECK_EXIT()

    return r0 || r1 || r2;
}
#endif

void YarpRunCmdWithStdioInfo::TerminateStdio()
{
    CHECK_ENTER("YarpRunCmdWithStdioInfo::TerminateStdio")

    if (mOn==mStdio)
    {
        CHECKPOINT()

        mStdioVector->Signal(mAlias,SIGTERM);
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
        fprintf(stdout,"%d terminated by WM_CLOSE\n",dwPID);
    }
    else
    {
        CHECKPOINT()
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, dwPID);
        GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);
        fprintf(stdout,"%d terminated by CTRL_BREAK_EVENT\n",dwPID);    
    }

    fflush(stdout);

    CHECKPOINT()

    CloseHandle(hProc);

    CHECK_EXIT()

    return true;
}

#endif
